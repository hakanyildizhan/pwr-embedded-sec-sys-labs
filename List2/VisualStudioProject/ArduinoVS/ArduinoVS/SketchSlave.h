#include <A51Cipher.h>
#include <STIR.h>

A51Cipher cipherLib;
STIR commLib(PIN_NOT_SET, PIN_NOT_SET);
bool cipherKeyIsReceived;

void setup()
{
    commLib = STIR(PIN_NOT_SET, PIN_NOT_SET, PIN_NOT_SET, 2, 3, false);
    cipherKeyIsReceived = false;
    commLib.beginListenIR();
}

void loop()
{
    if (!cipherKeyIsReceived)
    {
        cipherKeyIsReceived = commLib.listenForCipherKey();
        if (cipherKeyIsReceived) {
            cipherLib = A51Cipher(commLib.cipherKey);
            commLib.beginListen();
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
            char* message = (char*)malloc(sizeof(char) * (commLib.bufferMessageFromIRSize + 1));
            cipherLib.decryptMessage(commLib.bufferMessageFromIR, commLib.bufferMessageFromIRSize, message);
            Serial.println(message);
            commLib.isReceiving = false;
            free(message);
            commLib.freeBufferMessageFromIR();
        }
    }
}