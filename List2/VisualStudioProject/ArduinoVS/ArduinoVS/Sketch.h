#include <A51Cipher.h>
#include <STIR.h>

A51Cipher cipherLib;
STIR commLib(0,0);
bool keyStream[228];

void setup()
{
    //cipherLib.createCipherKey(keyStream);
    bool cipherKey[228] = { false,true,true,true,false,false,true,true,true,false,true,false,false,false,false,false,true,false,true,true,false,true,false,false,true,true,false,false,false,false,false,false,true,false,true,false,true,true,true,true,false,true,false,false,true,true,true,false,false,false,false,true,false,true,false,false,false,false,true,false,false,true,true,true,true,true,true,true,false,true,true,false,false,true,true,false,true,false,false,true,true,true,false,false,false,true,false,false,true,false,true,false,false,true,true,true,false,false,false,false,true,true,true,false,true,true,false,true,false,true,false,false,false,false,true,false,true,true,true,true,true,true,false,false,false,true,true,true,true,false,true,true,false,false,true,true,true,true,true,false,true,true,true,false,true,true,true,true,false,false,true,true,true,true,true,false,true,false,false,true,false,false,false,false,false,true,false,false,true,true,true,false,true,true,true,true,false,true,true,false,false,false,true,false,false,false,false,true,false,false,false,true,true,true,false,false,false,true,true,true,true,false,true,true,true,true,true,true,false,true,true,true,true,true,true,true,false,false,true,false,false,true,true,true,false,true,true,false };

    cipherLib = A51Cipher(cipherKey);
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
    commLib.sendBinary(encryptedMessage);
}

