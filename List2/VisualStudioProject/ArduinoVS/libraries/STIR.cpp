#pragma once
#ifndef STIR_h
#include <STIR.h>
#endif
//#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#define NUMBER_OF_REPEATS 3U

//IRrecv irrecv(2);
//decode_results results;
//IRsend irsend;

IRrecv irrecv(2);
IRsend irsend;

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
}

STIR::STIR(STIRConfig config)
{
	pinWaiting = config.listeningLEDPin;
	pinReceiving = config.receivingLEDPin;
	pinSending = config.sendingLEDPin;
	pinIRReceive = config.irReceivePin;
	onReceive = config.onMessageReceived;
	
	if (config.onMessageReceived != doNothing 
		&& config.incomingMessageProcessing == ProcessIncoming::WRITETOSERIAL)
	{
		onReceive = &writeToSerial;
	}

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
		sendString(Serial.readString());
	}

	// check incoming messages from IR to forward to PC
	if (irrecv.decode(&results))
	{
		unsigned long res = results.value;
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
		//IrReceiver.begin(pinIRReceive, ENABLE_LED_FEEDBACK);
		irrecv.enableIRIn();
		//IRrecv irrecv(2);
		irrecv = IRrecv(pinIRReceive);
		//IrSender.setSendPin(pinIRSend);
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
		//IrReceiver.end();
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
	irsend.sendPronto(messageArr, false, false);
}

void STIR::sendBinary(bool binaryMessage[])
{
	char* hexMessage = convertBinToHexString(binaryMessage);
	irsend.sendPronto(hexMessage, false, false);
}

char* STIR::convertBinToHexString(bool bin[])
{
	char message[1000];
	for (int i = 0; i < 1000; i++)
	{
		if (bin[i])
		{
			message[i] = '1';
		}
		else
		{
			message[i] = '0';
		}
	}
	return message;
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