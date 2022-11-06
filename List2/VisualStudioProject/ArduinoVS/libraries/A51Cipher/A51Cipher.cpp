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

	//Serial.print("Key: ");
	for (int i = 0; i < sizeof(key); i++)
	{
		key[i] = random(2);
		//Serial.print(key[i] == true ? '1' : '0');
	}
	//Serial.println();
	//Serial.print("Frame: ");
	for (int i = 0; i < sizeof(frame); i++)
	{
		frame[i] = random(2);
		//Serial.print(frame[i] == true ? '1' : '0');
	}
	//Serial.println();

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

	/*Serial.print("r1: ");
	for (int i = 0; i < sizeof(r1); i++)
	{
		Serial.print(r1[i] == true ? '1' : '0');
	}
	Serial.println();
	Serial.print("r2: ");
	for (int i = 0; i < sizeof(r2); i++)
	{
		Serial.print(r2[i] == true ? '1' : '0');
	}
	Serial.println();
	Serial.print("r3: ");
	for (int i = 0; i < sizeof(r3); i++)
	{
		Serial.print(r3[i] == true ? '1' : '0');
	}
	Serial.println();*/

	for (int i = 0; i < 100; i++)
	{
		bool majorityBit = ((byte)r1[8] + (byte)r2[10] + (byte)r3[10]) >= 2 ? true : false;

		if (majorityBit == r1[8])
		{
			int xorResult = r1[13] ^ r1[16] ^ r1[17] ^ r1[18];
			for (int j = sizeof(r1) - 1; j > 0; j--)
			{
				r1[j] = r1[j - 1];
			}
			r1[0] = xorResult;
		}

		if (majorityBit == r2[10])
		{
			int xorResult = r2[20] ^ r2[21];
			for (int j = sizeof(r2) - 1; j > 0; j--)
			{
				r2[j] = r2[j - 1];
			}
			r2[0] = xorResult;
		}

		if (majorityBit == r3[10])
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
		bool majorityBit = ((byte)r1[8] + (byte)r2[10] + (byte)r3[10]) >= 2 ? true : false;

		if (majorityBit == r1[8])
		{
			int xorResult = r1[13] ^ r1[16] ^ r1[17] ^ r1[18];
			for (int j = sizeof(r1) - 1; j > 0; j--)
			{
				r1[j] = r1[j - 1];
			}
			r1[0] = xorResult;
		}

		if (majorityBit == r2[10])
		{
			int xorResult = r2[20] ^ r2[21];
			for (int j = sizeof(r2) - 1; j > 0; j--)
			{
				r2[j] = r2[j - 1];
			}
			r2[0] = xorResult;
		}

		if (majorityBit == r3[10])
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
		//Serial.print(tobeAddedToKeystream ? '1' : '0');
		cipherKey[i] = tobeAddedToKeystream;
	}

	keyAndFrameInitialized = true;
}

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