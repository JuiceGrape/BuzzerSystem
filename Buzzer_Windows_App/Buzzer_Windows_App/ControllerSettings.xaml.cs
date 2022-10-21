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
using System.Threading;

namespace Buzzer_Windows_App
{
    /// <summary>
    /// Interaction logic for ControllerSettings.xaml
    /// </summary>
    public partial class ControllerSettings : Window
    {
        ControllerSettingsContainer controllerContainer;

        List<Task<bool>> tasks = new List<Task<bool>>();

        public ControllerSettings(ControllerSettingsContainer container)
        {
            InitializeComponent();
            controllerContainer = container;
            FillComboBoxes();

            Player1Name.Text = controllerContainer.players[0].Name;
            Player1Box.SelectedIndex = Player1Box.Items.IndexOf(controllerContainer.players[0].ID);

            Player2Name.Text = controllerContainer.players[1].Name;
            Player2Box.SelectedIndex = Player2Box.Items.IndexOf(controllerContainer.players[1].ID);

            Player3Name.Text = controllerContainer.players[2].Name;
            Player3Box.SelectedIndex = Player3Box.Items.IndexOf(controllerContainer.players[2].ID);

            Closed += OnExit;
        }

        public void OnExit(object sender, EventArgs e)
        {
            controllerContainer.players[0].Name = Player1Name.Text;
            controllerContainer.players[0].ID = Convert.ToUInt32(Player1Box.SelectedItem);

            controllerContainer.players[1].Name = Player2Name.Text;
            controllerContainer.players[1].ID = Convert.ToUInt32(Player2Box.SelectedItem);

            controllerContainer.players[2].Name = Player3Name.Text;
            controllerContainer.players[2].ID = Convert.ToUInt32(Player3Box.SelectedItem);

            foreach (UInt32 val in controllerContainer.ConnectedDevices)
            {
                controllerContainer.connection.SetLed(false, val);
            }
        }

        private void FillComboBoxes()
        {
            Player1Box.Items.Clear();
            Player2Box.Items.Clear();
            Player3Box.Items.Clear();

            foreach (UInt32 val in controllerContainer.ConnectedDevices)
            {
                Player1Box.Items.Add(val);
                Player2Box.Items.Add(val);
                Player3Box.Items.Add(val);
            }
        }

        private void RefreshButton_Click(object sender, RoutedEventArgs e)
        {
            controllerContainer.RefreshConnectedDevices();
            Thread.Sleep(100);
            FillComboBoxes();
        }

        static bool Led1 = false;
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (Player1Box.SelectedItem == null)
                return;
            
            Led1 = !Led1;
            controllerContainer.connection.SetLed(Led1, Convert.ToUInt32(Player1Box.SelectedItem));
            
        }

        static bool Led2 = false;
        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            
            if (Player2Box.SelectedItem == null)
                return;

            Led2 = !Led2;
            controllerContainer.connection.SetLed(Led2, Convert.ToUInt32(Player2Box.SelectedItem));
            
        }

        static bool Led3 = false;
        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            if (Player3Box.SelectedItem == null)
                return;

            Led3 = !Led3;
            controllerContainer.connection.SetLed(Led3, Convert.ToUInt32(Player3Box.SelectedItem));
            
        }
    }
}
