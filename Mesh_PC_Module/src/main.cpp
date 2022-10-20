#include <Arduino.h>
#include "painlessMesh.h"

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
char m_SerialBuffer[SERIAL_BUFFER_LENGTH];
int m_BufferWriteLocation = 0;
int m_bufferReadLocation = 0;

int m_MsgStart = -1; // Negative for invalid value
int m_MsgEnd = -1;	 // Negative for invalid value

// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{
	Serial.printf(MESSAGE_FORMAT, from, msg.c_str());
}

void HandleSerialMessage(String message)
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

	uint32_t targetID = first.toInt();
	m_Mesh.sendSingle(targetID, second);
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

		if (m_MsgStart != -1 && m_MsgEnd != -1 && m_MsgStart < m_MsgEnd) // Message found, processing
		{
			String receivedMessage;
			for (int i = m_MsgStart + 1; i != m_MsgEnd; i++)
			{
				Serial.print(m_SerialBuffer[i]);
				receivedMessage += m_SerialBuffer[i];
				if (i == SERIAL_BUFFER_LENGTH) // Loop around
				{
					i = 0;
				}
			}
			Serial.print('\n');
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
	Serial.printf(MESSAGE_FORMAT, nodeId, "connect");
}

void changedConnectionCallback()
{
	// Serial.printf("Changed connections\n");
}

void droppedConnectionCallback(uint32_t nodeId)
{
	// Serial.printf("Dropped connection %s\n", nodeId);
}

void nodeTimeAdjustedCallback(int32_t offset)
{
	// Serial.printf("Adjusted time %u. Offset = %d\n", m_Mesh.getNodeTime(), offset);
}

void setup()
{
	Serial.begin(115200);

	// mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
	m_Mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages

	m_Mesh.init(MESH_PREFIX, MESH_PASSWORD, &m_Scheduler, MESH_PORT);
	m_Mesh.onReceive(&receivedCallback);
	m_Mesh.onNewConnection(&newConnectionCallback);
	m_Mesh.onChangedConnections(&changedConnectionCallback);
	m_Mesh.onDroppedConnection(&droppedConnectionCallback);
	m_Mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void loop()
{
	// it will run the user scheduler as well
	m_Mesh.update();
	receiveSerialData();
}