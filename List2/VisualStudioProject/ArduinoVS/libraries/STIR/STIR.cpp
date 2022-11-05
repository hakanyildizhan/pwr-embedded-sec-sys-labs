#pragma once
#ifndef STIR_h
#include "STIR.h"

#define ENABLE_LED_FEEDBACK false

#ifndef IRremote_h
#include "IRremote.hpp"
#endif

//Buffer::Buffer(uint8_t* buffer, uint8_t bufferSize)
//{
//	Messagebuffer = buffer;
//	BufferSize = bufferSize;
//}

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
	irListenStarted = false;
	irTransmitStarted = false;
	bufferSize = 0;
	buffer = (uint8_t*)malloc(sizeof(uint8_t) * 1000);
	//Buffer = Buffer::Buffer();
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
	buffer = (uint8_t*)malloc(sizeof(uint8_t) * 1000);
	//Buffer = Buffer::Buffer();
	
	/*if (config.onMessageReceived != doNothing 
		&& config.incomingMessageProcessing == ProcessIncoming::WRITETOSERIAL)
	{
		onReceive = &writeToSerial;
	}*/

	serialStarted = false;
	initiated = false;
	irListenStarted = false;
	irTransmitStarted = false;
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

bool STIR::listenForCipherKey()
{
	//beginListenIR();
	beginListenSerial();
	Serial.println("Listening");
	
	bool messageFinished1 = false;
	bool messageFinished2 = false;
	while (true)
	{
		//Serial.println("#");
		if (IrReceiver.decodeNEC())
		{
			//Serial.println("Got sth");
			if (IrReceiver.decodedIRData.address == role || IrReceiver.decodedIRData.address == 0) {
				IrReceiver.resume();
				continue;
			}

			if (state == State::WAITING)
			{
				Serial.println("Receiving");
				state = State::RECEIVING;
			}

			if (!messageFinished1 &&
				IrReceiver.decodedIRData.command == 0xFF) {
				messageFinished1 = true;
			}
			else if (messageFinished1 && !messageFinished2 &&
				IrReceiver.decodedIRData.command == 0x1) {
				messageFinished2 = true;
			}
			else if (messageFinished1 && messageFinished2 &&
				IrReceiver.decodedIRData.command == 0xFF) {
				bufferCipherKey[29] = 0;
				bufferCipherKey[30] = 0;
				bufferCipherKey[31] = 0;
				bufferSize = 29;
				state = State::WAITING;
				endListenIR();
				messageReceived = true;
				Serial.println("Cipher key received completely");
				for (size_t i = 0; i < 3; i++)
				{
					sendAcknowledgedSequence();
					delay(1000);
				}
				//sendAcknowledgedSequence();
				bufferSize = 0;
				buildCipherKey();
				return true;
			}
			else {
				messageFinished1 = false;
				messageFinished2 = false;
			}
			delay(100);
			Serial.print('#');
			Serial.print(bufferSize+1);
			Serial.print(' ');
			IrReceiver.printIRResultShort(&Serial);
			bufferCipherKey[bufferSize++] = IrReceiver.decodedIRData.command;
			IrReceiver.resume(); // Enable receiving of the next value
		}
		else
		{
			delay(100);
			IrReceiver.resume();
		}
	}
	return false;
}

bool STIR::sendCipherKey(bool* buffer)
{
	beginTransmitIR();
	Serial.print("Cipher key: ");
	for (size_t i = 0; i < 228; i++)
	{
		Serial.print(buffer[i] == true ? '1' : '0');
	}
	Serial.println();
	state = State::SENDING;
	
	bool cipherKeyToSend[232];
	memcpy(cipherKeyToSend, buffer, 228);
	//Serial.println("Preparing to send cipher key");
	uint8_t* hexMessage = convertBinToCommandSequence(cipherKeyToSend, 232);
	Serial.println("Prepared cipher key");
	
	for (size_t i = 0; i < 232 / 8; i++)
	{
		IrSender.sendNEC(role, hexMessage[i], 0);
		delay(40);
		IrSender.sendRaw(NEC_BUFFER, 3, 38);
		delay(96);
	}
	Serial.println("sendFinishSequence");
	sendFinishSequence();
	Serial.println("Sent cipher key, waiting for response");
	beginListenIR();
	return waitForAcknowledgedSequence();
}

