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
        public static void PlaySound(string soundFile)
        {
            if (soundFile == null)
                return;

            player.Volume = 1.0;
            player.Pause();
            player.Open(new Uri(SoundLocationPrefix + soundFile, UriKind.RelativeOrAbsolute));

            player.Position = TimeSpan.Zero;
            player.Play();
        }

        static MediaPlayer player = new MediaPlayer();

        public const string SoundLocationPrefix = "Assets/SoundEffects/";
        public static readonly string[] sounds =
        {
            "short_bell.mp3",
            "short_pling.mp3",
            "test.mp3"
        };

        SettingsContainer settingsContainer;
        public Settings(SettingsContainer container)
        {
            InitializeComponent();
            settingsContainer = container;

            ComportTextBox.Text = settingsContainer.ComPort;
            BaudrateTextBox.Text = settingsContainer.BaudRate.ToString();
            StartupCheckbox.IsChecked = settingsContainer.ConnectOnBoot;
            Closed += Settings_Closed;

            int index = 0;
            foreach(var item in sounds) 
            {
                BuzzerCombobox.Items.Add(item);
                if (settingsContainer.BuzzerSound != null && settingsContainer.BuzzerSound.Equals(item))
                    BuzzerCombobox.SelectedIndex = index;

                PointsCombobox.Items.Add(item);
                if (settingsContainer.ScoreSound != null && settingsContainer.ScoreSound.Equals(item))
                    PointsCombobox.SelectedIndex = index;

                index++;
            }

            BuzzerCombobox.SelectionChanged += SoundCombobox_SelectionChanged;
            PointsCombobox.SelectionChanged += SoundCombobox_SelectionChanged;
        }


        private void Settings_Closed(object sender, EventArgs e)
        {
            settingsContainer.ComPort = ComportTextBox.Text;
            settingsContainer.BaudRate = int.Parse(BaudrateTextBox.Text);
            settingsContainer.ConnectOnBoot = StartupCheckbox.IsChecked.Value;
            settingsContainer.ScoreSound = PointsCombobox.SelectedItem?.ToString();
            settingsContainer.BuzzerSound = BuzzerCombobox.SelectedItem?.ToString();
        }

        private void SoundCombobox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            PlaySound(e.AddedItems[0] as string);
        }
    }
}
