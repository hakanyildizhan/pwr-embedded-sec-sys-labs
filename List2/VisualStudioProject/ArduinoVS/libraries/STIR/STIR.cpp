#pragma once
#ifndef STIR_h
#include "STIR.h"

#define ENABLE_LED_FEEDBACK             true

#ifndef IRremote_h
#include "IRremote.hpp"
#endif

Buffer::Buffer(uint16_t* buffer, uint8_t bufferSize)
{
	Messagebuffer = buffer;
	BufferSize = bufferSize;
}


STIR::STIR(int irReceivePin, int irSendPin)
{
	pinWaiting = PIN_NOT_SET;
	pinReceiving = PIN_NOT_SET;
	pinSending = PIN_NOT_SET;
	pinIRReceive = irReceivePin;
	pinIRSend = irSendPin;
	onReceive = &writeToSerial;
	state = State::WAITING;
	serialStarted = false;
	initiated = false;
	irStarted = false;
	bufferSize = 0;
	buffer = (uint16_t*)malloc(sizeof(uint16_t) * 1000);
	Buffer = Buffer::Buffer();
}

STIR::STIR(STIRConfig config, uint8_t cfgRole)
{
	role = cfgRole;
	pinWaiting = config.listeningLEDPin;
	pinReceiving = config.receivingLEDPin;
	pinSending = config.sendingLEDPin;
	pinIRReceive = config.irReceivePin;
	pinIRSend = config.irSendPin;
	onReceive = config.onMessageReceived;
	bufferSize = 0;
	buffer = (uint16_t*)malloc(sizeof(uint16_t) * 1000);
	Buffer = Buffer::Buffer();
	
	/*if (config.onMessageReceived != doNothing 
		&& config.incomingMessageProcessing == ProcessIncoming::WRITETOSERIAL)
	{
		onReceive = &writeToSerial;
	}*/

	serialStarted = false;
	initiated = false;
	irStarted = false;
	state = State::WAITING;
}

State STIR::getState()
{
	return state;
}

void STIR::beginListen()
{
	if (!initiated)
	{
		STIR::beginListenSerial();
		STIR::beginListenIR();
		initiated = true;
	}
}

bool STIR::listenForCipherKey(bool buffer[232])
{
	IrReceiver.begin(pinIRReceive, false);
	while (true)
	{
		if (IrReceiver.decodeNEC())
		{
			uint8_t command = IrReceiver.decodedIRData.command;
			uint16_t address = IrReceiver.decodedIRData.address;
			if (address == role || address == 0) {
				//delay(50);
				IrReceiver.resume();
				continue;
			}

			if (state == State::WAITING)
			{
				Serial.println("Receiving");
				state = State::RECEIVING;
			}

			Serial.print('#');
			Serial.print(bufferSize+1);
			Serial.print(' ');
			IrReceiver.printIRResultShort(&Serial);

			bufferCipherKey[bufferSize++] = command;

			if (command == 0xFF && address == 0xFF) // message end
			{
				IrReceiver.stop();
				state = State::WAITING;
				messageReceived = true;
				Serial.println("Cipher key received completely");
				IrSender.begin(pinIRSend, false);
				uint16_t buf[3];
				buf[0] = 9000;  // Mark 9ms
				buf[1] = 2250;  // Space 2.25ms
				buf[2] = 560;   // Burst
				for (size_t i = 0; i < 60; i++)
				{
					IrSender.sendNEC(0xFF, 0xFF, 0);
					delay(40);
					IrSender.sendRaw(buf, 3, 38);
					delay(96);
				}
				bufferSize = 0;
				buildCipherKey();
				return true;
			}
			IrReceiver.resume(); // Enable receiving of the next value
		}
		delay(50);
	}
	return false;
}