void STIR::communicationLoop()
{
	// check incoming messages from PC to forward to IR
	beginListenSerial();

	if (Serial.available() > 0)
	{
		uint8_t charCount = 0;
		while (Serial.available() > 0)
		{
			static char message[10];
			static unsigned int message_pos = 0;

			char inByte = Serial.read();
			charCount++;
			bufferMessageFromPCSize++;

			if (inByte != '\n' && (message_pos < 10 - 1))
			{
				//Add the incoming byte to our message
				message[message_pos] = inByte;
				message_pos++;
			}
			else
			{
				message[message_pos] = '\0';
				strcat(bufferMessageFromPC, message);
				strcat(bufferMessageFromPC, &inByte);
				message_pos = 0;
			}
			delay(50);
		}
		bufferMessageFromPC[bufferMessageFromPCSize] = '\0';
		//Serial.println(bufferMessageFromPC);
		//Serial.print("Total chars: ");
		////Serial.println(bufferMessageFromPCSize);
		//Serial.println(strlen(bufferMessageFromPC));
	}
	
	bool stop1 = false;
	bool stop2 = false;
	if (IrReceiver.decodeNEC())
	{
		if (IrReceiver.decodedIRData.address == role || IrReceiver.decodedIRData.address == 0) {
			IrReceiver.resume();
			return;
		}

		if (state == State::WAITING)
		{
			Serial.println("Receiving");
			state = State::RECEIVING;
		}

		Serial.print("Address: ");
		Serial.println(IrReceiver.decodedIRData.address);

		buffer[bufferSize++] = IrReceiver.decodedIRData.command;
		Serial.print("Received buffer #");
		Serial.println(bufferSize);
		IrReceiver.printIRResultShort(&Serial);
		int loop = 0;

		while (true)
		{
			if (IrReceiver.decodeNEC())
			{
				loop = 0;
				if (!stop1 && IrReceiver.decodedIRData.command == 0xFF)
				{
					stop1 = true;
				}
				else if (stop1 && !stop2 && IrReceiver.decodedIRData.command == 0x1)
				{
					stop2 = true;
				}
				else if (stop1 && stop2 && IrReceiver.decodedIRData.command == 0xFF)
				{
					IrReceiver.resume();
					sendAcknowledgedSequence();
					state = State::WAITING;
					//Buffer = Buffer::Buffer(buffer, bufferSize);
					Serial.println("Message received completely");
					bufferMessageFromIR = convertCommandSequenceToBin(buffer, bufferSize);
					bufferMessageFromIRSize = bufferSize * 8;
					memset(buffer, 0, bufferSize);
					bufferSize = 0;
					return;
				}
				else 
				{
					if (stop1)
					{
						buffer[bufferSize++] = 0xFF;
						Serial.print("Received buffer #");
						Serial.println(bufferSize);
						stop1 = false;
					}
					else if (stop1 && stop2)
					{
						buffer[bufferSize++] = 0x1;
						Serial.print("Received buffer #");
						Serial.println(bufferSize);
					}

					stop1 = false;
					stop2 = false;

					buffer[bufferSize++] = IrReceiver.decodedIRData.command;
					Serial.print("Received buffer #");
					Serial.println(bufferSize);
					IrReceiver.printIRResultShort(&Serial);
				}

				delay(100);
				IrReceiver.resume();
			}
			else
			{
				loop++;
				if (loop == 50)
				{
					state = State::WAITING;
					//Buffer = Buffer::Buffer(buffer, bufferSize);
					Serial.println("Message ended abruptly");
					break;
				}
				delay(100);
				IrReceiver.resume();
			}
		}

		IrReceiver.resume();
	}
	else
	{
		state = State::WAITING;
		
		//Serial.println("Message ended abruptly");
		//freeBufferMessageFromPC();
	}
	//delay(100);
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
	if (!irListenStarted)
	{
		IrReceiver.begin(pinIRReceive, false);
		irListenStarted = true;
	}
}

void STIR::beginTransmitIR()
{
	if (!irTransmitStarted)
	{
		IrSender.begin(pinIRSend, false);
		irTransmitStarted = true;
	}
}

void STIR::endListen()
{
	endListenSerial();
	endListenIR();
}

void STIR::endListenIR()
{
	if (irListenStarted)
	{
		IrReceiver.end();
		irListenStarted = false;
	}
}

void STIR::endListenSerial()
{
	if (serialStarted)
	{
		Serial.end();
		serialStarted = false;
	}
}

void STIR::sendString(String message)
{
	char messageArr[50];
	message.toCharArray(messageArr, message.length()+1);
	IrSender.sendPronto(messageArr, 0);
}

void STIR::freeBufferMessageFromPC()
{
	bufferMessageFromPCSize = 0;
	memset(bufferMessageFromPC, 0, 80);
}

void STIR::freeBufferMessageFromIR()
{
	memset(bufferMessageFromIR, false, bufferMessageFromIRSize);
	bufferMessageFromIRSize = 0;
}

