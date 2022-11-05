#ifndef MS_CPP_UNITTESTFRAMEWORK
#include <CppUnitTest.h>
#endif

#ifndef STIR_h  
#include <STIR.h>
#endif

#ifndef A51Cipher_h  
#include <A51Cipher.h>
#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ArduinoVSUnitTest
{
	TEST_CLASS(ArduinoVSUnitTestSTIR)
	{
	public:

		TEST_METHOD(Can_Convert_To_Hex_Correctly)
		{
			/*char originalMessage[] = "Hello";
			A51Cipher cipher;
			bool binMsg[5 * 8];
			cipher.convertStringToBinary(originalMessage, binMsg);
			STIR stir(0, 0);
			char* a = stir.convertBinToHexString(binMsg, 5 * 8);
			Assert::IsFalse(false, L"Cipher key bits are all 0's");*/

			/*bool message[40] = { false, true, false, false, true, false, false, false, false, true, true, false, false, true, false, true, false, true, true, false, true, true, false, false, false, true, true, false, true, true, false, false, false, true, true, false, true, true, true, true };*/

			bool message[8] = { false, true, false, false, true, false, false, false};
			STIR stir(0, 0);
			uint8_t* dec = stir.convertBinToCommandSequence(message, 8);
			/*Assert::IsTrue(dec[0] == 1701604463, L"Incorrect conversion");
			Assert::IsTrue(dec[1] == 72, L"Incorrect conversion");*/
			Assert::IsTrue(dec[0] == 909551122, L"Incorrect conversion");
			Assert::IsTrue(dec[1] == 246, L"Incorrect conversion");
		}

		TEST_METHOD(Can_Convert_To_Bin_Correctly)
		{
			STIR stir(0, 0);
			uint8_t commandSequence[] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F };
			bool* bin = stir.convertCommandSequenceToBin(commandSequence, 5);
			//01001000 01100101 01101100 01101100 01101111

			bool expected[40] =
			{
				false, true, false, false, true, false, false, false,
				false, true, true, false, false, true, false, true,
				false, true, true, false, true, true, false, false,
				false, true, true, false, true, true, false, false,
				false, true, true, false, true, true, true, true
			};

			for (size_t i = 0; i < 40; i++)
			{
				Assert::AreEqual(expected[i], bin[i], L"Different");
			}
		}
	};

}