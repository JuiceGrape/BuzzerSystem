﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.IO.Ports;

using System.IO;

namespace Buzzer_Windows_App
{
    public class BuzzerSerialConnection
    {
        public const char START_CHAR = '#';
        public const char END_CHAR = '$';
        public const char DELIM_CHAR = ':';
        public const string COMMAND_PHRASE = "command";

        private SerialPort Port;

        public event EventHandler<string> OnMessageReceived;

        StringBuilder receivedMessage = new StringBuilder();
        bool isBuilding = false;

        StreamWriter debugger;


        public BuzzerSerialConnection()
        {
            Port = new SerialPort();
            debugger = File.AppendText("debug.txt");
        }

        ~BuzzerSerialConnection()
        {
            if (Port != null)
            {
                Port.Close();
            }
        }

        public void Start(string comport, int baudRate)
        {
            if (Port.IsOpen)
                return;

            Port.PortName = comport;
            Port.BaudRate = baudRate;
            Port.DataReceived += Port_DataReceived;
            Port.Open();
        }

        private void Port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            
            while (Port.BytesToRead > 0)
            {
                char byteRead = (char)Port.ReadChar();
                debugger.Write(byteRead);
                if (byteRead == START_CHAR)
                {
                    isBuilding = true;
                    receivedMessage.Clear();
                }
                else if (isBuilding)
                {
                    if (byteRead == END_CHAR)
                    {
                        OnMessageReceived.Invoke(this, receivedMessage.ToString());
                        isBuilding = false;
                    }
                    else
                    {
                        receivedMessage.Append(byteRead);
                    }
                }
            }
            debugger.Flush();
        }

        private void SendString(string message)
        {
            if (Port.IsOpen)
            {
                Port.Write(message);
            }
        }

        public void SendMessage(UInt32 target, string message)
        {
            StringBuilder fullMessage = new StringBuilder();
            fullMessage.Append(START_CHAR);
            fullMessage.Append(target);
            fullMessage.Append(DELIM_CHAR);
            fullMessage.Append(message);
            fullMessage.Append(END_CHAR);
            SendString(fullMessage.ToString());
        }

        public void SendCommand(string command)
        {
            StringBuilder fullMessage = new StringBuilder();
            fullMessage.Append(START_CHAR);
            fullMessage.Append(COMMAND_PHRASE);
            fullMessage.Append(DELIM_CHAR);
            fullMessage.Append(command);
            fullMessage.Append(END_CHAR);
            SendString(fullMessage.ToString());
        }

        public void SetLed(bool on, UInt32 target)
        {
            string message = on ? "led_high" : "led_low";
            SendMessage(target, message);
        }
    }
}
