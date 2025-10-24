using System;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Threading;

namespace GS1.ExampleWebServiceUserDotnetApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        /*
         *  Barcode scanner input detection
         * 
         */
        private const char SCAN_START_CHAR = ']';                // Scans start with an AIM symbology identifier
        private const int SCAN_MIN_LENGTH = 4;                   // Minimum valid scan length
        private const int SCAN_CHAR_GAP_MS = 50;                 // Max transfer delay between characters
        private const int SCAN_END_TIMEOUT_MS = 100;             // Timeout after last char indicating scan complete

        // --- Visual feedback constants ---
        private static readonly Color SCAN_FLASH_COLOR = Colors.LightGreen;
        private const int SCAN_FLASH_DURATION_MS = 300;          // Flash duration

        // --- Barcode detection state ---
        private readonly StringBuilder _scanBuffer = new();
        private DateTime _lastKeystroke = DateTime.MinValue;
        private readonly DispatcherTimer _scaneEndTimer;
        private bool _scanInProgress = false;

        // Events do nothing. For use when updating UI data
        private readonly bool _disableEvents = false;

        public MainWindow()
        {

            _disableEvents = true;
            InitializeComponent();
            _disableEvents = false;
            UpdateHTTPrequestText();

            // Scanner input detection setup
            this.PreviewTextInput += MainWindow_PreviewTextInput;

            _scaneEndTimer = new DispatcherTimer
            {
                Interval = TimeSpan.FromMilliseconds(SCAN_END_TIMEOUT_MS)
            };
            _scaneEndTimer.Tick += BarcodeEndTimer_Tick;

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

        /*
         *  SendHTTPrequestButton_Click below demonstrates sending raw bytes of the
         *  HTTP request buffer over a socket for ease of demonstration purposes only.
         *
         *  In a real application you might instead simply build a URL and pass it to
         *  a higher-level API, for example:
         *
         *    using var client = new HttpClient();
         *    var resp = await client.GetAsync(url);
         *    var http_body = await resp.Content.ReadAsStringAsync();
         *    ...
         *         
         */
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
                try
                {
                    using var doc = JsonDocument.Parse(http_body);
                    var root = doc.RootElement;

                    // Extract fields from the error JSON response body
                    string error = root.TryGetProperty("error", out var errElem) ? errElem.GetString() : null;
                    string markup = root.TryGetProperty("markup", out var markupElem) ? markupElem.GetString() : null;

                    if (!string.IsNullOrEmpty(error))
                    {
                        var sb = new StringBuilder();
                        sb.Append("ERROR:\t\t" + error);
                        if (!string.IsNullOrEmpty(markup))
                        {
                            sb.Append("\nMARKUP:\t" + markup);
                        }
                        barcodeMessageTextBox.Text = sb.ToString();
                        return;
                    }
                }
                catch
                {
                    // Fall through
                }
                barcodeMessageTextBox.Text = $"⧚ Response is not decodable ⧛";
                return;
            }

            // If we reach here, we have a 200 OK response and expect a valid JSON body
            using var doc200 = JsonDocument.Parse(http_body);
            var root200 = doc200.RootElement;

            // Extract fields from JSON response body
            var dataStr = root200.TryGetProperty("dataStr", out var dataStrElem) ? dataStrElem.GetString() : null;
            var aiDataStr = root200.TryGetProperty("aiDataStr", out var aiDataStrElem) ? aiDataStrElem.GetString() : null;
            var dlURI = root200.TryGetProperty("dlURI", out var dlURIelem) ? dlURIelem.GetString() : null;
            var dlURIerror = root200.TryGetProperty("dlURIerror", out var dlURIerrorElem) ? dlURIerrorElem.GetString() : null;

            string[] hri = root200.TryGetProperty("hri", out var hriElem) && hriElem.ValueKind == JsonValueKind.Array
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
                httpRequestTextBox.Text = "";
                infoLabel.Content = "Enter scan data or scan a barcode to continue";
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


        /*
         *  UI event handlers
         * 
         */

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


        /*
         *  Barcode scanner input detection logic from here on...
         * 
         */

        private void MainWindow_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {

            var now = DateTime.Now;

            // Reset buffer if long gap indicative of human typing
            if ((now - _lastKeystroke).TotalMilliseconds > SCAN_CHAR_GAP_MS)
            {
                _scanBuffer.Clear();
                _scanInProgress = false;
            }

            _lastKeystroke = now;

            // Start new potential scan input
            if (e.Text.Length == 1 && e.Text[0] == SCAN_START_CHAR)
            {
                _scanBuffer.Clear();
                _scanInProgress = true;
            }

            if (_scanInProgress)
            {
                _scanBuffer.Append(e.Text);

                // Restart end of scan timer
                _scaneEndTimer.Stop();
                _scaneEndTimer.Start();
            }

        }

        private void BarcodeEndTimer_Tick(object sender, EventArgs e)
        {

            _scaneEndTimer.Stop();

            if (!_scanInProgress)
                return;

            _scanInProgress = false;
            string scanned = _scanBuffer.ToString().Trim();
            _scanBuffer.Clear();

            if (scanned.Length < SCAN_MIN_LENGTH)
                return;

            scanDataTextBox.Clear();
            scanDataTextBox.Text = scanned;
            scanDataTextBox.Focus();
            scanDataTextBox.CaretIndex = scanDataTextBox.Text.Length;

            ClearRender();
            UpdateHTTPrequestText();

            FlashScanFeedback();

            if (AutoSubmitScansCheckBox.IsChecked ?? false)
            {
                SendHTTPrequestButton_Click(null, null);
            }

        }

        private void FlashScanFeedback()
        {

            var originalBrush = scanDataTextBox.Background as SolidColorBrush ?? new SolidColorBrush(Colors.White);
            var flashBrush = new SolidColorBrush(originalBrush.Color);
            scanDataTextBox.Background = flashBrush;

            var flashAnimation = new ColorAnimation
            {
                From = SCAN_FLASH_COLOR,
                To = originalBrush.Color,
                Duration = TimeSpan.FromMilliseconds(SCAN_FLASH_DURATION_MS),
                EasingFunction = new QuadraticEase { EasingMode = EasingMode.EaseOut }
            };

            flashBrush.BeginAnimation(SolidColorBrush.ColorProperty, flashAnimation);

        }

    }
}
