#include <A51Cipher.h>
#include <STIR.h>

A51Cipher cipherLib;
STIR commLib(0,0);
bool keyStream[228];
const uint8_t ROLE = 0x1;

void setup()
{
    cipherLib = A51Cipher();
    cipherLib.createCipherKey(keyStream);
    STIRConfig config(0, 0, 0, 9, 3, ProcessIncoming::WRITETOSERIAL);
    commLib = STIR(config, ROLE);
    commLib.beginListen();
}

void loop()
{
    commLib.communicationLoop();
    if (commLib.bufferMessageFromPCSize > 0)
    {
        bool* encryptedMessage = cipherLib.encryptMessage(commLib.bufferMessageFromPC);
        commLib.sendBinary(encryptedMessage);
    }
}