using System;
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

        public static GS1Encoder gs1Encoder = new GS1Encoder();

        /*
         *  Native functions so that we can write our version information to the console (for CI)
         *
         */
        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool AttachConsole(uint dwProcessId);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool FreeConsole();

        const uint ATTACH_PARENT_PROCESS = 0x0ffffffff;  // default value if not specifing a process ID

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
                Title = "GS1 Syntax Engine GUI demo | Library release: " + gs1Encoder.Version
            };
            mw.Show();

            mw.LoadDataValues();

        }

    }

}
