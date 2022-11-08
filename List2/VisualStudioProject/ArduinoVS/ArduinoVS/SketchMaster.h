#include <A51Cipher.h>
#include <STIR.h>

A51Cipher cipherLib;
STIR commLib(PIN_NOT_SET, PIN_NOT_SET);
bool cipherKeyIsSent;

void setup()
{
    cipherKeyIsSent = false;
    cipherLib = A51Cipher();
    commLib = STIR(PIN_NOT_SET, PIN_NOT_SET, PIN_NOT_SET, 2, 3, true);
    commLib.beginListenSerial();
}

void loop()
{
    if (!cipherKeyIsSent)
    {
        cipherLib.createCipherKey();
        cipherKeyIsSent = commLib.sendCipherKey(cipherLib.cipherKey);
        if (cipherKeyIsSent) 
        {
            commLib.beginListen();
        }
    }
    else
    {
        commLib.communicationLoop();
        if (commLib.bufferMessageFromPCSize > 0)
        {
            uint8_t* encryptedMessage = cipherLib.encryptMessage2(commLib.bufferMessageFromPC);
            commLib.sendBinary(encryptedMessage);
        }
        if (commLib.bufferMessageFromIRSize > 0)
        {
            cipherLib.decryptMessage(commLib.buffer, commLib.bufferMessageFromIRSize);
            cipherLib.freeBuffer();
            commLib.freeBufferMessageFromIR();
            commLib.beginListen();
        }
    }
}