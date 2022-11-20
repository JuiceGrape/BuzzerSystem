#include <Arduino.h>
#include <vector>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

#define START_CHAR '#'
#define END_CHAR '$'
#define DELIM_CHAR ':'
#define MESSAGE_FORMAT "#%u:%s$"
#define SERIAL_BUFFER_LENGTH 256

char m_SerialBuffer[SERIAL_BUFFER_LENGTH + 1];
int m_BufferWriteLocation = 0;
int m_bufferReadLocation = 0;

int m_MsgStart = -1; // Negative for invalid value
int m_MsgEnd = -1;	 // Negative for invalid value

uint8_t mac[6] {0xDE, 0xAD, 0xBE, 0xEF, 0x12, 0x53};


std::vector<esp_now_peer_info> connectedDevices;

bool m_LedOn = false;

#define LED_BUILTIN 2

void SetLed(bool LedOn)
{
	if (LedOn)
	{
		digitalWrite(LED_BUILTIN, LOW);
		m_LedOn = true;
	}
	else
	{
		digitalWrite(LED_BUILTIN, HIGH);
		m_LedOn = false;
	}
}

void BlinkLed()
{
	SetLed(!m_LedOn);
}

void HandleSerialMessage(const String& message)
{
	String first;
	String second;
	bool delimReached = false;

	for (int i = 0; i < message.length(); i++)
	{
		if (message[i] == DELIM_CHAR)
		{
			delimReached = true;
			continue;
		}

		if (!delimReached)
		{
			first += message[i];
		}
		else
		{
			second += message[i];
		}
	}

	if (first.equals("command"))
	{
		if (second.equals("getConnected"))
		{
			// for (const auto& val : connectedDevices)
			// {
			// 	Serial.printf(MESSAGE_FORMAT, val, "connect");
			// }
		}
		else if (second.equals("test"))
		{
			Serial.printf(MESSAGE_FORMAT, 420, "69");
		}
	}
	else
	{
		// uint32_t targetID = strtoul(first.c_str(), NULL, 0);
		// m_Mesh.sendSingle(targetID, second);
	}
}

void receiveSerialData()
{
	int readBytes = 0;
	while (Serial.available() > 0)
	{
		m_SerialBuffer[m_BufferWriteLocation] = Serial.read();
		m_BufferWriteLocation++;
		if (m_BufferWriteLocation == SERIAL_BUFFER_LENGTH) // Loop around
		{
			m_BufferWriteLocation = 0;
		}
		readBytes++;
		if (readBytes >= 5) //Only read 5 bytes at a time so the mesh doesn't hang itself
		{
			return;
		}
	}

	while (m_bufferReadLocation != m_BufferWriteLocation)
	{
		if (m_SerialBuffer[m_bufferReadLocation] == START_CHAR)
		{
			m_MsgStart = m_bufferReadLocation;
		}
		else if (m_SerialBuffer[m_bufferReadLocation] == END_CHAR)
		{
			if (m_MsgStart == -1)
			{
				Serial.println("Detected error: End before start");
			}
			else
			{
				m_MsgEnd = m_bufferReadLocation;
			}
			
		}

		if (m_MsgStart != -1 && m_MsgEnd != -1) // Message found, processing
		{
			String receivedMessage;
			for (int i = m_MsgStart + 1; i != m_MsgEnd; i++)
			{
				if (i == SERIAL_BUFFER_LENGTH) // Loop around
				{
					i = 0;
				}
				receivedMessage += m_SerialBuffer[i];
			}
			HandleSerialMessage(receivedMessage);

			m_MsgStart = -1;
			m_MsgEnd = -1;
		}

		m_bufferReadLocation++;
		if (m_bufferReadLocation == SERIAL_BUFFER_LENGTH) // Loop around
		{
			m_bufferReadLocation = 0;
		}
	}
}

void AddMac(const uint8_t * mac)
{
	int indexToDelete = -1;
	for (const auto& val : connectedDevices)
	{
		bool match = true;
		for (int i = 0; i < 6; i++)
		{
			if (val.peer_addr[i] != mac[i])
			{
				match = false;
				break;
			}
		}
	}

	connectedDevices.push_back(esp_now_peer_info());
	
	connectedDevices.insert(mac);
	Serial.printf(MESSAGE_FORMAT, from, "connect");
	m_Mesh.sendSingle(from, "connect");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
	String message((char*)incomingData, len);
	Serial.println(message);

	if (message.equals("pair"))
	{
		
	}
	else
	{
		Serial.printf(MESSAGE_FORMAT, from, msg.c_str());
	}
}

// void receivedCallback(uint32_t from, String &msg)
// {
// 	if (msg.equals("pair"))
// 	{
// 		connectedDevices.insert(from);
// 		Serial.printf(MESSAGE_FORMAT, from, "connect");
// 		m_Mesh.sendSingle(from, "connect");
// 	}
// 	else
// 	{
// 		Serial.printf(MESSAGE_FORMAT, from, msg.c_str());
// 	}
	
// }

void setup()
{
	Serial.begin(115200);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
	m_SerialBuffer[SERIAL_BUFFER_LENGTH] = '\0';

	WiFi.mode(WIFI_STA);

	esp_wifi_set_mac(WIFI_IF_STA, mac);

	// Init ESP-NOW
	if (esp_now_init() != ESP_OK) {
		Serial.println("Error initializing ESP-NOW");
		return;
	}

	esp_now_register_recv_cb(OnDataRecv);

	// esp_now_peer_info peer;
	// memcpy(peer.peer_addr, mac, 6);
	// peer.channel = 0;
	// peer.encrypt = false;

	// if (esp_now_add_peer(&peer) != ESP_OK)
	// {
	// 	Serial.println("Failed to add peer");
	// 	return;
  	// }
	
	Serial.println("Ready");
}

void loop()
{
	// String message("TestMessage");
	// // receiveSerialData();

	// delay(5000);
	// esp_now_send(mac, (uint8_t*)message.c_str(), message.length());
}