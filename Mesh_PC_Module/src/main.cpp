#include <Arduino.h>
#include "painlessMesh.h"
#include <set>

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

#define START_CHAR '#'
#define END_CHAR '$'
#define DELIM_CHAR ':'
#define MESSAGE_FORMAT "#%u:%s$"

Scheduler m_Scheduler;
painlessMesh m_Mesh;

#define SERIAL_BUFFER_LENGTH 256
char m_SerialBuffer[SERIAL_BUFFER_LENGTH + 1];
int m_BufferWriteLocation = 0;
int m_bufferReadLocation = 0;

int m_MsgStart = -1; // Negative for invalid value
int m_MsgEnd = -1;	 // Negative for invalid value

std::set<uint32_t> connectedDevices;
#define LED_PIN 22

// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{
	Serial.printf(MESSAGE_FORMAT, from, msg.c_str());
}

void HandleMessage(String& message)
{
	if (message.equals("led_high"))
	{
		digitalWrite(LED_PIN, LOW);
	}
	else if (message.equals("led_low"))
	{
		digitalWrite(LED_PIN, HIGH);
	}
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
	//#2222541329:led_high$
	if (first.equals("command"))
	{
		if (second.equals("getConnected"))
		{
			for (const auto& val : connectedDevices)
			{
				Serial.printf(MESSAGE_FORMAT, val, "connect");
			}
		}
		else if (second.equals("test"))
		{
			Serial.printf(MESSAGE_FORMAT, 420, "69");
		}
	}
	else
	{
		uint32_t targetID = strtoul(first.c_str(), NULL, 0);
		Serial.printf("%u and %s\n", targetID, second);
		HandleMessage(second);
		m_Mesh.sendSingle(targetID, second);
	}
	
}

void receiveSerialData()
{
	while (Serial.available() > 0)
	{
		m_SerialBuffer[m_BufferWriteLocation] = Serial.read();
		m_BufferWriteLocation++;
		if (m_BufferWriteLocation == SERIAL_BUFFER_LENGTH) // Loop around
		{
			m_BufferWriteLocation = 0;
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
			m_MsgEnd = m_bufferReadLocation;
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

void newConnectionCallback(uint32_t nodeId)
{
	connectedDevices.insert(nodeId);
	Serial.printf(MESSAGE_FORMAT, nodeId, "connect");
	m_Mesh.sendSingle(nodeId, "connect");
}

void changedConnectionCallback()
{

}

void droppedConnectionCallback(uint32_t nodeId)
{
	connectedDevices.erase(nodeId);
}

void nodeTimeAdjustedCallback(int32_t offset)
{
	// Serial.printf("Adjusted time %u. Offset = %d\n", m_Mesh.getNodeTime(), offset);
}

void setup()
{
	Serial.begin(115200);

	// mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE );
	//m_Mesh.setDebugMsgTypes(ERROR | STARTUP);
	m_Mesh.setDebugMsgTypes( 0 ); //No debug logging: serial is used for pc communication

	m_Mesh.init(MESH_PREFIX, MESH_PASSWORD, &m_Scheduler, MESH_PORT);
	m_Mesh.onReceive(&receivedCallback);
	m_Mesh.onNewConnection(&newConnectionCallback);
	m_Mesh.onChangedConnections(&changedConnectionCallback);
	m_Mesh.onDroppedConnection(&droppedConnectionCallback);
	m_Mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

	pinMode(LED_PIN, OUTPUT);
	delay(5);
	digitalWrite(LED_PIN, LOW);

	m_SerialBuffer[SERIAL_BUFFER_LENGTH] = '\0';

	Serial.println("Ready");
}

void loop()
{
	// it will run the user scheduler as well
	m_Mesh.update();
	receiveSerialData();
}