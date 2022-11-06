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
	void createCipherKey();
	bool* encryptMessage(char* message);
	void decryptMessage(bool encryptedMessage[], uint8_t encryptedMessageSize, char message[]);
	bool cipherKey[228];
private:
	void initialize();
	void generateKeyAndFrame();
	void convertStringToBinary(char* message, bool binaryMessage[]);
	bool initialized;
	bool keyAndFrameInitialized;
	bool cipherKeyGenerated;
	bool isMaster;
};

#endif