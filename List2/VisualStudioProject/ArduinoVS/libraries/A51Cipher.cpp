#include "Arduino.h"
#include "A51Cipher.h"
#include <cores/arduino/WString.h>
#include <cores/arduino/Print.h>

bool initialized;
bool key[64];
bool frame[22];
bool r1[19];
bool r2[22];
bool r3[23];
const byte r1SpecialBit = 8;
const byte r2r3SpecialBit = 10;
bool messageToSendEncrypted[114];
bool messageReceived[114];

A51Cipher::A51Cipher()
{
    initialize();
}

void A51Cipher::initialize()
{
    initialized = false;
    memset(key, false, sizeof(key));
    memset(frame, false, sizeof(frame));
    memset(r1, false, sizeof(r1));
    memset(r2, false, sizeof(r2));
    memset(r3, false, sizeof(r3));
    memset(messageToSendEncrypted, false, sizeof(messageToSendEncrypted));
    initialized = true;
}

bool* A51Cipher::createCipherKey(bool keyStream[228])
{
    if (!initialized)
    {
        initialize();
    }
    generateKeyAndFrame();
    phaseOne();
    memset(keyStream, false, sizeof(keyStream));
    return phaseTwo(keyStream);
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

bool* A51Cipher::phaseTwo(bool keyStream[228])
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
    return keyStream;
}

bool* A51Cipher::encryptMessage(bool encryptionCypher[114], char message[], bool encryptedMessage[])
{
    bool* binaryMessage = (bool*)malloc(sizeof(bool) * (sizeof(message) * 8));
    memset(binaryMessage, false, sizeof(binaryMessage));

    for (int i = 0; i < sizeof(message); i++)
    {
        char character = message[i];

        for (int j = 7; j >= 0; j--)
        {
            binaryMessage[i*8+(7-j)] = bitRead(character, j) == 1;
        }
    }

    for (int i = 0; i < sizeof(encryptedMessage); i++)
    {
        encryptedMessage[i] = binaryMessage[i] ^ encryptionCypher[i%114];
    }

    free(binaryMessage);
    return encryptedMessage;
}

char* A51Cipher::decryptMessage(bool decryptionCypher[114], bool encryptedMessage[], char message[])
{

}