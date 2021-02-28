#include "block_handler.h"
#include "file_reader.h"
#include "timer.h"

#define BUFFER_SIZE 16

block_handler *bh = nullptr;
timer<1> *timer1 = nullptr;
timer<2> *timer2 = nullptr;
file_reader *reader = nullptr;

byte** buffer = block_handler::init_buffer(BUFFER_SIZE);
bool next_level = false;
bool next_period = 0;
bool next_duration = 0;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    reader = new file_reader("tape.tap");
    bh = new block_handler(BUFFER_SIZE);
    timer1 = new timer<1>();
    timer2 = new timer<2>();

    auto length = reader->get_data(*buffer, BUFFER_SIZE);
    if (length > 0) {
        bh->start(reader->get_block_type());
        bh->fill_buffer(buffer, length);

        next_level = bh->get_level();
        next_period = bh->get_period();
        next_duration = bh->get_duration();

        timer1->set_min();
    }

    digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
    if (reader->is_pause()) return;

    if (bh->is_buffer_empty()) {
        auto length = reader->get_data(*buffer, BUFFER_SIZE);
        bh->fill_buffer(buffer, length);
    }
}

#define OUTPUTPIN 4

template<>
void timer<1>::handler()
{
    digitalWrite(OUTPUTPIN, next_level ? HIGH : LOW);
    if (0 != next_period) {
        timer1->set(next_period);
    } else {
        timer1->stop();
    }
    if (0 != next_duration) {
        timer2->set(next_duration);
    }
    next_level = bh->get_level();
    next_period = bh->get_period();
    next_duration = bh->get_duration();
}

template<>
void timer<2>::handler()
{
    bh->switch_next();
    timer2->stop();
}
