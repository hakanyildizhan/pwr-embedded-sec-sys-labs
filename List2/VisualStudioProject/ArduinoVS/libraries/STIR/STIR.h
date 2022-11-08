#pragma once
#ifndef STIR_h
#define STIR_h

#ifndef Arduino_h
#include <Arduino.h>
#endif

#ifndef String_class_h
#include <WString.h>
#endif

#ifndef ARDUINO
#define ARDUINO 10800
#endif

#ifdef VISUAL_STUDIO
#include <cores/arduino/SerialSimulator.h>
#endif // VISUAL_STUDIO

#define PIN_NOT_SET (uint8_t)	0
#define ENABLE_LED_FEEDBACK		false
#define SERIAL_BAUDRATE (long)	115200

#define FIN_SEQ1 (uint8_t)		0xFF
#define FIN_SEQ2 (uint8_t)		0x1
#define FIN_SEQ3 (uint8_t)		0xFF

#define ACK_SEQ1 (uint8_t)		0x1
#define ACK_SEQ2 (uint8_t)		0xFF
#define ACK_SEQ3 (uint8_t)		0x1

#define ROLE_MASTER (uint8_t)	0x1
#define ROLE_SLAVE (uint8_t)	0x2
#define UNKNOWN_ADDR (uint8_t)	0x0

typedef enum class State 
{
	WAITING		= 1		<< 0,
	LISTENING	= 1		<< 1, 
	RECEIVING	= 1		<< 2, 
	SENDING		= 1		<< 4,
	FORWARDING	= 1		<< 8
} State;

class STIR
{
public:
	STIR() = default;
	STIR(int irReceivePin, int irSendPin);
	STIR(uint8_t listeningLEDPin, uint8_t receivingLEDPin, uint8_t sendingLEDPin, uint8_t irReceivePin, uint8_t irSendPin, bool isMaster);
	void sendBinary(bool binaryMessage[]);
	void sendBinary(uint8_t encryptedMessage[]);
	void communicationLoop();
	void beginListen();
	bool listenForCipherKey();
	bool sendCipherKey(bool* buffer);
	void endListen();
	uint8_t* convertBinToCommandSequence(bool bin[], size_t size);
	bool* convertCommandSequenceToBin(uint8_t commandSequence[], size_t size);
	char bufferMessageFromPC[150];
	bool* bufferMessageFromIR;
	//uint8_t* bufferMessageFromIRInt;
	uint8_t bufferCipherKey[32];
	uint8_t cipherKeyInt[28];
	uint8_t bufferMessageFromPCSize;
	uint8_t bufferMessageFromIRSize;
	void freeBufferMessageFromPC();
	void freeBufferMessageFromIR();
	void beginListenSerial();
	void beginListenIR();
	void beginTransmitIR();
	void endListenIR();
	void endListenSerial();
	void endTransmitIR();
	void setState(State newState);
	uint8_t role;
	bool messageReceived;
	uint8_t buffer[100];
private:
	State state;
	uint8_t pinWaiting;
	uint8_t pinReceiving;
	uint8_t pinSending;
	uint8_t pinIRReceive;
	uint8_t pinIRSend;
	bool serialStarted;
	bool irListenStarted;
	bool irTransmitStarted;
	
	uint8_t bufferSize;
	void sendFinishSequence();
	void sendAcknowledgedSequence();
	bool waitForAcknowledgedSequence(bool isMessage);
	void buildCipherKey();
	uint8_t intpow(uint8_t x);
	const static uint16_t NEC_BUFFER[3];
	const static bool hexCharToBin[16][4];
	const static uint8_t powersOf2[8];
	
};

#endif