using System;
using System.Windows;
using System.Windows.Controls;
using System.Text.RegularExpressions;
using GS1.Encoders;

namespace GS1.EncodersApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        // Events do nothing. For use when updating UI data
        private bool _disableEvents = false;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void ClearRender()
        {
            infoLabel.Content = "";
            errorMessageLabel.Content = "";
            syntaxTextBox.Text = "";
            dataStrTextBox.Text = "";
            aiDataTextBox.Text = "";
            dlURItextBox.Text = "";
            hriTextBox.Text = "";
        }

        public void LoadDataValues()
        {

            if (inputTextBox.Text.Equals(""))
                return;

            _disableEvents = true;

            dataStrTextBox.Text = App.gs1Encoder.DataStr;
            aiDataTextBox.Text = App.gs1Encoder.AIdataStr ?? "⧚ Not AI-based data ⧛";

            try
            {
                dlURItextBox.Text = App.gs1Encoder.GetDLuri(null);
            }
            catch (GS1EncoderDigitalLinkException e) {
                dlURItextBox.Text = "⧚ " + e.Message + " ⧛";
            }

            string[] hri = App.gs1Encoder.HRI;
            if (hri.Length > 0)
            {
                hriTextBox.Text = "";
                foreach (string ai in hri)
                {
                    hriTextBox.Text += ai + "\n";
                }
            }
            else
            {
                hriTextBox.Text = "⧚ Not AI-based data ⧛";
            }

            validateAIassociationsCheckBox.IsChecked = App.gs1Encoder.ValidateAIassociations;
            permitUnknownAIsCheckBox.IsChecked = App.gs1Encoder.PermitUnknownAIs;
            permitZeroSuppressedGTINinDLurisCheckBox.IsChecked = App.gs1Encoder.PermitZeroSuppressedGTINinDLuris;
            includeDataTitlesInHRIcheckBox.IsChecked = App.gs1Encoder.IncludeDataTitlesInHRI;

            _disableEvents = false;
        }


        private void ProcessInputButton_Click(object sender, RoutedEventArgs e)
        {

            ClearRender();

            try
            {
                if (inputTextBox.Text.Length > 0 && inputTextBox.Text[0] == '(')
                {  // Bracketed AI data
                    syntaxTextBox.Text = "Bracketed AI element string";
                    App.gs1Encoder.AIdataStr = inputTextBox.Text;
                }
                else if (inputTextBox.Text.Length > 0 && inputTextBox.Text[0] == ']')
                {  // Barcode scan data
                    syntaxTextBox.Text = "Barcode scan data with AIM symbology identifier";
                    var regex = new Regex(Regex.Escape("{GS}"));
                    string scandata = inputTextBox.Text;
                    scandata = regex.Replace(scandata, "\u001d");
                    App.gs1Encoder.ScanData = scandata;
                }
                else if (inputTextBox.Text.Length > 0 && inputTextBox.Text[0] == '^')
                {  // Unbracketed AI data with FNC1 in first
                    syntaxTextBox.Text = "Unbracketed AI element string";
                    App.gs1Encoder.DataStr = inputTextBox.Text;
                }
                else if (inputTextBox.Text.StartsWith("http://") || inputTextBox.Text.StartsWith("https://"))
                {  // GS1 Digital Link URI
                    syntaxTextBox.Text = "GS1 Digital Link URI";
                    App.gs1Encoder.DataStr = inputTextBox.Text;
                }
                else if (Regex.IsMatch(inputTextBox.Text, @"^\d+$"))
                {  // Plain number
                    syntaxTextBox.Text = "Plain data";

                    if (inputTextBox.Text.Length != 8 && inputTextBox.Text.Length != 12 && inputTextBox.Text.Length != 13 && inputTextBox.Text.Length != 14) {
                        errorMessageLabel.Content = "Invalid length for a GTIN-8, GTIN-12, GTIN-13 or GTIN-14";
                        return;
                    }

                    // Perform a checksum validation here, since the Syntax Engine validates only AI-based data
                    int parity = 0;
                    int weight = inputTextBox.Text.Length % 2 == 0 ? 3 : 1;
                    int i;
                    for (i = 0; i < inputTextBox.Text.Length - 1; i++)
                    {
                        parity += weight * (inputTextBox.Text[i] - '0');
                        weight = 4 - weight;
                    }
                    parity = (10 - parity % 10) % 10;

                    if (parity + '0' != inputTextBox.Text[i])
                    {
                        errorMessageLabel.Content = "Incorrect numeric check digit";
                        infoLabel.Content = "Plain data validation failed: " + inputTextBox.Text[0..^1] + "⧚" + inputTextBox.Text[i] + "⧛";
                        return;
                    }

                    syntaxTextBox.Text = "Plain GTIN-" + inputTextBox.Text.Length + " - converted to AI (01)...";
                    App.gs1Encoder.DataStr = "^01" + inputTextBox.Text.PadLeft(14, '0');
                }
                else
                {  // Plain, non-numeric (not used in GS1 system)
                    syntaxTextBox.Text = "Non-numeric plain data is not a valid GS1 syntax";
                    return;
                }
            }
            catch (Exception E)
            {
                if (!(E is GS1EncoderParameterException) && !(E is GS1EncoderScanDataException))
                    throw;

                errorMessageLabel.Content = "Error: " + E.Message;
                string markup = App.gs1Encoder.ErrMarkup;
                if (!markup.Equals("")) {
                    var regex = new Regex(Regex.Escape("|"));
                    markup = regex.Replace(markup, "⧚", 1);
                    markup = regex.Replace(markup, "⧛", 1);
                    infoLabel.Content = "AI content validation failed: " + markup;
                }
                return;
            }

            LoadDataValues();

        }

        private void GenericTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (_disableEvents) return;
            ClearRender();
        }

        private void GenericTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            if (_disableEvents) return;
            ClearRender();
            LoadDataValues();
        }

        private void PermitUnknownAIsCheckBox_Click(object sender, RoutedEventArgs e)
        {
            if (_disableEvents) return;
            ClearRender();
            App.gs1Encoder.PermitUnknownAIs = permitUnknownAIsCheckBox.IsChecked ?? false;
        }

        private void ValidateAIassociationsCheckBox_Click(object sender, RoutedEventArgs e)
        {
            if (_disableEvents) return;
            ClearRender();
            App.gs1Encoder.ValidateAIassociations = validateAIassociationsCheckBox.IsChecked ?? false;
        }

        private void IncludeDataTitlesInHRIcheckBox_Click(object sender, RoutedEventArgs e)
        {
            if (_disableEvents) return;
            ClearRender();
            App.gs1Encoder.IncludeDataTitlesInHRI = includeDataTitlesInHRIcheckBox.IsChecked ?? false;
        }

        private void PermitZeroSuppressedGTINinDLruisCheckBox_Click(object sender, RoutedEventArgs e)
        {
            if (_disableEvents) return;
            ClearRender();
            App.gs1Encoder.PermitZeroSuppressedGTINinDLuris = permitZeroSuppressedGTINinDLurisCheckBox.IsChecked ?? false;
        }

        private void InfoLabel_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            for (int i = 0; i < 10; i++)
            {
                try
                {
                    Clipboard.SetText(infoLabel.Content.ToString());
                    break;
                }
                catch { }
                System.Threading.Thread.Sleep(10);
            }
        }

        private void errorMessageLabel_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            for (int i = 0; i < 10; i++)
            {
                try
                {
                    Clipboard.SetText(errorMessageLabel.Content.ToString());
                    break;
                }
                catch { }
                System.Threading.Thread.Sleep(10);
            }
        }
    }
}
