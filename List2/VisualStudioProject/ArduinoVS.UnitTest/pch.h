// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.



#ifndef PCH_H
#define PCH_H

#define ARDUINO 10800
#define __AVR_ATmega328P__

// add headers that you want to pre-compile here
#include <VirtualBoardTest.h>

#ifdef VISUAL_STUDIO
#include <cores/arduino/SerialSimulator.h>
#endif // VISUAL_STUDIO

#endif //PCH_H
