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

STIR::STIR(STIRConfig config)
{
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


void STIR::communicationLoop()
{
	// check incoming messages from PC to forward to IR
	if (Serial.available() > 0) {
		String str = Serial.readString();
		Serial.readString().toCharArray(bufferMessageFromPC, str.length());
		bufferMessageFromPCSize = str.length();
	}
	
	if (IrReceiver.decodeNEC())
	{
		Serial.println("DECODING");
		if (state == State::WAITING)
		{
			Serial.println("Receiving");
			state = State::RECEIVING;
		}

		uint8_t command = IrReceiver.decodedIRData.command;
		if (command == 0x1) {
			return;
		}
		uint16_t address = IrReceiver.decodedIRData.address;
		Serial.print("Address: ");
		Serial.println(address);

		if (address == 255) // message end
		{
			state = State::WAITING;
			Buffer = Buffer::Buffer(buffer, bufferSize);
			Serial.println("Message received completely");
		}
		else
		{
			buffer[bufferSize++] = address;
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
	//Serial.print("Sending binary: ");
	uint16_t size = (bufferMessageFromPCSize - 1) * 8;
	freeBufferMessageFromPC();
	/*for (size_t i = 0; i < size; i++)
	{
		Serial.print(binaryMessage[i] == true ? '1' : '0');
	}
	Serial.println();*/
	uint16_t* hexMessage = convertBinToCommandSequence(binaryMessage, size);

	for (size_t i = 0; i < size/8; i++)
	{
		IrSender.sendNEC(hexMessage[i], 0x1, 0);
		delay(100);
		/*Serial.print("sent ");
		Serial.println(i);*/
	}
	delay(100);
	IrSender.sendNEC(0xFF, 0x1, 0);
	//Serial.println("sent all");
}

uint16_t* STIR::convertBinToCommandSequence(bool bin[], size_t size)
{
	uint16_t* decArray = (uint16_t*)malloc(sizeof(uint16_t) * (size / 8));

	for (size_t i = 0; i < size / 8; i++)
	{
		uint16_t dec = 0;
		for (size_t j = 0; j < 8; j++)
		{
			dec += (bin[i * 8 + j] ? 1 : 0) * intpow(7 - j);
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

uint16_t STIR::intpow(uint8_t x)
{
	uint16_t result = 1;
	for (uint8_t i = 0; i < x; i++)
	{
		result *= 2;
	}
	return result;
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