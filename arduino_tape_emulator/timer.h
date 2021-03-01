#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>

template <int>
class timer
{
public:
    timer();

    static void handler() {}
    static void f()
    {
        timer::handler();
    } 

    void set(size_t duration)
    {
        // TODO
    }
    void set_min()
    {
        // TODO
    }
    void stop()
    {
        // TODO
    }
};

template<>
timer<1>::timer()
{
    cli();
    TCCR1A = 0;
    TCCR1B = 1 << WGM12 | 1 << CS10;
    TIMSK1 = 1 << OCIE1A;
    sei();
}

template<>
timer<2>::timer()
{
    cli();
    TCCR2A = 0;
    TCCR2B = 1 << WGM22 | 1 << CS20;
    TIMSK2 = 1 << OCIE2A;
    sei();
}

ISR(TIMER1_COMPA_vect)
{
    timer<1>::f();
}

ISR(TIMER2_COMPA_vect)
{
    timer<2>::f();
}

#endif
