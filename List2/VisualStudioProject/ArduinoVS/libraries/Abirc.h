#pragma once
#ifndef Abirc_h
#define Abirc_h

class Abirc
{
    public:
        Abirc(int pin);
        void dot();
        void dash();
    private:
        int _pin;
};

#endif