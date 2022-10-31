#pragma once
#define TIMER_RESET_INTR_PENDING
#define TIMER_ENABLE_RECEIVE_INTR 1
#define TIMER_DISABLE_RECEIVE_INTR 0
void timerConfigForReceive() {}
//void ISR() {}
//void ISR(String name) {}
#define ISR(f) void (f)(void)
#define ISR() void IRTimerInterruptHandler()