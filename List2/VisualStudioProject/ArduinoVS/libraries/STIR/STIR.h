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

//struct Buffer
//{
//public:
//	Buffer() = default;
//	Buffer(uint8_t* buffer, uint8_t bufferSize);
//	uint8_t* Messagebuffer;
//	uint8_t BufferSize;
//};

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
	//Buffer Buffer;
	uint8_t* convertBinToCommandSequence(bool bin[], size_t size);
	bool* convertCommandSequenceToBin(uint8_t commandSequence[], size_t size);
	char bufferMessageFromPC[80];
	bool* bufferMessageFromIR;
	uint8_t bufferCipherKey[32];
	bool cipherKey[228];
	uint8_t bufferMessageFromPCSize;
	uint8_t bufferMessageFromIRSize;
	void freeBufferMessageFromPC();
	void freeBufferMessageFromIR();
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
	bool waitForAcknowledgedSequence();
	void buildCipherKey();
	uint8_t intpow(uint8_t x);
	uint16_t NEC_BUFFER[3] =
	{
		9000,  // Mark 9ms 
		2250,  // Space 2.25ms
		560    // Burst
	};

	bool hexCharToBin[16][4] =
	{
		{ false, false, false, false }, // 0 - 0000
		{ false, false, false, true  },	// 1 - 0001
		{ false, false, true,  false },	// 2 - 0010
		{ false, false, true,  true  },	// 3 - 0011
		{ false, true,  false, false },	// 4 - 0100
		{ false, true,  false, true  },	// 5 - 0101
		{ false, true,  true,  false },	// 6 - 0110
		{ false, true,  true,  true  },	// 7 - 0111
		{ true,  false, false, false },	// 8 - 1000
		{ true,  false, false, true  },	// 9 - 1001
		{ true,  false, true,  false },	// A - 1010
		{ true,  false, true,  true  },	// B - 1011
		{ true,  true,  false, false },	// C - 1100
		{ true,  true,  false, true  },	// D - 1101
		{ true,  true,  true,  false },	// E - 1110
		{ true,  true,  true,  true  },	// F - 1111
	};

	uint8_t powersOf2[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
};
#endif