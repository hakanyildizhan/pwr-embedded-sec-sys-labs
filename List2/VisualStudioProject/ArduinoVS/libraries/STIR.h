#pragma once
#ifndef STIR_h
#define STIR_h
#endif

#ifndef ARDUINO
#define ARDUINO 10800
#endif

#ifndef IRremote_h
#include <IRremote.h>
#endif

//#ifndef __AVR__
//#define __AVR__
//#endif
//
//#ifndef __AVR_ATmega328P__
//#define __AVR_ATmega328P__
//#endif



#include <cores/arduino/SerialSimulator.h>
#ifndef Arduino_h
#include <Arduino.h>
#endif
//#define RAW_BUFFER_LENGTH 100
//#define IR_SEND_PIN 3
//#define SEND_PWM_BY_TIMER 4

//#ifndef _IR_SEND_HPP
//#include <IRSend.hpp>
//#endif

#define PIN_NOT_SET 0
const long SERIAL_BAUDRATE = 9600;

typedef void* (*EventHandler)(String);

inline void* doNothing(String message) { return NULL; }

inline void* writeToSerial(String message)
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
		#undef IR_SEND_PIN
		#define IR_SEND_PIN irSendPin
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
		#undef IR_SEND_PIN
		#define IR_SEND_PIN irSendPin
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
	void sendBinary(bool binaryMessage[]);
	void sendString(String message);
	void communicationLoop();
	//String receive();
	void beginListen();
	void endListen();
	State getState();
private:
	State state;
	int pinWaiting;
	int pinReceiving;
	int pinSending;
	EventHandler onReceive;
	bool serialStarted;
	bool irStarted;
	bool initiated;
	int pinIRReceive;
	int pinIRSend;
	void beginListenSerial();
	void beginListenIR();
	char* convertBinToHexString(bool bin[]);
	String convertHexToBin(const char s[]);
	String convertBinToHex(const String& s);
	decode_results results;
};