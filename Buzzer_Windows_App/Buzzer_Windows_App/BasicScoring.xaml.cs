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
    /// Interaction logic for BasicScoring.xaml
    /// </summary>
    public partial class BasicScoring : Window
    {
        MessageDecoder m_decoder;
        ControllerSettingsContainer m_controllers;
        SettingsContainer m_settings;

        PlayerContainer[] players;

        bool HasBuzzed = false;

        public BasicScoring(ControllerSettingsContainer controllers, MessageDecoder decoder, SettingsContainer settings)
        {
            InitializeComponent();

            m_decoder = decoder;
            m_controllers = controllers;
            m_settings = settings;

            players = new PlayerContainer[3];
            players[0] = new PlayerContainer(PlayerName1, PlayerScore1, m_controllers.players[0]);
            players[1] = new PlayerContainer(PlayerName2, PlayerScore2, m_controllers.players[1]);
            players[2] = new PlayerContainer(PlayerName3, PlayerScore3, m_controllers.players[2]);

            m_decoder.OnButtonPressed += M_decoder_OnButtonPressed;

        }

        private void M_decoder_OnButtonPressed(object sender, UInt64 e)
        {
            if (HasBuzzed)
                return;

            this.Dispatcher.Invoke(() =>
            {
                foreach (var player in players)
                {
                    if (player.GetID() == e)
                    {
                        player.SetBuzzer(new SolidColorBrush(Color.FromRgb(255, 255, 0)));
                        HasBuzzed = true;
                        m_controllers.connection.SetLed(true, e);
                        Settings.PlaySound(m_settings.BuzzerSound);
                        return;
                    }
                }
            });

            
        }

        private void IncreaseScore(int player)
        {
            Settings.PlaySound(m_settings.ScoreSound);
            players[player].IncreaseScore(null, null);
        }

        private void Reset()
        {
            this.Dispatcher.Invoke(() =>
            {
                foreach (var player in players)
                {
                    player.SetBuzzer(new SolidColorBrush(Color.FromArgb(0, 0, 0, 0)));
                    m_controllers.connection.SetLed(false, player.GetID());
                }
                HasBuzzed = false;
            });
            
        }

        private void OnKeyDown(object sender, KeyEventArgs e)
        {
            switch(e.Key)
            {
                case Key.Escape:
                    Close();
                    break;
                case Key.D1:
                    IncreaseScore(0);
                    break;
                case Key.D2:
                    IncreaseScore(1);
                    break;
                case Key.D3:
                    IncreaseScore(2);
                    break;
                case Key.Tab:
                    Reset();
                    break;
            }
        }


        private class PlayerContainer
        {
            Label m_name;
            Button m_score;
            PlayerController m_player;

            int score = 0;

            public PlayerContainer(Label name, Button score, PlayerController player)
            {
                m_score = score;
                m_name = name;
                m_player = player;

                m_name.Content = player.Name;
                m_score.Click += IncreaseScore;
                m_score.MouseRightButtonDown += DecreaseScore;
            }

            ~PlayerContainer()
            {
                m_score.Click -= IncreaseScore;
                m_score.MouseRightButtonDown -= DecreaseScore;
            }

            public UInt64 GetID()
            {
                return m_player.ID;
            }

            public void SetBuzzer(Brush backgroundColour)
            {
                m_name.Background = backgroundColour;
            }

            public void IncreaseScore(object sender, RoutedEventArgs e)
            {
                score++;
                m_score.Content = score;
            }

            public void DecreaseScore(object sender, RoutedEventArgs e)
            {
                score--;
                m_score.Content = score;
            }
        }

    }
}
