#include <Arduino.h>
#include "painlessMesh.h"

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

#define START_CHAR '#'
#define END_CHAR '$'
#define DELIM_CHAR ':'
#define MESSAGE_FORMAT "#%u:%s$"

#define BUTTON_PIN 19
#define LED_PIN 5

Scheduler m_Scheduler;
painlessMesh m_Mesh;

bool m_InitReceived = false;
uint32_t m_MainNode = 0;

bool m_ButtonPressed = false;

void LookForConnection();

Task taskTryConnect( TASK_SECOND * 1 , TASK_FOREVER, &LookForConnection );

bool m_LedOn = false;

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

void LookForConnection()
{
	SetLed(!m_LedOn);
	m_Mesh.sendBroadcast("pair");
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
			m_Mesh.sendSingle(m_MainNode, "press");
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
			taskTryConnect.disable();
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

void newConnectionCallback(uint32_t nodeId)
{

}

void changedConnectionCallback()
{

}

void droppedConnectionCallback(uint32_t nodeId)
{
	if (nodeId == m_MainNode) //Haven't seen it work yet, but this should fully reset the system
	{
		m_InitReceived = false;
		taskTryConnect.enable();
		
	}
}

void nodeTimeAdjustedCallback(int32_t offset)
{

}

void setup()
{
	Serial.begin(115200);

	// mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
	m_Mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages

	m_Mesh.init(MESH_PREFIX, MESH_PASSWORD, &m_Scheduler, MESH_PORT);
	m_Mesh.setRoot(false);
	m_Mesh.setContainsRoot(true);
	m_Mesh.onReceive(&receivedCallback);
	m_Mesh.onNewConnection(&newConnectionCallback);
	m_Mesh.onChangedConnections(&changedConnectionCallback);
	m_Mesh.onDroppedConnection(&droppedConnectionCallback);
	m_Mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

	pinMode(BUTTON_PIN, INPUT_PULLDOWN);

	pinMode(LED_PIN, OUTPUT);
	pinMode(BUILTIN_LED, OUTPUT);

	m_Scheduler.addTask(taskTryConnect);
	taskTryConnect.enable();
}

void loop()
{
	// it will run the user scheduler as well
	m_Mesh.update();
	HandleButton(BUTTON_PIN);
}