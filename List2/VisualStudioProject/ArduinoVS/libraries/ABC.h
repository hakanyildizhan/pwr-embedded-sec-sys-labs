#pragma once
#ifndef ABC_h
#define ABC_h

class ABC
{
    public:
        ABC(int pin);
        void dot();
        void dash();
    private:
        int _pin;
};

#endif