void STIR::sendBinary(bool binaryMessage[])
{
	beginTransmitIR();
	uint16_t size = (bufferMessageFromPCSize - 1) * 8;
	Serial.print("Sending binary with size ");
	Serial.print(size);
	Serial.print(": ");
	//freeBufferMessageFromPC();
	for (size_t i = 0; i < size; i++)
	{
		Serial.print(binaryMessage[i] == true ? '1' : '0');
	}
	Serial.println();
	uint8_t* hexMessage = convertBinToCommandSequence(binaryMessage, size);

	for (size_t i = 0; i < size/8; i++)
	{
		IrSender.sendNEC(role, hexMessage[i], 0);
		delay(40);
		IrSender.sendRaw(NEC_BUFFER, 3, 38);
		delay(96);
	}
	sendFinishSequence();
	Serial.println("sent all");
	waitForAcknowledgedSequence();
	freeBufferMessageFromPC();
}

bool STIR::waitForAcknowledgedSequence()
{
	bool messageAcknowledged1 = false;
	bool messageAcknowledged2 = false;
	uint8_t loop = 0;
	while (true)
	{
		if (IrReceiver.decodeNEC())
		{
			if (IrReceiver.decodedIRData.address == role || IrReceiver.decodedIRData.address == 0) {
				delay(100);
				IrReceiver.resume();
				messageAcknowledged1 = false;
				messageAcknowledged2 = false;
				continue;
			}

			if (!messageAcknowledged1 &&
				IrReceiver.decodedIRData.command == 0x1) {
				messageAcknowledged1 = true;
				//Serial.println("Got messageAcknowledged1");
				delay(100);
				IrReceiver.resume();
			}
			else if (messageAcknowledged1 && !messageAcknowledged2 &&
				IrReceiver.decodedIRData.command == 0xFF) {
				messageAcknowledged2 = true;
				//Serial.println("Got messageAcknowledged2");
				IrReceiver.resume();
			}
			else if (messageAcknowledged1 && messageAcknowledged2 &&
				IrReceiver.decodedIRData.command == 0x1) {
				state = State::WAITING;
				Serial.println("Cipher key is received by the other party completely");
				endListenIR();
				return true;
			}
			else {
				/*messageAcknowledged1 = false;
				messageAcknowledged2 = false;*/
				/*Serial.print("Got ");
				Serial.println(IrReceiver.decodedIRData.command);
				IrReceiver.printIRResultShort(&Serial);*/
				loop++;
				if (loop == 5)
				{
					state = State::WAITING;
					Serial.println("Cipher key was not acknowledged by the other party");
					return false;
				}
				delay(1000);
				IrReceiver.resume();
			}
			//delay(100);
		}
		else
		{
			delay(100);
			IrReceiver.resume();
		}
	}
	return false;
}

void STIR::sendFinishSequence()
{
	//beginTransmitIR();
	IrSender.sendNEC(role, 0xFF, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
	IrSender.sendNEC(role, 0x1, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
	IrSender.sendNEC(role, 0xFF, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
}

void STIR::sendAcknowledgedSequence()
{
	beginTransmitIR();
	IrSender.sendNEC(role, 0x1, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
	IrSender.sendNEC(role, 0xFF, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
	IrSender.sendNEC(role, 0x1, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
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

bool* STIR::convertCommandSequenceToBin(uint8_t commandSequence[], size_t size)
{
	bool* binary = (bool*)malloc(sizeof(bool) * (size * 8));
	uint8_t k = 0;
	//char hexChar[] = { '\0', '\0' };
	for (size_t i = 0; i < size; i++)
	{
		char* hexChar = (char*)malloc(sizeof(char) * 2);
		sprintf(hexChar, "%02X", commandSequence[i]);
		for (size_t l = 0; l < 2; l++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				binary[k++] = hexCharToBin[hexChar[l] - (hexChar[l] < 57 ? 48 : 55)][j];
			}
		}
	}
	return binary;
}

uint8_t STIR::intpow(uint8_t x)
{
	return powersOf2[x];
}

void STIR::buildCipherKey()
{
	bool cipherKeyTemp[232];
	memset(cipherKeyTemp, false, 232);

	for (size_t i = 0; i < 29; i++)
	{
		for (uint8_t j = 0; j < 8; j++)
		{
			cipherKeyTemp[i*8+7-j] = bufferCipherKey[i] & 1;
			bufferCipherKey[i] /= 2;
		}
	}
	
	memcpy(cipherKey, cipherKeyTemp, 228);
	free(cipherKeyTemp);
	Serial.print("Received key: ");
	for (size_t i = 0; i < 228; i++)
	{
		Serial.print(cipherKey[i] == true ? '1' : '0');
	}
}

#endif