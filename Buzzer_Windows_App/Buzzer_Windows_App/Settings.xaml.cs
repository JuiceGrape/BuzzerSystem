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
using System.Windows.Shapes;

namespace Buzzer_Windows_App
{
    /// <summary>
    /// Interaction logic for Settings.xaml
    /// </summary>
    public partial class Settings : Window
    {
        SettingsContainer settingsContainer;
        public Settings(SettingsContainer container)
        {
            InitializeComponent();
            settingsContainer = container;

            ComportTextBox.Text = settingsContainer.ComPort;
            BaudrateTextBox.Text = settingsContainer.BaudRate.ToString();
            StartupCheckbox.IsChecked = settingsContainer.ConnectOnBoot;
            Closed += Settings_Closed;
        }

        private void Settings_Closed(object sender, EventArgs e)
        {
            settingsContainer.ComPort = ComportTextBox.Text;
            settingsContainer.BaudRate = int.Parse(BaudrateTextBox.Text);
            settingsContainer.ConnectOnBoot = StartupCheckbox.IsChecked.Value;
        }
    }
}
