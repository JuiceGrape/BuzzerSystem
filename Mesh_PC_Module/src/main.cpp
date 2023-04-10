#include <Arduino.h>
#include <map>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

#define START_CHAR '#'
#define END_CHAR '$'
#define DELIM_CHAR ':'
#define MESSAGE_FORMAT "#%llu:%s$"
#define SERIAL_BUFFER_LENGTH 256

char m_SerialBuffer[SERIAL_BUFFER_LENGTH + 1];
int m_BufferWriteLocation = 0;
int m_bufferReadLocation = 0;

int m_MsgStart = -1; // Negative for invalid value
int m_MsgEnd = -1;	 // Negative for invalid value

uint8_t mac[6] {0xDE, 0xAD, 0xBE, 0xEF, 0x12, 0x53};
std::map<uint64_t, esp_now_peer_info*> connectedDevices;


bool m_LedOn = false;

#define LED_BUILTIN 2

void SendMessage(esp_now_peer_info* peer, String message)
{
	esp_now_send(peer->peer_addr, (uint8_t*)message.c_str(), message.length());
}

void SendMessage(uint64_t UID, String message)
{
	SendMessage(connectedDevices[UID], message);
}

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

uint64_t macToInt(const uint8_t* mac)
{
	uint64_t retVal = 0;
	memcpy(&retVal, mac, 6);
	return retVal;
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
			for (const auto& val : connectedDevices)
			{
				Serial.printf(MESSAGE_FORMAT, val.first, "connect");
			}
		}
	}
	else
	{
		uint64_t targetID = strtoull(first.c_str(), NULL, 0);
		SendMessage(targetID, second);
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

void AddMac(const uint8_t* mac)
{
	auto UID = macToInt(mac);
	if (connectedDevices.find(UID) != connectedDevices.end())
	{
		if (esp_now_del_peer(mac) != ESP_OK)
		{
			Serial.println("Couldn't delete mac address");
		}

		delete connectedDevices[UID];
		connectedDevices.erase(UID);
	}

	auto peerInfo = new esp_now_peer_info();
	memcpy(peerInfo->peer_addr, mac, 6);
	peerInfo->channel = 0;
	peerInfo->encrypt = false;

	if (esp_now_add_peer(peerInfo) != ESP_OK)
	{
		Serial.println("Failed to add peer");
		delete peerInfo;
		return;
  	}

	SendMessage(peerInfo, "pair_complete");
	connectedDevices.insert({UID, peerInfo});
}

void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) 
{
	String message((char*)incomingData, len);

	if (message.equals("pair"))
	{
		AddMac(mac);
	}
	else
	{
		Serial.printf(MESSAGE_FORMAT, macToInt(mac), message);
	}
}

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

	Serial.println("Ready");
}

void loop()
{
	receiveSerialData();
}