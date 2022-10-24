#pragma once
#include <STIR.h>
#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#include <IRremote.hpp>
#define NUMBER_OF_REPEATS 3U

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
		IrReceiver.begin(pinIRReceive, ENABLE_LED_FEEDBACK);
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

String STIR::receive()
{
	// TODO
}

IRsend irsend;

void STIR::send(char binaryMessage[])
{
	char hexMsg[sizeof(binaryMessage)/4];
	hextobin(binaryMessage).copy(hexMsg, sizeof(binaryMessage) / 4);
	IrSender.begin(pinIRSend);
	irsend.sendPronto(hexMsg, NUMBER_OF_REPEATS);
}

string hextobin(const char s[]) {
	string out;
	for (int i = 0; i < sizeof(s); i++) {
		byte n;
		if (s[i] <= '9' && s[i] >= '0')
			n = s[i] - '0';
		else
			n = 10 + s[i] - 'A';
		for (byte j = 3; j >= 0; --j)
			out.push_back((n & (1 << j)) ? '1' : '0');
	}

	return out;
}

string bintohex(const string& s) {
	string out;
	for (int i = 0; i < s.size(); i += 4) {
		int n = 0;
		for (int j = i; j < i + 4; ++j) {
			n <<= 1;
			if (s[j] == '1')
				n |= 1;
		}

		if (n <= 9)
			out.push_back('0' + n);
		else
			out.push_back('A' + n - 10);
	}

	return out;
}