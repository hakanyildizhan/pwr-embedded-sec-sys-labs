#include <libraries/A51Cipher.h>
#include <libraries/STIR.h>

A51Cipher cipherLib;
STIR commLib;
bool keyStream[228];

void setup()
{
    cipherLib.createCipherKey(keyStream);
    STIRConfig config(0, 0, 0, 2, 3, ProcessIncoming::WRITETOSERIAL);
    commLib = STIR(config);
    commLib.beginListen();
}

void loop()
{
    char message[] = "Hello";
    bool encryptedMessage[40];
    cipherLib.encryptMessage(message, 5, encryptedMessage);
    commLib.communicationLoop();
}

