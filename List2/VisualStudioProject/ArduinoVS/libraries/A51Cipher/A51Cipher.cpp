#ifndef A51Cipher_h  
#include "A51Cipher.h"

A51Cipher::A51Cipher()
{
	initialize();
}

A51Cipher::A51Cipher(bool keyStreamInput[228])
{
	initialized = true;
	isMaster = true;
	keyAndFrameInitialized = true;
	cipherKeyGenerated = true;
	memcpy(cipherKey, keyStreamInput, 228);
	initialized = true;
}

A51Cipher::A51Cipher(uint8_t keyStreamInput[28])
{
	initialized = true;
	isMaster = true;
	keyAndFrameInitialized = true;
	cipherKeyGenerated = true;
	memcpy(cipherKeyInt, keyStreamInput, 28);
	initialized = true;
}

void A51Cipher::initialize()
{
	initialized = false;
	isMaster = true;
	keyAndFrameInitialized = false;
	cipherKeyGenerated = false;
	initialized = true;
}

void A51Cipher::createCipherKey()
{
	if (!initialized)
	{
		initialize();
	}
	if (!keyAndFrameInitialized)
	{
		generateKeyAndFrame();
	}

	cipherKeyGenerated = true;
}

void A51Cipher::generateKeyAndFrame()
{
	// if analog input pin 0 is unconnected, random analog
	// noise will cause the call to randomSeed() to generate
	// different seed numbers each time the sketch runs.
	// randomSeed() will then shuffle the random function.
	randomSeed(analogRead(0));

	bool key[64];
	bool frame[22];
	bool r1[19];
	bool r2[22];
	bool r3[23];
	memset(key, false, 64);
	memset(frame, false, 22);
	memset(r1, false, sizeof(r1));
	memset(r2, false, sizeof(r2));
	memset(r3, false, sizeof(r3));
	memset(cipherKey, false, sizeof(cipherKey));
	memset(cipherKeyInt, 0, sizeof(cipherKeyInt));

	for (int i = 0; i < sizeof(key); i++)
	{
		key[i] = random(2);
	}

	for (uint8_t i = 0; i < sizeof(frame); i++)
	{
		frame[i] = random(2);
	}

	for (uint8_t i = 0; i < sizeof(key); i++)
	{
		bool xorResult = r1[13] ^ r1[16] ^ r1[17] ^ r1[18] ^ key[i];
		for (uint8_t j = sizeof(r1) - 1; j > 0; j--)
		{
			r1[j] = r1[j - 1];
		}
		r1[0] = xorResult;

		xorResult = r2[20] ^ r2[21] ^ key[i];
		for (uint8_t j = sizeof(r2) - 1; j > 0; j--)
		{
			r2[j] = r2[j - 1];
		}
		r2[0] = xorResult;

		xorResult = r3[7] ^ r3[20] ^ r3[21] ^ r3[22] ^ key[i];
		for (uint8_t j = sizeof(r3) - 1; j > 0; j--)
		{
			r3[j] = r3[j - 1];
		}
		r3[0] = xorResult;
	}

	for (uint8_t i = 0; i < sizeof(frame); i++)
	{
		bool xorResult = r1[13] ^ r1[16] ^ r1[17] ^ r1[18] ^ frame[i];
		for (uint8_t j = sizeof(r1) - 1; j > 0; j--)
		{
			r1[j] = r1[j - 1];
		}
		r1[0] = xorResult;

		xorResult = r2[20] ^ r2[21] ^ frame[i];
		for (uint8_t j = sizeof(r2) - 1; j > 0; j--)
		{
			r2[j] = r2[j - 1];
		}
		r2[0] = xorResult;

		xorResult = r3[7] ^ r3[20] ^ r3[21] ^ r3[22] ^ frame[i];
		for (uint8_t j = sizeof(r3) - 1; j > 0; j--)
		{
			r3[j] = r3[j - 1];
		}
		r3[0] = xorResult;
	}

	for (uint8_t i = 0; i < 100; i++)
	{
		bool majorityBit = ((byte)r1[8] + (byte)r2[10] + (byte)r3[10]) >= 2 ? true : false;

		if (majorityBit == r1[8])
		{
			uint8_t xorResult = r1[13] ^ r1[16] ^ r1[17] ^ r1[18];
			for (uint8_t j = sizeof(r1) - 1; j > 0; j--)
			{
				r1[j] = r1[j - 1];
			}
			r1[0] = xorResult;
		}

		if (majorityBit == r2[10])
		{
			uint8_t xorResult = r2[20] ^ r2[21];
			for (uint8_t j = sizeof(r2) - 1; j > 0; j--)
			{
				r2[j] = r2[j - 1];
			}
			r2[0] = xorResult;
		}

		if (majorityBit == r3[10])
		{
			uint8_t xorResult = r3[7] ^ r3[20] ^ r3[21] ^ r3[22];
			for (uint8_t j = sizeof(r3) - 1; j > 0; j--)
			{
				r3[j] = r3[j - 1];
			}
			r3[0] = xorResult;
		}
	}

	for (uint8_t i = 0; i < 228; i++)
	{
		bool majorityBit = ((byte)r1[8] + (byte)r2[10] + (byte)r3[10]) >= 2 ? true : false;

		if (majorityBit == r1[8])
		{
			uint8_t xorResult = r1[13] ^ r1[16] ^ r1[17] ^ r1[18];
			for (uint8_t j = sizeof(r1) - 1; j > 0; j--)
			{
				r1[j] = r1[j - 1];
			}
			r1[0] = xorResult;
		}

		if (majorityBit == r2[10])
		{
			uint8_t xorResult = r2[20] ^ r2[21];
			for (uint8_t j = sizeof(r2) - 1; j > 0; j--)
			{
				r2[j] = r2[j - 1];
			}
			r2[0] = xorResult;
		}

		if (majorityBit == r3[10])
		{
			uint8_t xorResult = r3[7] ^ r3[20] ^ r3[21] ^ r3[22];
			for (uint8_t j = sizeof(r3) - 1; j > 0; j--)
			{
				r3[j] = r3[j - 1];
			}
			r3[0] = xorResult;
		}

		// XOR last bits, add to keystream
		bool tobeAddedToKeystream = r1[sizeof(r1) - 1] ^ r2[sizeof(r2) - 1] ^ r3[sizeof(r3) - 1];
		//Serial.print(tobeAddedToKeystream ? '1' : '0');
		cipherKey[i] = tobeAddedToKeystream;
		cipherKeyInt[i/8] += cipherKey[i] == true ? powersOf2[7 - (i%8)] : 0;
	}

	keyAndFrameInitialized = true;
}
const uint8_t A51Cipher::powersOf2[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
bool* A51Cipher::encryptMessage(char* message)
{
	if (!cipherKeyGenerated)
	{
#ifdef VISUAL_STUDIO
		throw std::runtime_error("Cipher key not generated");
#else
		bool res[1] = { false };
		return res;
#endif // VISUAL_STUDIO
	}

	size_t len = strlen(message);
	bool* binMessage = (bool*)malloc(sizeof(bool) * len*8);
	convertStringToBinary(message, binMessage);

	for (size_t i = 0; i < len * 8; i++) {
		binMessage[i] = binMessage[i] ^ cipherKey[i % 228];
	}
	return binMessage;
}

uint8_t* A51Cipher::encryptMessage2(char* message)
{
	if (!cipherKeyGenerated)
	{
#ifdef VISUAL_STUDIO
		throw std::runtime_error("Cipher key not generated");
#else
		uint8_t res[1] = { 0 };
		return res;
#endif // VISUAL_STUDIO
	}

	size_t len = strlen(message)-1;
	//Serial.println(F("Sending message:"));
	/*Serial.println(len);*/
	uint8_t* encryptedMessage = (uint8_t*)malloc(sizeof(uint8_t) * len);
	for (size_t i = 0; i < len; i++)
	{
		/*Serial.print(F("Message bit: "));
		Serial.print(message[i]);
		Serial.print(" ");
		Serial.print(F("cipher key bit: "));
		Serial.print(cipherKeyInt[i % 28]);*/
		encryptedMessage[i] = (uint8_t)message[i] ^ cipherKeyInt[i % 28];
		/*Serial.print(" ");
		Serial.print(F("resulting bit: "));
		Serial.print(encryptedMessage[i]);
		Serial.println();*/
	}
	return encryptedMessage;
}

void A51Cipher::convertStringToBinary(char* message, bool binaryMessage[])
{
	size_t len = strlen(message);
	for (size_t i = 0; i < len; ++i) {
		for (int j = 7; j >= 0; --j) {
			binaryMessage[i * 8 + (7 - j)] = (message[i] & (1 << j) ? 1 : 0);
		}
	}
}

void A51Cipher::decryptMessage(bool encryptedMessage[], uint8_t encryptedMessageSize, char message[])
{
	if (!cipherKeyGenerated)
	{
#ifdef VISUAL_STUDIO
		throw std::runtime_error("Cipher key not generated");
#else
		return;
#endif // VISUAL_STUDIO
	}

	//Serial.print(F("Decrypted binary: "));
	for (int i = 0; i < encryptedMessageSize; i++)
	{
		encryptedMessage[i] = encryptedMessage[i] ^ cipherKey[i % 228];
		//Serial.print(encryptedMessage[i]);
	}
	//Serial.println(); 

	for (int i = 0; i < encryptedMessageSize; i += 8)
	{
		char letter = 0;
		for (int j = 0; j < 8; j++)
		{
			if (encryptedMessage[i + j])
			{
				letter |= 1 << (7 - j);
			}
		}

		message[i / 8] = letter;
	}
	message[encryptedMessageSize / 8] = '\0';
}

void A51Cipher::decryptMessage(uint8_t encryptedMessage[], uint8_t encryptedMessageSize, char message[])
{
	if (!cipherKeyGenerated)
	{
#ifdef VISUAL_STUDIO
		throw std::runtime_error("Cipher key not generated");
#else
		return;
#endif // VISUAL_STUDIO
	}
	/*Serial.print(F("Decrypting message with size: "));
	Serial.println(encryptedMessageSize);
	Serial.print(F("Decrypted message: "));*/
	for (int i = 0; i < encryptedMessageSize; i++)
	{
		/*Serial.print(encryptedMessage[i]);
		Serial.print(" ");
		Serial.print(cipherKeyInt[i % 28]);*/
		message[i] = (char)(encryptedMessage[i] ^ cipherKeyInt[i % 28]);
		//Serial.println();
	}
	Serial.println();
	message[encryptedMessageSize] = '\0'; 
}

void A51Cipher::decryptMessage(uint8_t* encryptedMessage, uint8_t encryptedMessageSize)
{
	//char* message = (char*)malloc(sizeof(char) * (encryptedMessageSize + 1));
	memset(buffer, '\0', sizeof(buffer));
	/*Serial.print(F("Decrypting message with size: "));
	Serial.println(encryptedMessageSize);*/
	for (int i = 0; i < encryptedMessageSize; i++)
	{
		/*Serial.print(F("Encrypted message bit: "));
		Serial.print(encryptedMessage[i]);
		Serial.print(F(" "));
		Serial.print(F("cipher key bit: "));
		Serial.print(cipherKeyInt[i % 28]);*/
		buffer[i] = (char)(encryptedMessage[i] ^ cipherKeyInt[i % 28]);
		/*Serial.print(F(" "));
		Serial.print(F("resulting decrypted bit: "));
		Serial.print(buffer[i]);
		Serial.println();*/
	}
	Serial.println();
	buffer[encryptedMessageSize] = '\0';
	Serial.println(buffer);
	//return message;
}

void A51Cipher::freeBuffer()
{
	memset(buffer, '\0', sizeof(buffer));
}

#endif