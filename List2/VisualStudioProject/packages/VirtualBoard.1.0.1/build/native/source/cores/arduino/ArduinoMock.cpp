/*
  Part of the Wiring project - http://wiring.org.co
  Copyright (c) 2004-06 Hernando Barragan
  Modified 13 August 2006, David A. Mellis for Arduino - http://www.arduino.cc/

  Modified for the MySensors Arduino library
  by Henrik Ekblad <henrik.ekblad@mysensors.org>
  Copyright (C) 2013-2017 Sensnology AB
  Full contributor list: https://github.com/mysensors/MySensors/graphs/contributors

  Modified 2022 for the VirtualBoard project https://github.com/virtual-maker/VirtualBoard
  Copyright (c) 2022 Immo Wache

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
 */

#include "Arduino.h"
#include <stdlib.h>

#define _X86_
#include <consoleapi.h>
#include <stdio.h>
#include <synchapi.h>

#include "GPIOMock.h"

#if defined(MY_PROCESS_SYNCHRONIZATION)
ProcessSynchronizationWrapper processSynchronizationWrapper;
#endif

GPIOClass GPIO;

void pinMode(uint8_t pin, uint8_t direction)
{
    GPIO._pinMode(pin, direction);
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    GPIO._digitalWrite(pin, value);
}

uint8_t digitalRead(uint8_t pin)
{
  return GPIO._digitalRead(pin);
}

uint16_t analogRead(uint8_t pin)
{
  return GPIO._analogRead(pin);
}

void analogWrite(uint8_t pin, uint16_t value)
{
    GPIO._analogWrite(pin, value);
}

void yield(void)
{
  _yield();
}

unsigned long millis(void)
{
    return 0;
}

unsigned long micros()
{
  return 0;
}

void delay(unsigned int millisec)
{
#if defined(VM_USE_HARDWARE) && !defined(VM_CALL_YIELD_WHILE_DELAY)
  // see:
  // https://docs.microsoft.com/de-de/windows/win32/api/synchapi/nf-synchapi-sleep
  //
  Sleep(millisec);
#else
  unsigned long startMillis = millis();
  do {
    _yield();
  } while (millis() - startMillis < millisec);
#endif
}

void _delay_milliseconds_and_proc_sync(unsigned int millis)
{
#if defined(MY_PROCESS_SYNCHRONIZATION)
  processSynchronizationWrapper.wait(millis);
#else
#pragma warning( push )
#pragma warning( disable : 4996)
  _sleep(millis);
#pragma warning( pop )
#endif
}

void delayMicroseconds(unsigned int micro)
{
  // This seems not real possible in Windows
  delay(micro / 1000);
}

void randomSeed(unsigned long seed)
{
  if (seed != 0) {
    srand(seed);
  }
}

long random(long howbig)
{
  if (howbig == 0) {
    return 0;
  }
  return rand() % howbig;
}

long random(long howsmall, long howbig)
{
  if (howsmall >= howbig) {
    return howsmall;
  }
  long diff = howbig - howsmall;
  return random(diff) + howsmall;
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
