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
    public class SettingsContainer
    {
        public const string DEFAULT_FILE_NAME = "Settings.json";

        public static SettingsContainer GetSettingsContainer(string fileName)
        {
            if (File.Exists(fileName))
            {
                string jsonString = File.ReadAllText(fileName);
                return JsonSerializer.Deserialize<SettingsContainer>(jsonString);
            }
            return new SettingsContainer();
        }

        public void Save(string filename)
        {
            File.WriteAllText(filename, JsonSerializer.Serialize(this));
        }

        public int BaudRate { get; set; }
        public string ComPort { get; set; }
        public bool ConnectOnBoot { get; set; }

        public string BuzzerSound { get; set; }
        public string ScoreSound { get; set; }
    }
}
