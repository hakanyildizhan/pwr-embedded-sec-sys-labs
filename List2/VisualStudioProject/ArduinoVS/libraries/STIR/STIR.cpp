#pragma once
#ifndef STIR_h
#include "STIR.h"

#define ENABLE_LED_FEEDBACK false

#ifndef IRremote_h
#include "IRremote.hpp"
#endif

const uint8_t STIR::powersOf2[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
const bool STIR::hexCharToBin[16][4] =
{
	{ false, false, false, false }, // 0 - 0000
	{ false, false, false, true  },	// 1 - 0001
	{ false, false, true,  false },	// 2 - 0010
	{ false, false, true,  true  },	// 3 - 0011
	{ false, true,  false, false },	// 4 - 0100
	{ false, true,  false, true  },	// 5 - 0101
	{ false, true,  true,  false },	// 6 - 0110
	{ false, true,  true,  true  },	// 7 - 0111
	{ true,  false, false, false },	// 8 - 1000
	{ true,  false, false, true  },	// 9 - 1001
	{ true,  false, true,  false },	// A - 1010
	{ true,  false, true,  true  },	// B - 1011
	{ true,  true,  false, false },	// C - 1100
	{ true,  true,  false, true  },	// D - 1101
	{ true,  true,  true,  false },	// E - 1110
	{ true,  true,  true,  true  },	// F - 1111
};

const uint16_t STIR::NEC_BUFFER[3] =
{
	9000,  // Mark 9ms 
	2250,  // Space 2.25ms
	560    // Burst
};

STIR::STIR(int irReceivePin, int irSendPin)
{
	STIR::STIR(PIN_NOT_SET, PIN_NOT_SET, PIN_NOT_SET, irReceivePin, irSendPin, 1);
}

STIR::STIR(uint8_t listeningLEDPin, uint8_t receivingLEDPin, uint8_t sendingLEDPin, uint8_t irReceivePin, uint8_t irSendPin, bool isMaster)
{
	role = isMaster ? ROLE_MASTER : ROLE_SLAVE;
	pinWaiting = listeningLEDPin;
	pinReceiving = receivingLEDPin;
	pinSending = sendingLEDPin;
	pinIRReceive = irReceivePin;
	pinIRSend = irSendPin;
	bufferSize = 0;
	//buffer = (uint8_t*)malloc(sizeof(uint8_t) * 1000);
	memset(buffer, 0, 100);
	memset(cipherKeyInt, 0, 28);
	serialStarted = false;
	irListenStarted = false;
	irTransmitStarted = false;
	setState(State::WAITING);
}

void STIR::beginListen()
{
	STIR::beginListenSerial();
	STIR::beginListenIR();
	freeBufferMessageFromIR();
	freeBufferMessageFromPC();
	//STIR::beginTransmitIR();
}

bool STIR::listenForCipherKey()
{
	//beginListenIR();
	beginListenSerial();
	Serial.println(F("Listening out for cipher key"));
	
	bool messageFinished1 = false;
	bool messageFinished2 = false;
	while (true)
	{
		//Serial.println("#");
		if (IrReceiver.decodeNEC())
		{
			//Serial.println("Got sth");
			if (IrReceiver.decodedIRData.address == role || IrReceiver.decodedIRData.address == UNKNOWN_ADDR) {
				IrReceiver.resume();
				continue;
			}

			if (state == State::WAITING)
			{
				Serial.println(F("Incoming message"));
				setState(State::RECEIVING);
			}

			if (bufferSize > 0 && IrReceiver.decodedIRData.command == bufferCipherKey[bufferSize-1])
			{
				continue;
			}

			if (!messageFinished1 &&
				IrReceiver.decodedIRData.command == FIN_SEQ1) {
				messageFinished1 = true;
			}
			else if (messageFinished1 && !messageFinished2 &&
				IrReceiver.decodedIRData.command == FIN_SEQ2) {
				messageFinished2 = true;
			}
			else if (messageFinished1 && messageFinished2 &&
				IrReceiver.decodedIRData.command == FIN_SEQ3) {
				/*IrReceiver.resume();
				endListenIR();*/
				bufferCipherKey[29] = 0;
				bufferCipherKey[30] = 0;
				bufferCipherKey[31] = 0;
				if (bufferSize != 31)
				{
					Serial.println(F("Synchronization error"));
				}
				bufferSize = 28;
				setState(State::WAITING);
				messageReceived = true;
				Serial.println(F("Cipher key received"));
				for (size_t i = 0; i < 3; i++)
				{
					sendAcknowledgedSequence();
					delay(1000);
				}
				//sendAcknowledgedSequence();
				//bufferSize = 0;
				buildCipherKey();
				
				for (size_t i = 0; i < 100; i++)
				{
					if (IrReceiver.decodeNEC())
					{
						IrReceiver.resume();
					}
					delay(10);
				}
				IrReceiver.resume();
				endListenIR();
				//endTransmitIR();
				freeBufferMessageFromIR();
				bufferSize = 0;
				return true;
			}
			else {
				messageFinished1 = false;
				messageFinished2 = false;
			}
			delay(100);
			/*Serial.print(F("#"));
			Serial.print(bufferSize+1);
			Serial.print(F(" "));
			IrReceiver.printIRResultShort(&Serial);*/
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
	Serial.print(F("Cipher key: "));
	for (size_t i = 0; i < 228; i++)
	{
		Serial.print(buffer[i] == true ? '1' : '0');
	}
	Serial.println();
	setState(State::SENDING);
	
	bool cipherKeyToSend[232];
	memcpy(cipherKeyToSend, buffer, 228);
	//Serial.println("Preparing to send cipher key");
	uint8_t* hexMessage = convertBinToCommandSequence(cipherKeyToSend, 232);
	Serial.println(F("Sending cipher key"));
	
	for (size_t i = 0; i < 232 / 8; i++)
	{
		IrSender.sendNEC(role, hexMessage[i], 0);
		delay(40);
		IrSender.sendRaw(NEC_BUFFER, 3, 38);
		delay(96);
	}
	sendFinishSequence();
	endTransmitIR();
	Serial.println(F("Sent cipher key, waiting for response"));
	beginListenIR();
	return waitForAcknowledgedSequence(false);
}

void STIR::communicationLoop()
{
	// check incoming messages from PC to forward to IR
	if (Serial.available() > 0)
	{
		uint8_t charCount = 0;
		while (Serial.available() > 0)
		{
			char inByte = Serial.read();
			if (inByte < 0)
			{
				continue;
			}
			static char message[10];
			static unsigned int message_pos = 0;
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
	}

	if (IrReceiver.decodeNEC())
	{
		if (IrReceiver.decodedIRData.address == role || IrReceiver.decodedIRData.address == UNKNOWN_ADDR) {
			IrReceiver.resume();
			return;
		}

		if (state == State::WAITING)
		{
			Serial.println(F("Incoming message"));
			setState(State::RECEIVING);
		}

		uint8_t loop = 0;
		uint8_t loop2 = 0;
		bool stop1 = false;
		bool stop2 = false;
		bufferSize = 0;

		while (true)
		{ 
			delay(50);
			IrReceiver.resume();
			if (IrReceiver.decodeNEC())
			{
				loop = 0;
				//IrReceiver.printIRResultShort(&Serial);
				if (IrReceiver.decodedIRData.address == role || IrReceiver.decodedIRData.address == UNKNOWN_ADDR ||
					IrReceiver.decodedIRData.address > 99) 
				{
					//Serial.println("unknown command");
					IrReceiver.resume();
					loop2++;
					if (loop2 == 40)
					{
						freeBufferMessageFromIR();
						break;
					}
					continue;
				}
				if (bufferSize != 0 && (uint8_t)IrReceiver.decodedIRData.command == buffer[bufferSize - 1])
				{
					//Serial.println("Same command received");
					IrReceiver.resume();
					continue;
				}
				if (!stop1 && IrReceiver.decodedIRData.command == FIN_SEQ1)
				{
					//Serial.println("FIN_SEQ1 received");
					stop1 = true;
				}
				else if (stop1 && !stop2 && IrReceiver.decodedIRData.command == FIN_SEQ2)
				{
					//Serial.println("FIN_SEQ2 received");
					stop2 = true;
				}
				else if (stop1 && stop2 && IrReceiver.decodedIRData.command == FIN_SEQ3)
				{
					Serial.println("Message received. Acknowledging..");
					//Serial.print(F("Buffer size: "));
					//Serial.println(bufferSize);
					bufferMessageFromIRSize = bufferSize;
					IrReceiver.resume();
					for (size_t i = 0; i < 10; i++)
					{
						sendAcknowledgedSequence();
						delay(300);
					}
					setState(State::WAITING);
					endListenIR();
					beginListenIR();
					return;
				}
				else 
				{
					if (stop2 && IrReceiver.decodedIRData.command == FIN_SEQ2)
					{
						continue;
					}
					if (stop1 && IrReceiver.decodedIRData.command == FIN_SEQ1)
					{
						continue;
					}
					//Serial.println(F("Received signal"));
					if (stop1)
					{
						buffer[bufferSize++] = FIN_SEQ1;
						//Serial.print(F("Received buffer #"));
						//Serial.println(bufferSize);
						stop1 = false;
					}
					if (stop2)
					{
						buffer[bufferSize++] = FIN_SEQ2;
						//Serial.print(F("Received buffer #"));
						//Serial.println(bufferSize);
						stop2 = false;
					}

					//Serial.print(F("Received buffer #"));
					//Serial.println(bufferSize);
					//Serial.print("Command: ");
					//Serial.println(IrReceiver.decodedIRData.command);
					buffer[bufferSize++] = IrReceiver.decodedIRData.command;
				}
			}
			else
			{
				loop++;
				if (loop == 10)
				{
					setState(State::WAITING);
					Serial.println(F("Message ended abruptly"));
					freeBufferMessageFromIR();
					break;
				}
			}
		}

		IrReceiver.resume();
	}
	else
	{
		setState(State::WAITING);
		//Serial.println("Message ended abruptly");
		//freeBufferMessageFromPC();
	}
	//delay(100);
}

void STIR::freeBufferMessageFromPC()
{
	bufferMessageFromPCSize = 0;
	memset(bufferMessageFromPC, 0, 150);
}

void STIR::freeBufferMessageFromIR()
{
	memset(bufferMessageFromIR, false, bufferMessageFromIRSize);
	memset(buffer, 0, 100);
	bufferMessageFromIRSize = 0;
}

void STIR::sendBinary(bool binaryMessage[])
{
	beginTransmitIR();
	uint16_t size = (bufferMessageFromPCSize - 1) * 8;
	Serial.print(F("Sending binary with size "));
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
	Serial.println(F("Sent message, waiting for acknowledgement"));
	waitForAcknowledgedSequence(true);
	freeBufferMessageFromPC();
}

void STIR::sendBinary(uint8_t encryptedMessage[])
{
	beginTransmitIR();
	uint8_t size = bufferMessageFromPCSize-1;
	//Serial.print(F("Sending encrypted message with size "));
	//Serial.print(size);

	for (size_t i = 0; i < size; i++)
	{
		IrSender.sendNEC(role, encryptedMessage[i], 0);
		delay(40);
		IrSender.sendRaw(NEC_BUFFER, 3, 38);
		delay(96);
	}
	sendFinishSequence();
	Serial.println(F("Sent message, waiting for acknowledgement"));
	waitForAcknowledgedSequence(true);
	freeBufferMessageFromPC();
}

bool STIR::waitForAcknowledgedSequence(bool isMessage)
{
	bool messageAcknowledged1 = false;
	bool messageAcknowledged2 = false;
	uint8_t loop = 0;
	uint8_t wait = isMessage ? 10 : 100;
	while (true)
	{
		if (IrReceiver.decodeNEC())
		{
			if (IrReceiver.decodedIRData.address == role || IrReceiver.decodedIRData.address == UNKNOWN_ADDR) {
				delay(wait);
				IrReceiver.resume();
				//Serial.println("Got unknown address");
				//messageAcknowledged1 = false;
				//messageAcknowledged2 = false;
				continue;
			}

			if (!messageAcknowledged1 &&
				IrReceiver.decodedIRData.command == ACK_SEQ1) 
			{
				messageAcknowledged1 = true;
				//Serial.println("Got messageAcknowledged1");
				delay(wait);
				IrReceiver.resume();
			}
			else if (messageAcknowledged1 && !messageAcknowledged2 &&
				IrReceiver.decodedIRData.command == ACK_SEQ2) 
			{
				messageAcknowledged2 = true;
				//Serial.println("Got messageAcknowledged2");
				IrReceiver.resume();
			}
			else if (messageAcknowledged1 && messageAcknowledged2 &&
				IrReceiver.decodedIRData.command == ACK_SEQ3) 
			{
				/*IrReceiver.resume();
				endListenIR();*/
				setState(State::WAITING);
				Serial.println(F("Message delivered"));
				
				for (size_t i = 0; i < 100; i++)
				{
					if (IrReceiver.decodeNEC())
					{
						IrReceiver.resume();
					}
					delay(10);
				}
				IrReceiver.resume();
				endListenIR();
				if (isMessage) 
				{
					delay(1000);
				}
				freeBufferMessageFromIR();
				beginListenIR();
				return true;
			}
			else 
			{
				/*messageAcknowledged1 = false;
				messageAcknowledged2 = false;*/
				/*Serial.print("Got ");
				Serial.println(IrReceiver.decodedIRData.command);*/
				loop++;
				if (loop == 5)
				{
					IrReceiver.resume();
					setState(State::WAITING);
					Serial.println(F("Message was not acknowledged by the other party"));
					endListenIR();
					beginListenIR();
					return false;
				}
				delay(1000);
				IrReceiver.resume();
			}
			//delay(100);
		}
		else
		{
			//Serial.println("Got nothing");
			loop++;
			if (loop == 50)
			{
				IrReceiver.resume();
				setState(State::WAITING);
				Serial.println(F("Message was not acknowledged by the other party"));
				endListenIR();
				beginListenIR();
				return false;
			}
			delay(wait);
			IrReceiver.resume();
		}
	}
	return false;
}

void STIR::sendFinishSequence()
{
	beginTransmitIR();
	IrSender.sendNEC(role, FIN_SEQ1, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
	IrSender.sendNEC(role, FIN_SEQ2, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
	IrSender.sendNEC(role, FIN_SEQ3, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
}

void STIR::sendAcknowledgedSequence()
{
	beginTransmitIR();
	IrSender.sendNEC(role, ACK_SEQ1, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
	IrSender.sendNEC(role, ACK_SEQ2, 0);
	delay(40);
	IrSender.sendRaw(NEC_BUFFER, 3, 38);
	delay(96);
	IrSender.sendNEC(role, ACK_SEQ3, 0);
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
	//char hexChar[2] = "\0";

	for (size_t i = 0; i < size; i++)
	{
		char* hexChar = (char*)malloc(sizeof(char) * 3);
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
	//Serial.print(F("Received key: "));
	for (size_t i = 0; i < 28; i++)
	{
		//Serial.print(bufferCipherKey[i]);
		cipherKeyInt[i] = bufferCipherKey[i];
	}
	//Serial.println();
}

void STIR::setState(State newState)
{
	if (state != newState)
	{
		state = newState;
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
	if (!irListenStarted)
	{
		IrReceiver.begin(pinIRReceive, ENABLE_LED_FEEDBACK);
		irListenStarted = true;
	}
}

void STIR::beginTransmitIR()
{
	if (!irTransmitStarted)
	{
		IrSender.begin(pinIRSend, ENABLE_LED_FEEDBACK);
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

void STIR::endTransmitIR()
{
	if (irTransmitStarted)
	{
		IrSender.IRLedOff();
	}
}

#endif