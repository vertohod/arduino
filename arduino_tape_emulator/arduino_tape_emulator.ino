#include "block_handler.h"
#include "file_reader.h"
#include "timer.h"

#define OUTPUTPIN 4
#define BUFFER_SIZE 16
#define DURATION_PAUSE 1.0 // seconds

block_handler *bh = nullptr;
timer<1> *timer1 = nullptr;
timer<2> *timer2 = nullptr;
file_reader *reader = nullptr;

byte* buffer = new byte[BUFFER_SIZE];
byte next_level = 0;
double  next_period = 0.0;
double next_duration = 0.0;

void start_reading()
{
    auto length = reader->get_data(buffer, BUFFER_SIZE);
    if (length > 0) {
        buffer = bh->fill_buffer(buffer, length);
        bh->start(reader->get_block_type());

        next_level = bh->get_level();
        next_period = bh->get_period();
        next_duration = bh->get_duration();

        timer2->set_min();
    }
}

void setup()
{
    // FIXME
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    reader = new file_reader("tape.tap");
    bh = new block_handler(BUFFER_SIZE);

    cli();
    DDRD = B00010000;
    timer1 = new timer<1>();
    timer2 = new timer<2>();
    start_reading();
    sei();

    digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
    if (reader->is_pause() == 1) return;

    if (bh->is_buffer_empty() == 1) {
        auto length = reader->get_data(buffer, BUFFER_SIZE);
        if (length > 0) {
            buffer = bh->fill_buffer(buffer, length);
        }
    }
}

template<>
void timer<1>::handler()
{
    Serial.println("Timer 1 is working");

    if (bh->is_pilot() == 1) {
        Serial.println("Timer 1, pilot turn-off");
        bh->switch_next();
    }
    if (bh->is_finished() == 1 && reader->is_pause() == 1) {
        Serial.println("Timer 1, continue to read");
        reader->read_continue();
        start_reading();
    }
}

template<>
void timer<2>::handler()
{
    PORTD = next_level ? 0xff : 0;

    if (0.0 != next_period) {
        timer2->set(next_period);
    } else {
        if (bh->is_finished() == 1) {
            if (reader->is_pause() == 1) {
                timer1->set(DURATION_PAUSE);
            }
            return;
        }
    }
    if (0.0 != next_duration) {
        timer1->set(next_duration);
    }
    next_level = bh->get_level();
    next_period = bh->get_period();
    next_duration = bh->get_duration();
}
