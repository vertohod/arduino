#include "types.h"
#include "block_handler.h"
#include "file_reader.h"
#include "timer1.h"

#define OUTPUTPIN 4
#define BUFFER_SIZE 32
#define DURATION_PAUSE 2.0 // seconds

block_handler *bh = nullptr;
file_reader *reader = nullptr;

byte* buffer = new byte[BUFFER_SIZE];

void setup()
{
//    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    reader = new file_reader("tape.tap");
    bh = new block_handler(BUFFER_SIZE);

    // Enable INT0
    EICRA = 1 << ISC01 | 1 << ISC00;
    EIMSK = 1 << INT0;

    DDRD = B00010000;

    digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
    if (reader->is_pause()) return;

    if (!bh->is_finished() && bh->is_buffer_empty()) {
        size_t length = reader->get_data(buffer, BUFFER_SIZE);
        if (length > 0) {
            buffer = bh->fill_buffer(buffer, length);
        }
    }
}

class duration_counter
{
private:
    bool m_enabled;
    double m_duration;

public:
    duration_counter() : m_enabled(false), m_duration(0.0) {}
    void set(double duration)
    {
        m_duration = duration;
        m_enabled = duration != 0.0;
    }
    bool enabled() { return m_enabled; }
    bool check(double period)
    {
        if (m_duration <= period) {
            m_enabled = false;
            return true;
        } else {
            m_duration -= period;
        }
        return false;
    }
};

duration_counter dc;
bool next_level_up = false;
double next_period = 0.0;

void start_reading()
{
    size_t length = reader->get_data(buffer, BUFFER_SIZE);
    if (length > 0) {
        buffer = bh->fill_buffer(buffer, length);
        bh->start(reader->get_block_type());

        // initialization
        next_level_up = bh->get_level();
        next_period = bh->get_period();

        // just launch the process with some period
        timer1::instance().init(next_period);
        timer1::instance().start();
    }
}

// -------------- Handlers ----------------

ISR(TIMER1_COMPA_vect)
{
    cli();

    if (reader->is_pause() && bh->is_finished()) {
        if (dc.enabled()) {
            if (dc.check(timer1::instance().duration())) {
                reader->read_continue();
                start_reading();
            }
        } else {
            dc.set(DURATION_PAUSE);
            timer1::instance().init(DURATION_PAUSE);
            timer1::instance().start();

            sei();
            return;
        }
    }

    if (!bh->is_finished()) {
        PORTD = next_level_up ? 0xff : 0x00;
        timer1::instance().start();

        next_level_up = bh->get_level();
        next_period = bh->get_period();

        // initialize the timer now do not waste time
        timer1::instance().init(next_period);
    }

    sei();
}

ISR(INT0_vect)
{
    start_reading();
}
