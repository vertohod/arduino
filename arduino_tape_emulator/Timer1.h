#ifndef TIMER1_H
#define TIMER1_H

#include <avr/interrupt.h>
#include <avr/io.h>

#include "Types.h"

#define TIMER_FRQ_Hz 16000000.0
#define MAX_OCR1A 65535.0
#define STEP1 (((MAX_OCR1A + 1) * 2 * 1) / TIMER_FRQ_Hz)
#define STEP2 (((MAX_OCR1A + 1) * 2 * 8) / TIMER_FRQ_Hz)
#define STEP3 (((MAX_OCR1A + 1) * 2 * 64) / TIMER_FRQ_Hz)
#define STEP4 (((MAX_OCR1A + 1) * 2 * 256) / TIMER_FRQ_Hz)
#define STEP5 (((MAX_OCR1A + 1) * 2 * 1024) / TIMER_FRQ_Hz)

class Timer1 {
private:
    byte    mPrescaling;
    float   mDuration;

private:
    Timer1();

public:
    static Timer1& instance() {
        static Timer1 object;
        return object;
    }
    Timer1(Timer1 const&) = delete;
    void operator=(Timer1 const&) = delete;

    void start(float duration);
    float duration();
    void stop();
};

#endif
