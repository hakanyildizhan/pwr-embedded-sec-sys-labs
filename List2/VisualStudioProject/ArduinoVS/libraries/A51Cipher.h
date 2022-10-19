#pragma once
#ifndef A51Cipher_h
#define A51Cipher_h

class A51Cipher
{
    public:
        A51Cipher();
        void generateKeyAndFrame();
        bool* createCipherKey(bool keyStream[]);
        bool* encryptMessage(bool encryptionCypher[114], char message[], bool encryptedMessage[]);
        char* decryptMessage(bool decryptionCypher[114], bool encryptedMessage[], char message[]);
    private:
        void initialize();
        void phaseOne();
        bool* phaseTwo(bool keyStream[]);
};

#endif