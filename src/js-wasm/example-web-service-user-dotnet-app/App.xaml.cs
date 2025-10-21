using System;
using System.Windows;
using System.Runtime.InteropServices;

namespace GS1.ExampleWebServiceUserDotnetApp
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {

        const uint ATTACH_PARENT_PROCESS = 0x0ffffffff;  // default value if not specifying a process ID

        private void App_Startup(object sender, StartupEventArgs e)
        {

            MainWindow mw = new MainWindow();
            mw.Show();

//            mw.LoadDataValues();

        }

    }

}
