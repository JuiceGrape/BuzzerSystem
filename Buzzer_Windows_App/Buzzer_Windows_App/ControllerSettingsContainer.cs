using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Text.Json;
using System.Text.Json.Serialization;
using System.IO;

namespace Buzzer_Windows_App
{
    public struct PlayerController
    {
        public UInt64 ID;
        public string Name;
    }

    public class ControllerSettingsContainer
    {
        public PlayerController[] players;

        public List<UInt64> ConnectedDevices;

        public BuzzerSerialConnection connection;

        public ControllerSettingsContainer(BuzzerSerialConnection buzzerConnection)
        {
            players = new PlayerController[3];
            ConnectedDevices = new List<UInt64>();
            connection = buzzerConnection;
        }

        public void OnConnectionDetected(object sender, UInt64 e)
        {
            if (!ConnectedDevices.Contains(e))
            {
                ConnectedDevices.Add(e);
            }
        }

        public void RefreshConnectedDevices()
        {
            ConnectedDevices.Clear();
            connection.SendCommand("getConnected");
        }
    }
}
