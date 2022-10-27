#pragma once
#ifndef IRremote_h
#define IRremote_h

typedef
enum {
	UNKNOWN = -1,
	UNUSED = 0,
	RC5,
	RC6,
	NEC,
	SONY,
	PANASONIC,
	JVC,
	SAMSUNG,
	WHYNTER,
	AIWA_RC_T501,
	LG,
	SANYO,
	MITSUBISHI,
	DISH,
	SHARP,
	DENON,
	PRONTO,
}
decode_type_t;

class decode_results
{
public:
	decode_type_t          decode_type;  // UNKNOWN, NEC, SONY, RC5, ...
	unsigned int           address;      // Used by Panasonic & Sharp [16-bits]
	unsigned long          value;        // Decoded value [max 32-bits]
	int                    bits;         // Number of bits in decoded value
	volatile unsigned int* rawbuf;      // Raw intervals in 50uS ticks
	int                    rawlen;       // Number of records in rawbuf
	int                    overflow;     // true iff IR raw code too long
};

#define IRPRONTO
#define SEND_PRONTO true

class IRrecv
{
public:
	IRrecv(int recvpin);
	IRrecv(int recvpin, int blinkpin);

	void  blink13(int blinkflag);
	int   decode(decode_results* results);
	void  enableIRIn();
	bool  isIdle();
	void  resume();

private:
	long  decodeHash(decode_results* results);
	int   compare(unsigned int oldval, unsigned int newval);

	//......................................................................
#		if (DECODE_RC5 || DECODE_RC6)
			// This helper function is shared by RC5 and RC6
	int  getRClevel(decode_results* results, int* offset, int* used, int t1);
#		endif
#		if DECODE_RC5
	bool  decodeRC5(decode_results* results);
#		endif
#		if DECODE_RC6
	bool  decodeRC6(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_NEC
	bool  decodeNEC(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_SONY
	bool  decodeSony(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_PANASONIC
	bool  decodePanasonic(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_JVC
	bool  decodeJVC(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_SAMSUNG
	bool  decodeSAMSUNG(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_WHYNTER
	bool  decodeWhynter(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_AIWA_RC_T501
	bool  decodeAiwaRCT501(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_LG
	bool  decodeLG(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_SANYO
	bool  decodeSanyo(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_MITSUBISHI
	bool  decodeMitsubishi(decode_results* results);
#		endif
	//......................................................................
#		if DECODE_DISH
	bool  decodeDish(decode_results* results); // NOT WRITTEN
#		endif
		//......................................................................
#		if DECODE_SHARP
	bool  decodeSharp(decode_results* results); // NOT WRITTEN
#		endif
		//......................................................................
#		if DECODE_DENON
	bool  decodeDenon(decode_results* results);
#		endif
};

class IRsend
{
public:
	IRsend();

	void  custom_delay_usec(unsigned long uSecs);
	void  enableIROut(int khz);
	void  mark(unsigned int usec);
	void  space(unsigned int usec);
	void  sendRaw(const unsigned int buf[], unsigned int len, unsigned int hz);

	//......................................................................
#		if SEND_RC5
	void  sendRC5(unsigned long data, int nbits);
#		endif
#		if SEND_RC6
	void  sendRC6(unsigned long data, int nbits);
#		endif
	//......................................................................
#		if SEND_NEC
	void  sendNEC(unsigned long data, int nbits);
#		endif
	//......................................................................
#		if SEND_SONY
	void  sendSony(unsigned long data, int nbits);
#		endif
	//......................................................................
#		if SEND_PANASONIC
	void  sendPanasonic(unsigned int address, unsigned long data);
#		endif
	//......................................................................
#		if SEND_JVC
			// JVC does NOT repeat by sending a separate code (like NEC does).
			// The JVC protocol repeats by skipping the header.
			// To send a JVC repeat signal, send the original code value
			//   and set 'repeat' to true
	void  sendJVC(unsigned long data, int nbits, bool repeat);
#		endif
	//......................................................................
#		if SEND_SAMSUNG
	void  sendSAMSUNG(unsigned long data, int nbits);
#		endif
	//......................................................................
#		if SEND_WHYNTER
	void  sendWhynter(unsigned long data, int nbits);
#		endif
	//......................................................................
#		if SEND_AIWA_RC_T501
	void  sendAiwaRCT501(int code);
#		endif
	//......................................................................
#		if SEND_LG
	void  sendLG(unsigned long data, int nbits);
#		endif
	//......................................................................
#		if SEND_SANYO
	void  sendSanyo(); // NOT WRITTEN
#		endif
		//......................................................................
#		if SEND_MISUBISHI
	void  sendMitsubishi(); // NOT WRITTEN
#		endif
		//......................................................................
#		if SEND_DISH
	void  sendDISH(unsigned long data, int nbits);
#		endif
	//......................................................................
#		if SEND_SHARP
	void  sendSharpRaw(unsigned long data, int nbits);
	void  sendSharp(unsigned int address, unsigned int command);
#		endif
	//......................................................................
#		if SEND_DENON
	void  sendDenon(unsigned long data, int nbits);
#		endif
	//......................................................................
#		if SEND_PRONTO
	void  sendPronto(char* code, bool repeat, bool fallback);
#		endif
};

#endif