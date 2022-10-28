#include "Arduino.h"
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

void A51Cipher::initialize()
{
	initialized = false;
	isMaster = true;
	keyAndFrameInitialized = false;
	cipherKeyGenerated = false;
	memset(key, false, sizeof(key));
	memset(frame, false, sizeof(frame));
	memset(r1, false, sizeof(r1));
	memset(r2, false, sizeof(r2));
	memset(r3, false, sizeof(r3));
	memset(cipherKey, false, sizeof(cipherKey));
	initialized = true;
}

void A51Cipher::initialize(bool keyInput[64], bool additionalFrame[22])
{
	initialized = false;
	keyAndFrameInitialized = false;
	cipherKeyGenerated = false;
	isMaster = false;
	memcpy(key, keyInput, 64);
	memcpy(frame, additionalFrame, 22);
	keyAndFrameInitialized = true;
	memset(r1, false, sizeof(r1));
	memset(r2, false, sizeof(r2));
	memset(r3, false, sizeof(r3));
	memset(cipherKey, false, sizeof(cipherKey));
	initialized = true;
}

void A51Cipher::createCipherKey(bool(&keyStream)[228])
{
	if (!initialized)
	{
		initialize();
	}
	if (!keyAndFrameInitialized)
	{
		generateKeyAndFrame();
	}
	phaseOne();
	memset(keyStream, false, sizeof(keyStream));
	phaseTwo(keyStream);
	cipherKeyGenerated = true;
}

void A51Cipher::generateKeyAndFrame()
{
	// if analog input pin 0 is unconnected, random analog
	// noise will cause the call to randomSeed() to generate
	// different seed numbers each time the sketch runs.
	// randomSeed() will then shuffle the random function.
	randomSeed(analogRead(0));

	for (int i = 0; i < sizeof(key); i++)
	{
		key[i] = random(2);
	}

	for (int i = 0; i < sizeof(frame); i++)
	{
		frame[i] = random(2);
	}
}

void A51Cipher::phaseOne()
{
	for (int i = 0; i < sizeof(key); i++)
	{
		bool xorResult = r1[13] ^ r1[16] ^ r1[17] ^ r1[18] ^ key[i];
		for (int j = sizeof(r1) - 1; j > 0; j--)
		{
			r1[j] = r1[j - 1];
		}
		r1[0] = xorResult;

		xorResult = r2[20] ^ r2[21] ^ key[i];
		for (int j = sizeof(r2) - 1; j > 0; j--)
		{
			r2[j] = r2[j - 1];
		}
		r2[0] = xorResult;

		xorResult = r3[7] ^ r3[20] ^ r3[21] ^ r3[22] ^ key[i];
		for (int j = sizeof(r3) - 1; j > 0; j--)
		{
			r3[j] = r3[j - 1];
		}
		r3[0] = xorResult;
	}

	for (int i = 0; i < sizeof(frame); i++)
	{
		bool xorResult = r1[13] ^ r1[16] ^ r1[17] ^ r1[18] ^ frame[i];
		for (int j = sizeof(r1) - 1; j > 0; j--)
		{
			r1[j] = r1[j - 1];
		}
		r1[0] = xorResult;

		xorResult = r2[20] ^ r2[21] ^ frame[i];
		for (int j = sizeof(r2) - 1; j > 0; j--)
		{
			r2[j] = r2[j - 1];
		}
		r2[0] = xorResult;

		xorResult = r3[7] ^ r3[20] ^ r3[21] ^ r3[22] ^ frame[i];
		for (int j = sizeof(r3) - 1; j > 0; j--)
		{
			r3[j] = r3[j - 1];
		}
		r3[0] = xorResult;
	}
}

void A51Cipher::phaseTwo(bool(&keyStream)[228])
{
	for (int i = 0; i < 100; i++)
	{
		bool majorityBit = ((byte)r1[r1SpecialBit] + (byte)r2[r2r3SpecialBit] + (byte)r3[r2r3SpecialBit]) >= 2 ? true : false;

		if (majorityBit == r1[r1SpecialBit])
		{
			int xorResult = r1[13] ^ r1[16] ^ r1[17] ^ r1[18];
			for (int j = sizeof(r1) - 1; j > 0; j--)
			{
				r1[j] = r1[j - 1];
			}
			r1[0] = xorResult;
		}

		if (majorityBit == r2[r2r3SpecialBit])
		{
			int xorResult = r2[20] ^ r2[21];
			for (int j = sizeof(r2) - 1; j > 0; j--)
			{
				r2[j] = r2[j - 1];
			}
			r2[0] = xorResult;
		}

		if (majorityBit == r3[r2r3SpecialBit])
		{
			int xorResult = r3[7] ^ r3[20] ^ r3[21] ^ r3[22];
			for (int j = sizeof(r3) - 1; j > 0; j--)
			{
				r3[j] = r3[j - 1];
			}
			r3[0] = xorResult;
		}
	}

	for (int i = 0; i < 228; i++)
	{
		bool majorityBit = ((byte)r1[r1SpecialBit] + (byte)r2[r2r3SpecialBit] + (byte)r3[r2r3SpecialBit]) >= 2 ? true : false;

		if (majorityBit == r1[r1SpecialBit])
		{
			int xorResult = r1[13] ^ r1[16] ^ r1[17] ^ r1[18];
			for (int j = sizeof(r1) - 1; j > 0; j--)
			{
				r1[j] = r1[j - 1];
			}
			r1[0] = xorResult;
		}

		if (majorityBit == r2[r2r3SpecialBit])
		{
			int xorResult = r2[20] ^ r2[21];
			for (int j = sizeof(r2) - 1; j > 0; j--)
			{
				r2[j] = r2[j - 1];
			}
			r2[0] = xorResult;
		}

		if (majorityBit == r3[r2r3SpecialBit])
		{
			int xorResult = r3[7] ^ r3[20] ^ r3[21] ^ r3[22];
			for (int j = sizeof(r3) - 1; j > 0; j--)
			{
				r3[j] = r3[j - 1];
			}
			r3[0] = xorResult;
		}

		// XOR last bits, add to keystream
		bool tobeAddedToKeystream = r1[sizeof(r1) - 1] ^ r2[sizeof(r2) - 1] ^ r3[sizeof(r3) - 1];
		keyStream[i] = tobeAddedToKeystream;
	}

	memcpy(cipherKey, keyStream, 228);
}

void A51Cipher::encryptMessage(char message[], int messageSize, bool encryptedMessage[])
{
	if (!cipherKeyGenerated)
	{
		return;
		//throw std::runtime_error("Cipher key not generated");
	}

	bool* binaryMessage = (bool*)malloc(sizeof(bool) * (messageSize * 8));
	memset(binaryMessage, false, sizeof(bool) * (messageSize * 8));

	for (int i = 0; i < messageSize; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			binaryMessage[i * 8 + 7 - j] = bitRead(message[i], j) == 1;
		}
	}

	for (int i = 0; i < messageSize * 8; i++)
	{
		encryptedMessage[i] = binaryMessage[i] ^ cipherKey[i % 228];
	}

	free(binaryMessage);
}

void A51Cipher::decryptMessage(bool encryptedMessage[], int encryptedMessageSize, char message[])
{
	if (!cipherKeyGenerated)
	{
		return;
		//throw std::runtime_error("Cipher key not generated");
	}

	for (int i = 0; i < encryptedMessageSize; i++)
	{
		encryptedMessage[i] = encryptedMessage[i] ^ cipherKey[i % 228];
	}

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

#endif