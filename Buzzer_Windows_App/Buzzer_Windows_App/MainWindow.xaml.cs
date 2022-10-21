using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Buzzer_Windows_App
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        BuzzerSerialConnection moduleConnection;
        MessageDecoder decoder;
        SettingsContainer settingsContainer;
        ControllerSettingsContainer controllerSettings;

        public MainWindow()
        {
            InitializeComponent();
            moduleConnection = new BuzzerSerialConnection(); 
            moduleConnection.OnMessageReceived += ModuleConnection_OnMessageReceived;

            decoder = new MessageDecoder();
            decoder.OnButtonPressed += Decoder_OnButtonPressed;
            

            settingsContainer = SettingsContainer.GetSettingsContainer(SettingsContainer.DEFAULT_FILE_NAME);
            controllerSettings = new ControllerSettingsContainer(moduleConnection);
            decoder.OnConnectionDetected += controllerSettings.OnConnectionDetected;

            if (settingsContainer.ConnectOnBoot)
                moduleConnection.Start(settingsContainer.ComPort, settingsContainer.BaudRate);
        }

        private void Decoder_OnButtonPressed(object sender, uint e)
        {

        }

        private void ModuleConnection_OnMessageReceived(object sender, string message)
        {
            decoder.DecodeMessage(message);
        }

        private void StartButton_Click(object sender, RoutedEventArgs e)
        {

        }

        private void ReceiveMessage_Click(object sender, RoutedEventArgs e)
        {
        }

        private void Settings_Click(object sender, RoutedEventArgs e)
        {
            Settings settings = new Settings(settingsContainer);
            settings.ShowDialog();
            settingsContainer.Save(SettingsContainer.DEFAULT_FILE_NAME);
        }

        private void ConfigureControllers_Click(object sender, RoutedEventArgs e)
        {
            moduleConnection.Start(settingsContainer.ComPort, settingsContainer.BaudRate);
            ControllerSettings controllerWindow = new ControllerSettings(controllerSettings);
            controllerWindow.ShowDialog();
        }
    }
}
