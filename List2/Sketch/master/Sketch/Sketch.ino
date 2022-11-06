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
            //delay(1000);
        }
    }
    else
    {
        commLib.communicationLoop();
        if (commLib.bufferMessageFromPCSize > 0)
        {
            bool* encryptedMessage = cipherLib.encryptMessage(commLib.bufferMessageFromPC);
            commLib.sendBinary(encryptedMessage);
        }
        if (commLib.bufferMessageFromIRSize > 0)
        {
            char* message = (char*)malloc(sizeof(char) * (commLib.bufferMessageFromIRSize+1));
            cipherLib.decryptMessage(commLib.bufferMessageFromIR, commLib.bufferMessageFromIRSize, message);
            Serial.println(message);
            commLib.isReceiving = false;
            free(message);
            commLib.freeBufferMessageFromIR();
        }
    }
}