#include <A51Cipher.h>
#include <STIR.h>

A51Cipher cipherLib;
STIR commLib(0, 0);
bool buffer[228];
const uint8_t ROLE = 0x2;
bool cipherKeyIsReceived = false;

void setup()
{
    STIRConfig config(0, 0, 0, 2, 3, ProcessIncoming::WRITETOSERIAL);
    commLib = STIR(config, ROLE);
    cipherKeyIsReceived = false;
    Serial.begin(115200);
}

void loop()
{
    if (!cipherKeyIsReceived)
    {
        cipherKeyIsReceived = commLib.listenForCipherKey(buffer);
        if (cipherKeyIsReceived) {
            Serial.println("cipher key received");
            cipherKeyIsReceived = true;
            cipherLib = A51Cipher(buffer);
            commLib.beginListen();
        }
    }
    else
    {
        //commLib.communicationLoop();
    }
    
    /*if (commLib.bufferMessageFromPCSize > 0)
    {
        bool* encryptedMessage = cipherLib.encryptMessage(commLib.bufferMessageFromPC);
        commLib.sendBinary(encryptedMessage);
    }*/
}