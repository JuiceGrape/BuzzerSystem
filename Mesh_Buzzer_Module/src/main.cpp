#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

#define START_CHAR '#'
#define END_CHAR '$'
#define DELIM_CHAR ':'
#define MESSAGE_FORMAT "#%s:%s$"

#define BUTTON_PIN 19
#define LED_PIN 5

bool m_InitReceived = false;
uint32_t m_MainNode = 0;

bool m_ButtonPressed = false;

bool m_LedOn = false;

uint8_t targetMac[6] {0xDE, 0xAD, 0xBE, 0xEF, 0x12, 0x53};
esp_now_peer_info mainBoard;

void SendMessage(String message)
{
	esp_now_send(targetMac, (uint8_t*)message.c_str(), message.length());
}

void SetLed(bool LedOn)
{
	if (LedOn)
	{
		digitalWrite(LED_PIN, HIGH);
		digitalWrite(BUILTIN_LED, LOW);
		m_LedOn = true;
	}
	else
	{
		digitalWrite(LED_PIN, LOW);
		digitalWrite(BUILTIN_LED, HIGH);
		m_LedOn = false;
	}
}

void HandleButton(int pin)
{
	if (!m_InitReceived)
		return;

	if (!m_ButtonPressed)
	{
		if (digitalRead(pin) > 0)
		{
			m_ButtonPressed = true;
			//m_Mesh.sendSingle(m_MainNode, "press");
		}
	}
	else
	{
		if (digitalRead(pin) <= 0)
		{
			m_ButtonPressed = false;
		}
	}
}

void HandleMessage(String& message)
{
	if (message.equals("led_high"))
	{
		SetLed(true);
	}
	else if (message.equals("led_low"))
	{
		SetLed(false);
	}
}

void receivedCallback(uint32_t from, String &msg)
{
	if (!m_InitReceived) //TODO: Broadcast connect request message instead of depending on connection request from server. Maybe state behaviour?
	{
		if (msg.equals("connect"))
		{
			m_MainNode = from;
			m_InitReceived = true;
			Serial.printf("Initialized with %u", m_MainNode);
			SetLed(false);
		}	
	}
	else
	{
		if (from == m_MainNode)
		{
			HandleMessage(msg);
		}
	}
	Serial.println(msg);
}

void setup()
{
	Serial.begin(115200);

	pinMode(BUTTON_PIN, INPUT_PULLDOWN);

	pinMode(LED_PIN, OUTPUT);
	pinMode(BUILTIN_LED, OUTPUT);

	WiFi.mode(WIFI_STA);

	// Init ESP-NOW
	if (esp_now_init() != ESP_OK) {
		Serial.println("Error initializing ESP-NOW");
		return;
	}

	memcpy(mainBoard.peer_addr, targetMac, 6);
	mainBoard.channel = 0;
	mainBoard.encrypt = false;

	if (esp_now_add_peer(&mainBoard) != ESP_OK)
	{
		Serial.println("Failed to add peer");
		return;
  	}
}

String testMessage("I like boobies");

void loop()
{
	HandleButton(BUTTON_PIN);
	sleep(2);
	SetLed(!m_LedOn);
}