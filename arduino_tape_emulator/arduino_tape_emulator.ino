#include "block_handler.h"
#include "file_reader.h"
#include "timer.h"

#define OUTPUTPIN 4
#define BUFFER_SIZE 16
#define DURATION_PAUSE 2.0 // seconds

block_handler *bh = nullptr;
timer<22> *timer2 = nullptr;
timer<1> *timer1 = nullptr;
file_reader *reader = nullptr;

byte* buffer = new byte[BUFFER_SIZE];
bool next_level_up = false;
double  next_period = 0.0;
byte next_period_byte = 0;
double next_duration = 0.0;

void start_reading()
{
    auto length = reader->get_data(buffer, BUFFER_SIZE);
    if (length > 0) {
        buffer = bh->fill_buffer(buffer, length);
        bh->start(reader->get_block_type());

        next_level_up = bh->get_level();
        next_period = bh->get_period();
        next_period_byte = bh->get_period_byte();
        next_duration = bh->get_duration();

        timer2->set_min();
    }
}

void setup()
{
//    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    reader = new file_reader("tape.tap");
    bh = new block_handler(BUFFER_SIZE);

    DDRD = B00010000;
    timer2 = new timer<22>();
    timer1 = new timer<1>();
    start_reading();

    digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
    if (reader->is_pause()) return;

    if (bh->is_buffer_empty()) {
        auto length = reader->get_data(buffer, BUFFER_SIZE);
        if (length > 0) {
            buffer = bh->fill_buffer(buffer, length);
        }
    }
}

template<>
void timer<22>::handler()
{
    PORTD = next_level_up ? 0xff : 0x00;

    if (0 != next_period_byte) {
        timer2->set(next_period_byte);
    } else {
        if (bh->is_finished()) {
            if (reader->is_pause()) {
                timer1->set(DURATION_PAUSE);
            }
            timer2->stop();
            return;
        }
    }
    if (0.0 != next_duration) {
        timer1->set(next_duration);
    }
    next_level_up = bh->get_level();
    next_period_byte = bh->get_period_byte();
    next_duration = bh->get_duration();
}

template<>
void timer<1>::handler()
{
    if (bh->is_pilot()) {
        bh->switch_next();
    }
    if (bh->is_finished() && reader->is_pause()) {
        reader->read_continue();
        start_reading();
    }
    timer1->stop();
}

template<>
void timer<2>::handler()
{
    PORTD = next_level_up ? 0xff : 0x00;

    if (0.0 != next_period) {
        timer2->set(next_period);
    } else {
        if (bh->is_finished()) {
            if (reader->is_pause()) {
                timer1->set(DURATION_PAUSE);
            }
            timer2->stop();
            return;
        }
    }
    if (0.0 != next_duration) {
        timer1->set(next_duration);
    }
    next_level_up = bh->get_level();
    next_period = bh->get_period();
    next_duration = bh->get_duration();
}
