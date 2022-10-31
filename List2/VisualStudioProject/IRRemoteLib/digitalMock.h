#pragma once
#ifndef Arduino_h
#include <Arduino.h>
#endif
#include <cores/arduino/SerialSimulator.h>
void digitalWriteFast(uint8_t sendPin, int highOrLow)
{
	Serial.write(sendPin);
}

void pinModeFast(uint8_t sendPin, int highOrLow)
{
	Serial.write(sendPin);
}

int digitalReadFast(uint_fast8_t receivePin)
{
	Serial.read();
	return 0;
}

uint8_t digitalPinToBitMask(uint_fast8_t pin)
{
	return 0;
}

uint8_t digitalPinToPort(uint_fast8_t pin)
{
	return 0;
}

uint8_t* portInputRegister(uint8_t)
{
	return 0;
}

uint8_t clockCyclesPerMicrosecond()
{
	return 1;
}