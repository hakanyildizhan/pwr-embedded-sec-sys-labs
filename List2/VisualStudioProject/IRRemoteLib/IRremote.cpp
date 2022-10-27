#include "pch.h"
#include "IRremote.h"

IRrecv::IRrecv(int recvpin) {}
IRrecv::IRrecv(int recvpin, int blinkpin) {}

void  IRrecv::blink13(int blinkflag) {}
int   IRrecv::decode(decode_results* results) { return 0; }
void  IRrecv::enableIRIn() {}
bool  IRrecv::isIdle() { return 0; }
void  IRrecv::resume() {}

long  IRrecv::decodeHash(decode_results* results) { return 0; }
int   IRrecv::compare(unsigned int oldval, unsigned int newval) { return 0; }

//......................................................................
#		if (DECODE_RC5 || DECODE_RC6)
			// This helper function is shared by RC5 and RC6
int  IRrecv::getRClevel(decode_results* results, int* offset, int* used, int t1) { return 0; }
#		endif
#		if DECODE_RC5
bool  IRrecv::decodeRC5(decode_results* results) { return 0; }
#		endif
#		if DECODE_RC6
bool  IRrecv::decodeRC6(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_NEC
bool  IRrecv::decodeNEC(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_SONY
bool  IRrecv::decodeSony(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_PANASONIC
bool  IRrecv::decodePanasonic(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_JVC
bool  IRrecv::decodeJVC(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_SAMSUNG
bool  IRrecv::decodeSAMSUNG(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_WHYNTER
bool  IRrecv::decodeWhynter(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_AIWA_RC_T501
bool  IRrecv::decodeAiwaRCT501(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_LG
bool  IRrecv::decodeLG(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_SANYO
bool  IRrecv::decodeSanyo(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_MITSUBISHI
bool  IRrecv::decodeMitsubishi(decode_results* results) { return 0; }
#		endif
//......................................................................
#		if DECODE_DISH
bool  IRrecv::decodeDish(decode_results* results) { return 0; } // NOT WRITTEN
#		endif
		//......................................................................
#		if DECODE_SHARP
bool  IRrecv::decodeSharp(decode_results* results) { return 0; } // NOT WRITTEN
#		endif
		//......................................................................
#		if DECODE_DENON
bool  IRrecv::decodeDenon(decode_results* results) { return 0; }
#		endif


IRsend::IRsend() { }

void  IRsend::custom_delay_usec(unsigned long uSecs) { }
void  IRsend::enableIROut(int khz) { }
void  IRsend::mark(unsigned int usec) { }
void  IRsend::space(unsigned int usec) { }
void  IRsend::sendRaw(const unsigned int buf[], unsigned int len, unsigned int hz) { }

//......................................................................
#		if SEND_RC5
void  IRsend::sendRC5(unsigned long data, int nbits) { }
#		endif
#		if SEND_RC6
void  IRsend::sendRC6(unsigned long data, int nbits) { }
#		endif
//......................................................................
#		if SEND_NEC
void  IRsend::sendNEC(unsigned long data, int nbits) { }
#		endif
//......................................................................
#		if SEND_SONY
void  IRsend::sendSony(unsigned long data, int nbits) { }
#		endif
//......................................................................
#		if SEND_PANASONIC
void  IRsend::sendPanasonic(unsigned int address, unsigned long data) { }
#		endif
//......................................................................
#		if SEND_JVC
			// JVC does NOT repeat by sending a separate code (like NEC does).
			// The JVC protocol repeats by skipping the header.
			// To send a JVC repeat signal, send the original code value
			//   and set 'repeat' to true
void  IRsend::sendJVC(unsigned long data, int nbits, bool repeat) { }
#		endif
//......................................................................
#		if SEND_SAMSUNG
void  IRsend::sendSAMSUNG(unsigned long data, int nbits) { }
#		endif
//......................................................................
#		if SEND_WHYNTER
void  IRsend::sendWhynter(unsigned long data, int nbits) { }
#		endif
//......................................................................
#		if SEND_AIWA_RC_T501
void  IRsend::sendAiwaRCT501(int code) { }
#		endif
//......................................................................
#		if SEND_LG
void  IRsend::sendLG(unsigned long data, int nbits) { }
#		endif
//......................................................................
#		if SEND_SANYO
void  IRsend::sendSanyo() { } // NOT WRITTEN
#		endif
		//......................................................................
#		if SEND_MISUBISHI
void  IRsend::sendMitsubishi() { } // NOT WRITTEN
#		endif
		//......................................................................
#		if SEND_DISH
void  IRsend::sendDISH(unsigned long data, int nbits) { }
#		endif
//......................................................................
#		if SEND_SHARP
void  IRsend::sendSharpRaw(unsigned long data, int nbits);
void  IRsend::sendSharp(unsigned int address, unsigned int command) { }
#		endif
//......................................................................
#		if SEND_DENON
void  IRsend::sendDenon(unsigned long data, int nbits) { }
#		endif
//......................................................................
#		if SEND_PRONTO
void  IRsend::sendPronto(char* code, bool repeat, bool fallback) { }
#		endif