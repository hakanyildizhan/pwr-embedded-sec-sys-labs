#pragma once
#ifndef A51Cipher_h
#define A51Cipher_h

#ifndef Arduino_h
#include <Arduino.h>
#endif

#ifdef VISUAL_STUDIO
#include <cores/arduino/SerialSimulator.h>
#include <stdexcept>
#endif // VISUAL_STUDIO

class A51Cipher
{
public:
	A51Cipher();
	A51Cipher(bool keyStreamInput[228]);
	A51Cipher(uint8_t keyStreamInput[28]);
	void createCipherKey();
	bool* encryptMessage(char* message);
	uint8_t* encryptMessage2(char* message);
	void decryptMessage(bool encryptedMessage[], uint8_t encryptedMessageSize, char message[]);
	void decryptMessage(uint8_t encryptedMessage[], uint8_t encryptedMessageSize, char message[]);
	void decryptMessage(uint8_t* encryptedMessage, uint8_t encryptedMessageSize);
	bool cipherKey[228];
	uint8_t cipherKeyInt[28];
	char buffer[100];
	void freeBuffer();
private:
	void initialize();
	void generateKeyAndFrame();
	void convertStringToBinary(char* message, bool binaryMessage[]);
	bool initialized;
	bool keyAndFrameInitialized;
	bool cipherKeyGenerated;
	bool isMaster;
	const static uint8_t powersOf2[8];
};

#endif