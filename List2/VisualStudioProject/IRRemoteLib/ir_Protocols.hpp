#include <Arduino.h>
#include "IRremoteInt.h"

//#ifndef IRremote_h
//#include "IRremote.hpp"
//#endif


void IRsend::sendBoseWave(uint8_t aCommand, uint_fast8_t aNumberOfRepeats)
{

}
void IRsend::sendDenon(uint8_t aAddress, uint8_t aCommand, uint_fast8_t aNumberOfRepeats, bool aSendSharp){}
void IRsend::sendDenonRaw(uint16_t aRawData, uint_fast8_t aNumberOfRepeats){}

void IRsend::sendJVC(uint8_t aAddress, uint8_t aCommand, uint_fast8_t aNumberOfRepeats){}

void IRsend::sendLGRepeat(bool aUseLG2Protocol){}
void IRsend::sendLG(uint8_t aAddress, uint16_t aCommand, uint_fast8_t aNumberOfRepeats, bool aSendOnlySpecialLGRepeat, bool aUseLG2Protocol){}
void IRsend::sendLG2(uint8_t aAddress, uint16_t aCommand, uint_fast8_t aNumberOfRepeats, bool aSendOnlySpecialLGRepeat){}
void IRsend::sendLGRaw(uint32_t aRawData, uint_fast8_t aNumberOfRepeats, bool aSendOnlySpecialLGRepeat, bool aUseLG2Protocol){}

void IRsend::sendNECRepeat(){}
void IRsend::sendNEC(uint16_t aAddress, uint8_t aCommand, uint_fast8_t aNumberOfRepeats, bool aSendOnlySpecialNECRepeat){}
void IRsend::sendNEC2(uint16_t aAddress, uint8_t aCommand, uint_fast8_t aNumberOfRepeats){}
void IRsend::sendNECRaw(uint32_t aRawData, uint_fast8_t aNumberOfRepeats, bool aSendOnlySpecialNECRepeat){}
// NEC variants
void IRsend::sendOnkyo(uint16_t aAddress, uint16_t aCommand, uint_fast8_t aNumberOfRepeats, bool aSendOnlySpecialNECRepeat){}
void IRsend::sendApple(uint8_t aAddress, uint8_t aCommand, uint_fast8_t aNumberOfRepeats, bool aSendOnlySpecialNECRepeat) {}

void IRsend::sendKaseikyo(uint16_t aAddress, uint8_t aData, uint_fast8_t aNumberOfRepeats, uint16_t aVendorCode) {} // LSB first
void IRsend::sendPanasonic(uint16_t aAddress, uint8_t aData, uint_fast8_t aNumberOfRepeats) {} // LSB first
void IRsend::sendKaseikyo_Denon(uint16_t aAddress, uint8_t aData, uint_fast8_t aNumberOfRepeats) {} // LSB first
void IRsend::sendKaseikyo_Mitsubishi(uint16_t aAddress, uint8_t aData, uint_fast8_t aNumberOfRepeats) {} // LSB first
void IRsend::sendKaseikyo_Sharp(uint16_t aAddress, uint8_t aData, uint_fast8_t aNumberOfRepeats) {} // LSB first
void IRsend::sendKaseikyo_JVC(uint16_t aAddress, uint8_t aData, uint_fast8_t aNumberOfRepeats) {} // LSB first

void IRsend::sendRC5(uint8_t aAddress, uint8_t aCommand, uint_fast8_t aNumberOfRepeats, bool aEnableAutomaticToggle) {}
void IRsend::sendRC6(uint8_t aAddress, uint8_t aCommand, uint_fast8_t aNumberOfRepeats, bool aEnableAutomaticToggle) {}
void IRsend::sendSamsungLGRepeat() {}
void IRsend::sendSamsung(uint16_t aAddress, uint16_t aCommand, uint_fast8_t aNumberOfRepeats) {}
void IRsend::sendSamsungLG(uint16_t aAddress, uint16_t aCommand, uint_fast8_t aNumberOfRepeats, bool aSendOnlySpecialSamsungRepeat) {}
void IRsend::sendSharp(uint8_t aAddress, uint8_t aCommand, uint_fast8_t aNumberOfRepeats) {} // redirected to sendDenon
void IRsend::sendSony(uint16_t aAddress, uint8_t aCommand, uint_fast8_t aNumberOfRepeats, uint8_t numberOfBits) {}

void IRsend::sendLegoPowerFunctions(uint8_t aChannel, uint8_t tCommand, uint8_t aMode, bool aDoSend5Times) {}
void IRsend::sendLegoPowerFunctions(uint16_t aRawData, bool aDoSend5Times) {}
void IRsend::sendLegoPowerFunctions(uint16_t aRawData, uint8_t aChannel, bool aDoSend5Times) {}

void IRsend::sendMagiQuest(uint32_t wand_id, uint16_t magnitude) {}

void IRsend::sendPronto(const __FlashStringHelper* str, uint_fast8_t aNumberOfRepeats) {}
void IRsend::sendPronto(const char* prontoHexString, uint_fast8_t aNumberOfRepeats) {}
void IRsend::sendPronto(const uint16_t* data, unsigned int length, uint_fast8_t aNumberOfRepeats) {}
void IRsend::sendNECMSB(uint32_t data, uint8_t nbits, bool repeat) {}
void IRsend::sendRC5(uint32_t data, uint8_t nbits) {}
void IRsend::sendPanasonic(uint16_t address, uint32_t data) {}

bool IRrecv::decodeNEC() { return true; };
bool IRrecv::decodeNECMSB(decode_results* results) { return true; }
bool IRrecv::decodeRC5() { return true; }
bool IRrecv::decodePanasonicMSB(decode_results* aResults) { return true; }

void setFeedbackLED(bool aSwitchLedOn) {}
void setLEDFeedback(uint8_t aFeedbackLEDPin, uint8_t aEnableLEDFeedback) {}