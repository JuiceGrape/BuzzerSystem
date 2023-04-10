using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Buzzer_Windows_App
{
    public class MessageDecoder
    {
        public const string CONNECT_COMMAND = "connect";
        public const string BUTTON_COMMAND = "press";

        public event EventHandler<UInt64> OnButtonPressed;
        public event EventHandler<UInt64> OnConnectionDetected;

        public void DecodeMessage(string message)
        {
            var splitMessage = message.Split(BuzzerSerialConnection.DELIM_CHAR);
            if (splitMessage.Length != 2)
                return;

            UInt64 source = UInt64.Parse(splitMessage[0]);
            string command = splitMessage[1];

            switch(command)
            {
                case CONNECT_COMMAND:
                    OnConnectionDetected.Invoke(this, source);
                    break;
                case BUTTON_COMMAND:
                    OnButtonPressed.Invoke(this, source);
                    break;
                default:
                    throw new SystemException("Unkown message: " + message);
                    break;
            }
        }
    }
}