bool STIR::sendCipherKey(bool buffer[228])
{
	Serial.print("Key: ");
	for (size_t i = 0; i < 228; i++)
	{
		Serial.print(buffer[i] == true ? '1' : '0');
	}
	Serial.println();
	state = State::SENDING;
	
	bool cipherKey[232];
	memcpy(cipherKey, buffer, 228);
	uint8_t* hexMessage = convertBinToCommandSequence(cipherKey, 232);
	IrSender.begin(pinIRSend, false);

	uint16_t buf[3];
	buf[0] = 9000;  // Mark 9ms
	buf[1] = 2250;  // Space 2.25ms
	buf[2] = 560;   // Burst
	for (size_t i = 0; i < 232 / 8; i++)
	{
		IrSender.sendNEC(role, hexMessage[i], 0);
		delay(40);
		IrSender.sendRaw(buf, 3, 38);
		delay(96);
	}
	//delay(48);
	IrSender.sendNEC(0xFF, 0xFF, 0);
	delay(96);
	Serial.println("Sent cipher key, waiting for response");
	IrReceiver.start();
	IrReceiver.begin(pinIRReceive, false);
	for (size_t i = 0; i < 150; i++)
	{
		if (IrReceiver.decodeNEC())
		{
			if (IrReceiver.decodedIRData.address == role) {
				return false;
			}

			if (IrReceiver.decodedIRData.command == 0xFF && IrReceiver.decodedIRData.address == 0xFF) // message end
			{
				state = State::WAITING;
				Serial.println("Cipher key is received by the other party completely");
				IrReceiver.end();
				return true;
			}
			
			IrReceiver.resume();
		}
		delay(150);
	}
	
	return false;
}

void STIR::communicationLoop()
{
	// check incoming messages from PC to forward to IR
	if (Serial.available() > 0) {
		String str = Serial.readString();
		Serial.readString().toCharArray(bufferMessageFromPC, str.length());
		bufferMessageFromPCSize = str.length();
		Serial.print("received message from pc: ");
		Serial.println(str);
	}
	
	if (IrReceiver.decodeNEC())
	{
		uint8_t command = IrReceiver.decodedIRData.command;
		uint16_t address = IrReceiver.decodedIRData.address;
		if (address == role) {
			return;
		}

		if (state == State::WAITING)
		{
			if (command == 0xFF && role == 0xFF)
			{
				return;
			}

			Serial.println("Receiving");
			state = State::RECEIVING;
		}

		Serial.print("Address: ");
		Serial.println(address);

		if (command == 0xFF && role == 0xFF) // message end
		{
			state = State::WAITING;
			Buffer = Buffer::Buffer(buffer, bufferSize);
			Serial.println("Message received completely");
		}
		else
		{
			buffer[bufferSize++] = command;
			Serial.print("Received buffer #");
			Serial.println(bufferSize);
			IrReceiver.printIRResultShort(&Serial);
		}

		IrReceiver.resume(); // Enable receiving of the next value
	}
}

void STIR::beginListenSerial()
{
	if (!serialStarted)
	{
		Serial.begin(SERIAL_BAUDRATE);
		serialStarted = true;
	}
}

void STIR::beginListenIR()
{
	if (!irStarted)
	{
		if (pinIRReceive != 0)
		{
			
			IrReceiver.begin(pinIRReceive, false);
			IrReceiver.start();
		}

		//IrSender.setSendPin(pinIRSend);
		IrSender.begin(pinIRSend, false);
		//IrSender.enableIROut(38);
		
		irStarted = true;
	}
}

void STIR::endListen()
{
	if (serialStarted)
	{
		Serial.end();
		serialStarted = false;
	}

	if (irStarted)
	{
		IrReceiver.end();
		irStarted = false;
	}
}

//String STIR::receive()
//{
//	// TODO
//
//}

void STIR::sendString(String message)
{
	char messageArr[50];
	message.toCharArray(messageArr, message.length()+1);
	IrSender.sendPronto(messageArr, 0);
}

void STIR::freeBufferMessageFromPC()
{
	bufferMessageFromPCSize = 0;
	bufferMessageFromPC = '\0';
}

void STIR::sendBinary(bool binaryMessage[])
{
	uint16_t size = (bufferMessageFromPCSize - 1) * 8;
	Serial.print("Sending binary: ");
	freeBufferMessageFromPC();
	for (size_t i = 0; i < size; i++)
	{
		Serial.print(binaryMessage[i] == true ? '1' : '0');
	}
	Serial.println();
	uint8_t* hexMessage = convertBinToCommandSequence(binaryMessage, size);

	uint16_t buf[3];
	buf[0] = 9000;  // Mark 9ms
	buf[1] = 2250;  // Space 2.25ms
	buf[2] = 560;   // Burst
	for (size_t i = 0; i < size/8; i++)
	{
		IrSender.sendNEC(role, hexMessage[i], 0);
		delay(40);
		IrSender.sendRaw(buf, 3, 38);
		delay(96);
	}
	//delay(48);
	IrSender.sendNEC(role, 0xFF, 0);
	delay(96);
	Serial.println("sent all");
}

