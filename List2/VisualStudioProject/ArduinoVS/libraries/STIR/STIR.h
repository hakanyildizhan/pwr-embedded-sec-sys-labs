#pragma once
#ifndef STIR_h
#define STIR_h

#ifndef Arduino_h
#include <Arduino.h>
#endif

#ifndef ARDUINO
#define ARDUINO 10800
#endif

#ifdef VISUAL_STUDIO
#include <cores/arduino/SerialSimulator.h>
#endif // VISUAL_STUDIO

#define PIN_NOT_SET 0
#define ENABLE_LED_FEEDBACK false
//#define IR_SEND_PIN 3

const long SERIAL_BAUDRATE = 115200;

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

struct STIRConfig 
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
		/*#undef IR_SEND_PIN
		#define IR_SEND_PIN irSendPin*/
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
		/*#undef IR_SEND_PIN
		#define IR_SEND_PIN irSendPin*/
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
		irSendPin = 3;
		incomingMessageProcessing = ProcessIncoming::DONOTHING;
		onMessageReceived = onMessage;
	}
};

struct Buffer
{
public:
	Buffer() = default;
	Buffer(uint8_t* buffer, uint8_t bufferSize);
	uint8_t* Messagebuffer;
	uint8_t BufferSize;
};

class STIR
{
public:
	STIR() = default;
	STIR(int irReceivePin, int irSendPin);
	STIR(STIRConfig config, uint8_t role);
	void sendBinary(bool binaryMessage[]);
	void sendString(String message);
	void communicationLoop();
	void beginListen();
	bool listenForCipherKey();
	bool sendCipherKey(bool* buffer);
	void endListen();
	State getState();
	Buffer Buffer;
	uint8_t* STIR::convertBinToCommandSequence(bool bin[], size_t size);
	char bufferMessageFromPC[100];
	uint8_t bufferCipherKey[32];
	bool cipherKey[228];
	uint8_t bufferMessageFromPCSize;
	void freeBufferMessageFromPC();
	void beginListenSerial();
	void beginListenIR();
	void beginTransmitIR();
	void endListenIR();
	void endListenSerial();
	uint8_t role;
	bool messageReceived;
private:
	State state;
	int pinWaiting;
	int pinReceiving;
	int pinSending;
	EventHandler onReceive;
	bool serialStarted;
	bool irListenStarted;
	bool irTransmitStarted;
	bool initiated;
	int pinIRReceive;
	int pinIRSend;
	uint8_t* buffer;
	uint8_t bufferSize;
	void sendFinishSequence();
	void sendAcknowledgedSequence();
	String convertHexToBin(const char s[]);
	String convertBinToHex(const String& s);
	void buildCipherKey();
	uint8_t intpow(uint8_t x);
};
#endif