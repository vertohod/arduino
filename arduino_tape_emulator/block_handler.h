#ifndef BLOCK_HANDLER_H
#define BLOCK_HANDLER_H

#include "types.h"

// Signal duration in ticks
#define PILOT_SGN       2168
#define SYNC_SGN1       667
#define SYNC_SGN2       735
#define SYNC_SGN3       954
#define LG1_SGN         1710
#define LG0_SGN         855

#define PILOT_HEADER_IMPULSES   3228 
#define PILOT_DATA_IMPULSES     1614 

#define Z80_FRQ_Hz 3500000

class block_handler
{
private:
    volatile byte *m_buffer_in;
    volatile size_t m_length_in;
    volatile byte *m_buffer_out;
    volatile size_t m_length_out;

    enum STAGE
    {
        PILOT,
        SYNC1,
        SYNC2,
        DATA,
        SYNC3,
        FINAL1,
        FINAL2,
        FINISH
    };

    size_t m_index_byte;
    byte m_mask;
    byte m_current_byte;

    volatile STAGE m_stage;
    bool m_current_bit_one;
    bool  m_meander_up;
    size_t m_period;

    size_t m_impulse_couter;

public:
    block_handler(size_t buffer_size) : m_stage(STAGE::FINISH)
    {
        m_buffer_in = new byte[buffer_size];
        m_buffer_out = new byte[buffer_size];
        init();
    }

    ~block_handler()
    {
        delete[] m_buffer_out;
        delete[] m_buffer_in;
    }

    bool is_buffer_empty()
    {
        return 0 == m_length_in;
    }

    byte* fill_buffer(byte *buffer, size_t length)
    {
        auto old_buffer = m_buffer_in;
        m_buffer_in = buffer;
        m_length_in = length;
        return old_buffer;
    }

private:
    void init()
    {
        m_length_in = 0;
        m_length_out = 0;
        m_index_byte = 0;
        m_mask = 0;
        m_current_bit_one = false;
        m_meander_up = true;
        m_period = 0;
    }

    bool move_data()
    {
        if (m_length_in == 0) return false;

        auto temp = m_buffer_out;
        m_buffer_out = m_buffer_in;
        m_length_out = m_length_in;
        m_buffer_in = temp;
        m_length_in = 0;

        return true;
    }

public:
    void start(byte type)
    {
        m_stage = STAGE::PILOT;
        m_impulse_couter = (0 == type ? PILOT_HEADER_IMPULSES : PILOT_DATA_IMPULSES);
    }

    void stop()
    {
        m_stage = STAGE::FINISH;
    }

    bool get_bit()
    {
        if (0 == m_mask) {
            m_mask = 0x80;
            m_current_byte = m_buffer_out[m_index_byte++];
            if (m_index_byte == m_length_out) m_length_out = 0;
        }
        bool result = m_current_byte & m_mask;
        m_mask >>= 1;
        return result;
    }

    bool get_level()
    {
        switch (m_stage) {
            case STAGE::PILOT:
                m_period = PILOT_SGN;
                if (!m_meander_up) {
                    --m_impulse_couter;
                    if (0 == m_impulse_couter) m_stage = STAGE::SYNC1;
                }
                m_meander_up = !m_meander_up;
                return !m_meander_up;
            case STAGE::SYNC1:
                m_period = SYNC_SGN1;
                m_stage = STAGE::SYNC2;
                return true;
            case STAGE::SYNC2:
                m_period = SYNC_SGN2;
                m_stage = STAGE::DATA;
                m_meander_up = true;  // for any case
                return false;
            case STAGE::DATA:
                if (m_meander_up) {
                    if (0 == m_length_out && 0 == m_mask) {
                        if (move_data()) {
                            m_index_byte = 0;
                            m_mask = 0;
                        } else {
                            m_period = SYNC_SGN3;
                            m_stage = STAGE::FINAL1;
                            return true;
                        }
                    }
                    m_current_bit_one = get_bit();
                }
                m_period = m_current_bit_one ? LG1_SGN : LG0_SGN;
                m_meander_up = !m_meander_up;
                return !m_meander_up;
            case STAGE::FINAL1:
                m_stage = STAGE::FINAL2;
                return false;
            case STAGE::FINAL2:
                m_stage = STAGE::FINISH;
                return false;
            case STAGE::FINISH:
                init();
                return false;
            default:
                return false;
        }
    }
    double get_period()
    {
        return static_cast<double>(m_period) / Z80_FRQ_Hz * 2;
    }
    bool is_pilot()
    {
        return STAGE::PILOT == m_stage;
    }
    bool is_finished()
    {
        return STAGE::FINISH == m_stage;
    }
};

#endif
