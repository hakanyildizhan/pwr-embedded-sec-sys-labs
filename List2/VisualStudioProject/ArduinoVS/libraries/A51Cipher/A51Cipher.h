#pragma once
#ifndef A51Cipher_h
#define A51Cipher_h

#ifndef Arduino_h
#include <Arduino.h>
#endif

#ifdef VISUAL_STUDIO
#include <cores/arduino/SerialSimulator.h>
#endif // VISUAL_STUDIO

class A51Cipher
{
public:
	A51Cipher();
	A51Cipher(bool keyStreamInput[228]);
	void createCipherKey();
	bool* encryptMessage(char* message);
	void decryptMessage(bool encryptedMessage[], uint8_t encryptedMessageSize, char message[]);
	bool cipherKey[228];
private:
	void initialize();
	void initialize(bool keyInput[64], bool additionalFrame[22]);
	void generateKeyAndFrame();
	void phaseOne();
	void phaseTwo();
	void convertStringToBinary(char* message, bool binaryMessage[]);
	bool initialized;
	bool keyAndFrameInitialized;
	bool cipherKeyGenerated;
	bool isMaster;
	bool key[64];
	bool frame[22];
	bool r1[19];
	bool r2[22];
	bool r3[23];
	static const uint8_t r1SpecialBit = 8;
	static const uint8_t r2r3SpecialBit = 10;
	bool messageReceived[114];
	
};

#endif