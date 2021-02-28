#include "block_handler.h"
#include "file_reader.h"
#include "timer.h"

#define BUFFER_SIZE     16

block_handler *bh = nullptr;
timer<1> *timer1 = nullptr;
timer<2> *timer2 = nullptr;
file_reader *reader = nullptr;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    reader = new file_reader("tape.tap");
    bh = new block_handler(BUFFER_SIZE);
    timer1 = new timer<1>();
    timer2 = new timer<2>();

    digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
    static byte** buffer = block_handler::init_buffer(BUFFER_SIZE); 

    if (!bh->is_finished() && bh->is_buffer_empty()) {
        auto length = reader->get_data(*buffer, BUFFER_SIZE);
        bh->fill_buffer(buffer, length);
    }
}

#define OUTPUTPIN 4

template<>
void timer<1>::handler()
{
    if (bh->is_finished()) return;

    auto level = bh->get_level();
    auto period = bh->get_period();
    auto duration = bh->get_duration();

    digitalWrite(OUTPUTPIN, level ? HIGH : LOW);
    OCR1A = period;
    OCR2A = duration;
}

template<>
void timer<2>::handler()
{
}
