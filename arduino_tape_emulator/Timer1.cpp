#include "Timer1.h"

Timer1::Timer1() : mPrescaling(0), mDuration(0.0) {}

void Timer1::start(float duration) {
    mDuration = duration;

    if (duration < STEP1) {
        OCR1A = static_cast<uint16_t>(duration * TIMER_FRQ_Hz / (2 * 1) - 1);
        mPrescaling = 1 << CS10;
    } else if (duration < STEP2) {
        OCR1A = static_cast<uint16_t>(duration * TIMER_FRQ_Hz / (2 * 8) - 1);
        mPrescaling = 1 << CS11;
    } else if (duration < STEP3) {
        OCR1A = static_cast<uint16_t>(duration * TIMER_FRQ_Hz / (2 * 64) - 1);
        mPrescaling = 1 << CS11 | 1 << CS10;
    } else if (duration < STEP4) {
        OCR1A = static_cast<uint16_t>(duration * TIMER_FRQ_Hz / (2 * 256) - 1);
        mPrescaling = 1 << CS12;
    } else if (duration < STEP5) {
        OCR1A = static_cast<uint16_t>(duration * TIMER_FRQ_Hz / (2 * 1024) - 1);
        mPrescaling = 1 << CS12 | 1 << CS10;
    } else {
        mDuration = 0.0;
    }

    TIMSK1 = 1 << OCIE1A;
    TCCR1A = 0;
    TCCR1B = 1 << WGM12 | mPrescaling;
}

float Timer1::duration() {
    return mDuration;
}

void Timer1::stop() {
    TCCR1B = 0;
}
