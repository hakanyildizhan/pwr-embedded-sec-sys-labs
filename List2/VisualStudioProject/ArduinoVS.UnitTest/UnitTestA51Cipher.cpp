#ifndef MS_CPP_UNITTESTFRAMEWORK
#include <CppUnitTest.h>
#endif

#ifndef A51Cipher_h  
#include <A51Cipher.h>
#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ArduinoVSUnitTest
{
	TEST_CLASS(ArduinoVSUnitTestA51Cipher)
	{
	public:

		TEST_METHOD(Can_Create_Cipher_Key_Correctly)
		{
			A51Cipher cipher;
			bool cipherKey[228];
			cipher.createCipherKey(cipherKey);
			Assert::IsTrue(sizeof(cipherKey) == 228, L"Size of the cipher key is not correct");

			bool allZeroes = true;
			for (int i = 0; i < 228; i++)
			{
				if (cipherKey[i] == true)
				{
					allZeroes = false;
					break;
				}
			}
			Assert::IsFalse(allZeroes, L"Cipher key bits are all 0's");
		}

		TEST_METHOD(Created_Cipher_Keys_Are_Different)
		{
			A51Cipher cipher;
			bool cipherKeyOne[228];
			cipher.createCipherKey(cipherKeyOne);
			bool cipherKeyTwo[228];
			cipher.createCipherKey(cipherKeyTwo);

			bool different = false;
			for (int i = 0; i < 228; i++)
			{
				if (cipherKeyOne[i] != cipherKeyTwo[i])
				{
					different = true;
					break;
				}
			}
			Assert::IsTrue(different, L"Cipher keys are not different");
		}

		TEST_METHOD(Message_Is_Encrypted_Correctly)
		{
			A51Cipher cipher;
			bool cipherKeyOne[228];
			cipher.createCipherKey(cipherKeyOne);

			char originalMessage[] = "Hello my friends";
			bool* encryptedMessage = cipher.encryptMessage(originalMessage);

			char decryptedMessage[17];
			cipher.decryptMessage(encryptedMessage, sizeof(encryptedMessage), decryptedMessage);

			bool same = true;
			for (int i = 0; i < 16; i++)
			{
				if (originalMessage[i] != decryptedMessage[i])
				{
					same = false;
					break;
				}
			}
			Assert::IsTrue(same, L"Original and decrypted messages are not the same");
		}

		TEST_METHOD(Throws_Exception_When_Encrypting_Message_If_CipherKey_Is_Not_Generated)
		{
			auto func = [this] {EncryptBeforeCreatingCypherKey(); };
			Assert::ExpectException<std::runtime_error>(func);
		}

		TEST_METHOD(Throws_Exception_When_Decrypting_Message_If_CipherKey_Is_Not_Generated)
		{
			auto func = [this] {DecryptBeforeCreatingCypherKey(); };
			Assert::ExpectException<std::runtime_error>(func);
		}

		void EncryptBeforeCreatingCypherKey()
		{
			A51Cipher cipher;
			
			char originalMessage[] = "Hello my friends";
			bool* encryptedMessage = cipher.encryptMessage(originalMessage);
		}

		void DecryptBeforeCreatingCypherKey()
		{
			A51Cipher cipher;
			bool encryptedMessage[128];
			char decryptedMessage[17];
			cipher.decryptMessage(encryptedMessage, sizeof(encryptedMessage), decryptedMessage);
		}
	};

	
}