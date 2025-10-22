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

        private void App_Startup(object sender, StartupEventArgs e)
        {
            MainWindow mw = new MainWindow();
            mw.Show();
        }

    }

}
