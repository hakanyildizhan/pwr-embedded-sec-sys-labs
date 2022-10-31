#pragma once
#ifndef Arduino_h
#include <Arduino.h>
#endif
/**
 * Contains pin number and enable status of the feedback LED
 */
struct FeedbackLEDControlStruct {
    uint8_t FeedbackLEDPin;         ///< if 0, then take board specific FEEDBACK_LED_ON() and FEEDBACK_LED_OFF() functions
    uint8_t LedFeedbackEnabled; ///< LED_FEEDBACK_ENABLED_FOR_RECEIVE or LED_FEEDBACK_ENABLED_FOR_SEND -> enable blinking of pin on IR processing
};

struct FeedbackLEDControlStruct FeedbackLEDControl; ///< The feedback LED control instance