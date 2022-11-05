#include <A51Cipher.h>
#include <STIR.h>

A51Cipher cipherLib;
STIR commLib(0, 0);
const uint8_t ROLE = 0x1;
bool cipherKeyIsSent;

void setup()
{
    cipherKeyIsSent = true;
    cipherLib = A51Cipher();
    
    STIRConfig config(0, 0, 0, 2, 3, ProcessIncoming::WRITETOSERIAL);
    commLib = STIR(config, ROLE);
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
            Serial.println("Beginning to listen");
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
            char* message = (char*)malloc(sizeof(char) * (commLib.bufferMessageFromIRSize+1));
            cipherLib.decryptMessage(commLib.bufferMessageFromIR, commLib.bufferMessageFromIRSize, message);
            Serial.println(message);
            free(message);
            commLib.freeBufferMessageFromIR();
        }
    }
}