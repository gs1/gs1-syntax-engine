using System;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Runtime.InteropServices;
using GS1.Encoders;

namespace GS1.EncodersApp
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {

        public static GS1Encoder gs1Encoder = CreateEncoder();

        // Seed the GS1 Syntax Dictionary into the app's writable local storage
        // from the copy shipped alongside the app on first run, then load it from
        // there. A real application could replace this file with a newer revision
        // of the Syntax Dictionary without rebuilding. The AI table embedded in
        // the library is used as a fallback while no local copy is available.
        private static GS1Encoder CreateEncoder()
        {
            GS1Encoder.InitOptions options = new GS1Encoder.InitOptions { FallbackOnSyndictError = true };
            string seed = Path.Combine(AppContext.BaseDirectory, "gs1-syntax-dictionary.txt");
            try
            {
                string dir = Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                    "GS1 Encoders App");
                Directory.CreateDirectory(dir);
                string local = Path.Combine(dir, "gs1-syntax-dictionary.txt");
                if (!File.Exists(local) && File.Exists(seed))
                    File.Copy(seed, local);
                options.SyntaxDictionary = File.Exists(local) ? local : (File.Exists(seed) ? seed : null);
            }
            catch (IOException)
            {
                if (File.Exists(seed))
                    options.SyntaxDictionary = seed;
            }

            GS1Encoder encoder = new GS1Encoder(options);
            if (encoder.InitFallbackWarning != null)
                Trace.TraceWarning("GS1 Syntax Dictionary not loaded; using embedded AI table: " + encoder.InitFallbackWarning);
            return encoder;
        }

        /*
         *  Native functions so that we can write our version information to the console (for CI)
         *
         */
        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool AttachConsole(uint dwProcessId);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool FreeConsole();

        const uint ATTACH_PARENT_PROCESS = 0x0ffffffff;  // default value if not specifying a process ID

        private void App_Startup(object sender, StartupEventArgs e)
        {

            string[] arguments = Environment.GetCommandLineArgs();
            if (arguments.Length == 2 && arguments[1].Equals("--version"))
            {
                AttachConsole(ATTACH_PARENT_PROCESS);
                Console.WriteLine("pinvoked DLL version: " + gs1Encoder.Version);
                FreeConsole();
                Shutdown(0);
                return;
            }

            MainWindow mw = new MainWindow
            {
                Title = "GS1 Barcode Syntax Engine GUI demo | Library release: " + gs1Encoder.Version
            };
            mw.Show();

            mw.LoadDataValues();

        }

    }

}
