#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>

#define TIMER_FRQ_Mhz 16.0

template <int>
class timer
{
private:
    byte m_TCCRnB;
    bool m_is_working;

public:
    timer();
    static void handler() {}
    static void f()
    {
        timer::handler();
    } 
    void set(double duration) {}
    void set(byte duration) {}
    void set_min()
    {
        set(0.001024);
    }
    void stop();
};

// --------------- Timer 1 ----------------
template<>
timer<1>::timer() : m_is_working(false)
{
    m_TCCRnB = 1 << WGM12;

    TCCR1A = 0;
    TCCR1B = m_TCCRnB;
    TIMSK1 = 1 << OCIE1A;
}
template<>
void timer<1>::set(double duration)
{
    // duration from 0.000128 to 8.388608
    OCR1A = static_cast<size_t>(duration * 1000000 * TIMER_FRQ_Mhz / (2 * 1024) - 1);

    if (!m_is_working) {
        TCCR1B = m_TCCRnB | 1 << CS12 | 1 << CS10;
        m_is_working = true;
    }
}
template<>
void timer<1>::stop()
{
    // Turn-off timer
    TCCR1B = 1 << WGM12;
    m_is_working = false;
}

// --------------- Timer 2 ----------------
template<>
timer<2>::timer() : m_is_working(false)
{
    m_TCCRnB = 1 << WGM22;

    TCCR2A = 0;
    TCCR2B = m_TCCRnB;
    TIMSK2 = 1 << OCIE2A;
}
template<>
void timer<2>::set(double duration)
{
    // duration from 0.000004 to 0.001024
    OCR2A = static_cast<size_t>(duration * 1000000 * TIMER_FRQ_Mhz / (2 * 32) - 1);

    if (!m_is_working) {
        TCCR2B = m_TCCRnB | 1 << CS21 | 1 << CS20;
        m_is_working = true;
    }
}
template<>
void timer<2>::stop()
{
    // Turn-off timer
    TCCR2B = 1 << WGM22;
    m_is_working = false;
}

// --------------- Timer 2.2 ----------------
template<>
timer<22>::timer() : m_is_working(false)
{
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    TIMSK2 = 1 << TOIE2;
}
template<>
void timer<22>::set(byte duration)
{
    TCNT2 = 0xff - duration;

    if (!m_is_working) {
        TCCR2B = 1 << CS22;
        m_is_working = true;
    }
}
template<>
void timer<22>::set_min()
{
    set(static_cast<byte>(128));
}
template<>
void timer<22>::stop()
{
    // Turn-off timer
    TCCR2B = 0;
    m_is_working = false;
}

// -------------- Handlers ----------------

ISR(TIMER1_COMPA_vect)
{
    cli();
    timer<1>::f();
    sei();
}

ISR(TIMER2_COMPA_vect)
{
    cli();
    timer<2>::f();
    sei();
}

ISR(TIMER2_OVF_vect)
{
    cli();
    timer<22>::f();
    sei();
}

#endif
