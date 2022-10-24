#pragma once
#ifndef STIR_h
#define STIR_h
#include <cores/arduino/WString.h>
#include <cores/arduino/SerialSimulator.h>

#define PIN_NOT_SET 0
const long SERIAL_BAUDRATE = 9600;

typedef void* (*EventHandler)(String);

void* doNothing(String message) { return NULL; }

void* writeToSerial(String message)
{
	Serial.print(message);
	return NULL;
}

typedef enum class State 
{
	WAITING = 1		<< 0,
	LISTENING = 1	<< 1, 
	RECEIVING = 1	<< 2, 
	SENDING = 1		<< 4,
	FORWARDING = 1	<< 8
} State;

typedef enum class ProcessIncoming
{
	DONOTHING,
	WRITETOSERIAL
} ProcessIncoming;

typedef struct STIRConfig 
{
	int listeningLEDPin;
	int receivingLEDPin; 
	int sendingLEDPin;
	int irReceivePin;
	int irSendPin;
	ProcessIncoming incomingMessageProcessing;
	EventHandler onMessageReceived;

	STIRConfig(int listeningLedPin,
		int receivingLedPin,
		int sendingLedPin,
		int receiveIrPin,
		int sendIrPin)
	{
		listeningLEDPin = listeningLedPin;
		receivingLEDPin = receivingLedPin;
		sendingLEDPin = sendingLedPin;
		irReceivePin = receiveIrPin;
		irSendPin = sendIrPin;
		incomingMessageProcessing = ProcessIncoming::WRITETOSERIAL;
		onMessageReceived = doNothing;
	}

	STIRConfig(int listeningLedPin,
		int receivingLedPin,
		int sendingLedPin,
		int receiveIrPin,
		int sendIrPin,
		ProcessIncoming messageProcessing)
	{
		listeningLEDPin = listeningLedPin;
		receivingLEDPin = receivingLedPin;
		sendingLEDPin = sendingLedPin;
		irReceivePin = receiveIrPin;
		irSendPin = sendIrPin;
		incomingMessageProcessing = messageProcessing;
		onMessageReceived = doNothing;
	}

	STIRConfig(int listeningLedPin,
		int receivingLedPin,
		int sendingLedPin,
		int receiveIrPin,
		EventHandler onMessage)
	{
		listeningLEDPin = listeningLedPin;
		receivingLEDPin = receivingLedPin;
		sendingLEDPin = sendingLedPin;
		irReceivePin = receiveIrPin;
		incomingMessageProcessing = ProcessIncoming::DONOTHING;
		onMessageReceived = onMessage;
	}

	/*void doNothing(String p) {  }*/
};

//typedef struct _event {
//	EventHandler handler;
//}Event, * PEvent;

//typedef struct Message {
//	int length;
//	char content[];
//} Message;

class STIR
{
public:
	STIR(int irReceivePin, int irSendPin);
	STIR(STIRConfig config);
	void send(char binaryMessage[]);
	String receive();
	void beginListen();
	void endListen();
	State getState();
private:
	State state;
	void beginListenSerial();
	void beginListenIR();
	int pinWaiting;
	int pinReceiving;
	int pinSending;
	EventHandler onReceive;
	bool serialStarted;
	bool irStarted;
	bool initiated;
	int pinIRReceive;
	int pinIRSend;
};

#endif