uint8_t* STIR::convertBinToCommandSequence(bool bin[], size_t size)
{
	uint8_t* decArray = (uint8_t*)malloc(sizeof(uint8_t) * (size / 8));

	for (size_t i = 0; i < size / 8; i++)
	{
		uint8_t dec = 0;
		for (size_t j = 0; j < 8; j++)
		{
			dec += bin[i * 8 + j] ? intpow(7 - j) : 0;
		}

		decArray[i] = dec;
	}

	return decArray;
}

//uint32_t* STIR::convertBinToCommandSequence(bool bin[], size_t size)
//{
//	Serial.print("bin: ");
//	for (size_t i = 0; i < size; i++)
//	{
//		Serial.print(bin[i] ? '1' : '0');
//	}
//	Serial.println();
//	uint32_t* decArray = (uint32_t*)malloc(sizeof(uint32_t) * (size / 8));
//	bool debugArr[16];
//	for (size_t i = 0; i < size / 8; i++)
//	{
//		uint32_t dec = 0;
//		for (size_t j = 0; j < 8; j++)
//		{
//			dec += (bin[i * 8 + j] ? 1 : 0) * intpow(23  - j); // actual bit
//			dec += (bin[i * 8 + j] ? 0 : 1) * intpow(31 - j); // inverse bit
//
//			/*Serial.print("Round ");
//			char str[1];
//			itoa(j, str, 10);
//			Serial.print(str);
//			Serial.print(" ");
//			Serial.print(dec);
//			Serial.println();*/
//			debugArr[8+j] = bin[i * 8 + j];
//			debugArr[j] = !bin[i * 8 + j];
//		}
//		Serial.print("Command sequence ");
//		/*char str[1];
//		itoa(i, str, 10);
//		Serial.print(str);
//		Serial.print(" ");*/
//		for (size_t i = 0; i < 16; i++)
//		{
//			Serial.print(debugArr[i] ? '1' : '0');
//		}
//		Serial.println();
//		dec += 65025;
//		decArray[i] = dec;
//
//		Serial.print(dec);
//		Serial.println();
//	}
//
//	/*char str[5];
//	itoa(decArray[0], str, 10);
//	Serial.print(str);*/
//
//	// not reversing:
//	/*for (int j = 0; j < size / 32 + 1; j++)
//	{
//		uint32_t dec = 0;
//		for (int i = size - 1 - (j*32), k = 0; i >size-(j+1)-33, i >= 0; i--, k++)
//		{
//			dec += bin[i] * pow(2, k);
//		}
//		decArray[j] = dec;
//	}*/
//
//	// reversing
//	/*for (int j = 0; j < size / 32 + 1; j++)
//	{
//		uint32_t dec = 0;
//		for (int i = j * 32; i < (j+1)*32 && i < size; i++)
//		{
//			dec += bin[i] * pow(2, i%32);
//		}
//		decArray[j] = dec;
//	}*/
//
//	return decArray;
//	//return binChar;
//}

uint8_t powers[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

uint8_t STIR::intpow(uint8_t x)
{
	/*uint8_t result = 1;
	for (uint8_t i = 0; i < x; i++)
	{
		result *= 2;
	}
	return result;*/
	return powers[x];
}

void STIR::buildCipherKey()
{
	bool cipherKey[232];
	memset(cipherKey, false, 232);

	for (size_t i = 0; i < 29; i++)
	{
		for (uint8_t j = 0; j < 8; j++)
		{
			cipherKey[i*8+7-j] = bufferCipherKey[i] & 1;
			bufferCipherKey[i] /= 2;
		}
	}
	
	Serial.print("Received key: ");
	for (size_t i = 0; i < 228; i++)
	{
		Serial.print(cipherKey[i] == true ? '1' : '0');
	}
}

String STIR::convertHexToBin(const char s[]) {
	String out;
	for (int i = 0; i < sizeof(s); i++) {
		byte n;
		if (s[i] <= '9' && s[i] >= '0')
			n = s[i] - '0';
		else
			n = 10 + s[i] - 'A';
		for (byte j = 3; j >= 0; --j)
			out.concat((n & (1 << j)) ? '1' : '0');
	}

	return out;
}

String STIR::convertBinToHex(const String& s) {
	String out;
	for (int i = 0; i < s.length(); i += 4) {
		int n = 0;
		for (int j = i; j < i + 4; ++j) {
			n <<= 1;
			if (s[j] == '1')
				n |= 1;
		}

		if (n <= 9)
			out.concat('0' + n);
		else
			out.concat('A' + n - 10);
	}

	return out;
}
#endif