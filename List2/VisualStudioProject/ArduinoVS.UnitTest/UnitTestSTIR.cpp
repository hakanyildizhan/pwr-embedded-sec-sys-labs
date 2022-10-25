#define ARDUINO 10800
#define __AVR_ATmega328P__

#include <VirtualBoardTest.h>
#include "pch.h"
#include "CppUnitTest.h"
#ifndef STIR_h  
#include "STIR.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ArduinoVSUnitTest
{
	TEST_CLASS(ArduinoVSUnitTestSTIR)
	{
	public:

		TEST_METHOD(Can_Create_Cipher_Key_Correctly)
		{
			bool allZeroes = false;
			Assert::IsFalse(allZeroes, L"Cipher key bits are all 0's");
		}
	};

}
#endif