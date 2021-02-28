#include <avr/io.h>
#include <avr/interrupt.h>
#include <SPI.h>
#include <SD.h>
#include "block_handler.h"
#include "file_reader.h"

#define BUFFER_SIZE     16

#define SDPIN 8

void setup() {
    // begin of setup
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    if (!SD.begin(SDPIN)) {
        while (1);
    }

    // tun timer 1
    cli();
    TCCR1A = 0;
    TCCR1B = 1 << WGM12 | 1 << CS10;
    TIMSK1 = 1 << OCIE1A;
    sei();

    // tun timer 2
    cli();
    TCCR2A = 0;
    TCCR2B = 1 << WGM22 | 1 << CS20;
    TIMSK2 = 1 << OCIE2A;
    sei();

    // end of setup
    digitalWrite(LED_BUILTIN, LOW);
}

block_handler handler(BUFFER_SIZE);

byte** init_buffer()
{
    byte** result;
    result = new byte*;
    *result = new byte[BUFFER_SIZE];

    return result;
}

void loop() {
    static byte** buffer = init_buffer();
    static file_reader reader("tape.tap");

    if (!handler.is_finished() && handler.is_buffer_empty()) {
        auto length = reader.get_data(*buffer, BUFFER_SIZE);
        handler.fill_buffer(buffer, length);
    }
}

#define OUTPUTPIN 4

ISR(TIMER1_COMPA_vect)
{
    if (handler.is_finished()) return;

    auto level = handler.get_level();
    auto period = handler.get_period();
    auto duration = handler.get_duration();

    digitalWrite(OUTPUTPIN, level ? HIGH : LOW);
    OCR1A = period;
    OCR2A = duration;
}

ISR(TIMER2_COMPA_vect)
{
}
