#pragma once
#ifndef A51Cipher_h
#define A51Cipher_h

class A51Cipher
{
public:
	A51Cipher();
	void createCipherKey(bool(&keyStream)[228]);
	void encryptMessage(char message[], int messageSize, bool encryptedMessage[]);
	void decryptMessage(bool encryptedMessage[], int encryptedMessageSize, char message[]);
private:
	void initialize();
	void initialize(bool keyInput[64], bool additionalFrame[22]);
	void generateKeyAndFrame();
	void phaseOne();
	void phaseTwo(bool(&keyStream)[228]);
	bool initialized;
	bool keyAndFrameInitialized;
	bool cipherKeyGenerated;
	bool isMaster;
	bool key[64];
	bool frame[22];
	bool r1[19];
	bool r2[22];
	bool r3[23];
	static const int r1SpecialBit = 8;
	static const int r2r3SpecialBit = 10;
	bool messageReceived[114];
	bool cipherKey[228];
};

#endif