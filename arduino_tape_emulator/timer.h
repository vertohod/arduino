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

public:
    timer();

    static void handler() {}
    static void f()
    {
        timer::handler();
    } 

    void set(double duration)
    {
    }
    void set_min()
    {
        set(0.001024);
    }
    static void stop()
    {
    }
};

template<>
timer<1>::timer()
{
    m_TCCRnB = 1 << WGM12;

    cli();
    TCCR1A = 0;
    TCCR1B = m_TCCRnB;
    TIMSK1 = 1 << OCIE1A;
    sei();
}
template<>
void timer<1>::set(double duration)
{
    // duration from 0.000128 to 8.388608
    OCR1A = static_cast<size_t>(duration * 1000000 * TIMER_FRQ_Mhz / (2 * 1024) - 1);
    TCCR1B = m_TCCRnB | 1 << CS12 | 1 << CS10;
}
template<>
void timer<1>::stop()
{
    // Turn-off timer
    TCCR1B = 1 << WGM12;
}

template<>
timer<2>::timer()
{
    m_TCCRnB = 1 << WGM22;

    cli();
    TCCR2A = 0;
    TCCR2B = m_TCCRnB;
    TIMSK2 = 1 << OCIE2A;
    sei();
}
template<>
void timer<2>::set(double duration)
{
    // duration from 0.000004 to 0.001024
    OCR2A = static_cast<size_t>(duration * 1000000 * TIMER_FRQ_Mhz / (2 * 32) - 1);
    TCCR2B = m_TCCRnB | 1 << CS21 | 1 << CS20;
}
template<>
void timer<2>::stop()
{
    // Turn-off timer
    TCCR2B = 1 << WGM22;
}

ISR(TIMER1_COMPA_vect)
{
    cli();
    timer<1>::stop();
    timer<1>::f();
    sei();
}

ISR(TIMER2_COMPA_vect)
{
    cli();
    timer<2>::stop();
    timer<2>::f();
    sei();
}

#endif
