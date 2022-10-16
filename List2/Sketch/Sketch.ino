bool key[64];
bool frame[22];
bool r1[19];
bool r2[22];
bool r3[23];
bool keyStream[228];
const byte r1SpecialBit = 8;
const byte r2r3SpecialBit = 10;
bool messageToSend[114];
bool messageToSendEncrypted[114];
bool messageReceived[114];

void setup()
{
	// if analog input pin 0 is unconnected, random analog
	// noise will cause the call to randomSeed() to generate
	// different seed numbers each time the sketch runs.
	// randomSeed() will then shuffle the random function.
	randomSeed(analogRead(0));

    memset(key, false, sizeof(key));
    memset(frame, false, sizeof(frame));
    memset(r1, false, sizeof(r1));
    memset(r2, false, sizeof(r2));
    memset(r3, false, sizeof(r3));
    memset(keyStream, false, sizeof(keyStream));
    memset(messageToSend, false, sizeof(messageToSend));
    memset(messageToSendEncrypted, false, sizeof(messageToSendEncrypted));
}

void loop()
{
    
}

void generateKey()
{
    for (int i = 0; i < sizeof(key); i++)
    {
        key[i] = random(2);
    }

    for (int i = 0; i < sizeof(frame); i++)
    {
        frame[i] = random(2);
    }
}

void phaseOne()
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

void phaseTwo()
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
        bool tobeAddedToKeystream = r1[sizeof(r1)-1] ^ r2[sizeof(r2) - 1] ^ r3[sizeof(r3) - 1];
        keyStream[i] = tobeAddedToKeystream;
    }
}

void generateMessage()
{
    for (int i = 0; i < sizeof(messageToSend); i++)
    {
        messageToSend[i] = random(2);
    }
}

void encryptMessage()
{
    generateMessage();
    for (int i = 0; i < sizeof(messageToSend); i++)
    {
        messageToSendEncrypted[i] = messageToSend[i] ^ keyStream[i];
    }
}