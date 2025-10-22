using System;
using System.Linq;
using System.Linq.Expressions;
using System.Security.Policy;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using static System.Net.WebRequestMethods;

namespace GS1.ExampleWebServiceUserDotnetApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        // Events do nothing. For use when updating UI data
        private readonly bool _disableEvents = false;

        public MainWindow()
        {
            _disableEvents = true;
            InitializeComponent();
            _disableEvents = false;
            UpdateHTTPrequestText();
        }

        private void ClearRender()
        {
            infoLabel.Content = "";
            httpResponseTextBox.Text = "";
            barcodeMessageTextBox.Text = "";
            elementStringTextBox.Text = "";
            dlURItextBox.Text = "";
            extractedAisTextBox.Text = "";
        }

        private async void SendHTTPrequestButton_Click(object sender, RoutedEventArgs e)
        {
            ClearRender();

            if (httpRequestTextBox.Text == "")
                return;

            infoLabel.Content = "Sending HTTP request...";

            // Send HTTP request and get response
            string httpResult = "";
            try
            {

                var sw = System.Diagnostics.Stopwatch.StartNew();

                using var tcp = new System.Net.Sockets.TcpClient();
                Uri uri = new(endpointTextBox.Text);
                await tcp.ConnectAsync(uri.Host, uri.IsDefaultPort ? 80 : uri.Port);

                using var netStream = tcp.GetStream();
                var requestBytes = System.Text.Encoding.ASCII.GetBytes(httpRequestTextBox.Text + "\r\n\r\n");
                await netStream.WriteAsync(requestBytes.AsMemory(), System.Threading.CancellationToken.None);
                await netStream.FlushAsync();

                var ms = new System.IO.MemoryStream();
                var buffer = new byte[8192];
                int read;
                while ((read = await netStream.ReadAsync(buffer.AsMemory(), System.Threading.CancellationToken.None)) > 0)
                {
                    ms.Write(buffer, 0, read);
                }

                httpResult = System.Text.Encoding.UTF8.GetString(ms.ToArray());

                sw.Stop();
                infoLabel.Content = $"HTTP response received in {sw.Elapsed.TotalMilliseconds:F3}ms";
            }
            catch (Exception ex)
            {
                infoLabel.Content = "Request failed";
                httpResponseTextBox.Text = "Error: " + ex.Message;
                return;
            }

            httpResponseTextBox.Text = httpResult.TrimEnd();

            // Parse HTTP response and check status code
            var split = httpResult.Split(new string[] { "\r\n\r\n", "\n\n" }, 2, StringSplitOptions.None);
            var (headers, http_body) = (split[0], split.Length > 1 ? split[1].Trim() : string.Empty);

            var http_code = int.TryParse(
                Regex.Match(headers, @"^HTTP/\d\.\d\s+(\d{3})", RegexOptions.Multiline).Groups[1].Value,
                out var code
            ) ? code : 0;

            if (http_code != 200)
            {
                barcodeMessageTextBox.Text = $"⧚ Decoded data not received ⧛";
                return;
            }

            // If we reach here, we have a 200 OK response and expect a valid JSON body
            using var doc = JsonDocument.Parse(http_body);
            var root = doc.RootElement;

            // Extract fields from JSON response body
            var dataStr = root.TryGetProperty("dataStr", out var dataStrElem) ? dataStrElem.GetString() : null;
            var aiDataStr = root.TryGetProperty("aiDataStr", out var aiDataStrElem) ? aiDataStrElem.GetString() : null;
            var dlURI = root.TryGetProperty("dlURI", out var dlURIelem) ? dlURIelem.GetString() : null;
            var dlURIerror = root.TryGetProperty("dlURIerror", out var dlURIerrorElem) ? dlURIerrorElem.GetString() : null;

            string[] hri = root.TryGetProperty("hri", out var hriElem) && hriElem.ValueKind == JsonValueKind.Array
                ? hriElem.EnumerateArray()
                    .Select(e => e.ValueKind == JsonValueKind.String ? e.GetString() : e.ToString())
                    .Where(s => !string.IsNullOrEmpty(s))
                    .ToArray()
                : Array.Empty<string>();

            // Display extracted data in UI
            barcodeMessageTextBox.Text = dataStr;
            elementStringTextBox.Text = aiDataStr ?? "⧚ Not AI-based data ⧛";
            dlURItextBox.Text = dlURI ?? ("⧚ " + (dlURIerror ?? "Not AI-based data") + " ⧛");
            extractedAisTextBox.Text = hri.Length > 0 ? string.Join("\n", hri) : "⧚ Not AI-based data ⧛";

        }

        private void UpdateHTTPrequestText()
        {

            if (scanDataTextBox.Text == "")
            {
                infoLabel.Content = "Enter scan data to continue";
                return;
            }

            Uri uri;
            try
            {
                uri = new Uri(endpointTextBox.Text);
            } catch (UriFormatException E)
            {
                infoLabel.Content = "Error: Invalid endpoint URI - " + E.Message;
                return;
            }
            string hostHeader = uri.IsDefaultPort ? uri.Host : $"{uri.Host}:{uri.Port}";
            string path = uri.AbsolutePath;

            var sb = new StringBuilder();
            sb.Append("GET " + path);

            // Replace all literal occurrences of the indicator with the GS (U+001D) character.
            string indicator = fnc1indicatorTextBox.Text ?? "";
            string scandata = scanDataTextBox.Text ?? "";

            if (!string.IsNullOrEmpty(indicator))
            {
                // Literal replacement of all instances of `indicator` with the Group Separator.
                scandata = scandata.Replace(indicator, "\u001d");
            }

            sb.Append("?input=" + Uri.EscapeDataString(scandata));

            if (includeDataTitlesInHRIcheckBox.IsChecked ?? false)
                sb.Append("&includeDataTitlesInHRI");
            if (permitUnknownAIsCheckBox.IsChecked ?? false)
                sb.Append("&permitUnknownAIs");
            if (!(validateAIassociationsCheckBox.IsChecked ?? false))
                sb.Append("&noValidateRequisiteAIs");
            if (permitZeroSuppressedGTINinDLurisCheckBox.IsChecked ?? false)
                sb.Append("&permitZeroSuppressedGTINinDLuris");

            sb.Append(" HTTP/1.0\r\n");
            sb.Append("Host: " + hostHeader + "\r\n");
            sb.Append("Accept: application/json\r\n");
            sb.Append("Connection: close\r\n");

            httpRequestTextBox.Text = sb.ToString().TrimEnd();

            infoLabel.Content = "Click 'Send HTTP request' to decode the scan data";

        }

        private void GenericTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (_disableEvents) return;
            ClearRender();
            UpdateHTTPrequestText();
        }

        private void GenericCheckBox_Click(object sender, RoutedEventArgs e)
        {
            if (_disableEvents) return;
            ClearRender();
            UpdateHTTPrequestText();
        }

    }
}
