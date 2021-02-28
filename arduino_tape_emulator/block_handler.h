#ifndef BLOCK_HANDLER_H
#define BLOCK_HANDLER_H

// Signal duration in ticks
#define PILOT_SGN_UP    2168
#define PILOT_SGN_DN    2168
#define SYNC_SGN_UP     667
#define SYNC_SGN_DN     735
#define LG1_SGN_UP      1710
#define LG1_SGN_DN      1710
#define LG0_SGN_UP      855
#define LG0_SGN_DN      855

#define TIMER_FRQ_Mhz   16.0
#define Z80_FRQ_Mhz     3.5

#define DURATION_PILOT_HEADER    5 // seconds
#define DURATION_PILOT_DATA      2 // seconds

class block_handler
{
private:
    const size_t m_buffer_size;
    volatile byte **m_buffer_in;
    volatile size_t m_length_in;
    volatile byte **m_buffer_out;
    volatile size_t m_length_out;

    enum STAGE
    {
        BEGIN,
        PILOT,
        SYNC,
        BUFFER,
        END
    };

    size_t m_index_byte;
    size_t m_index_bit;
    byte m_current_byte;

    volatile STAGE m_stage;
    bool m_current_bit_one;
    bool m_meander_up;
    size_t m_period;
    volatile size_t m_duration;
    bool m_request_switch;

public:
    static byte** init_buffer(size_t buffer_size)
    {
        byte** result;
        result = new byte*;
        *result = new byte[buffer_size];
        return result;
    }

    block_handler(size_t buffer_size) :
        m_buffer_size(buffer_size),
        m_length_in(0),
        m_stage(STAGE::BEGIN)
    {
        m_buffer_in = init_buffer(buffer_size);
        m_buffer_out = init_buffer(buffer_size);
    }

    ~block_handler()
    {
        delete[] *m_buffer_in;
        delete[] *m_buffer_out;

        delete m_buffer_in;
        delete m_buffer_out;
    }

    bool is_buffer_empty()
    {
        return m_length_in == 0;
    }

    void fill_buffer(byte **buffer, size_t length)
    {
        auto temp = buffer;
        buffer = m_buffer_in;
        m_buffer_in = temp;
        m_length_in = length;
    }

private:
    void init()
    {
        m_index_byte = 0;
        m_index_bit = 0;
        m_current_bit_one = true;
        m_meander_up = true;
        m_period = 0;
        m_duration = 0;
        m_request_switch = false;
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
        if (m_stage != STAGE::BEGIN) return;
        if (!move_data()) return;

        m_stage = STAGE::PILOT;
        m_duration = (0 == type ? DURATION_PILOT_HEADER : DURATION_PILOT_DATA);
    }

    void stop()
    {
        m_stage = STAGE::END;
    }

    bool get_bit()
    {
        if (m_index_bit == 0) {
            m_index_bit = 8;
            m_current_byte = (*m_buffer_out)[m_index_byte++];
            if (m_index_byte == m_length_out) m_length_out = 0;
        }
        return m_current_byte & 1 << --m_index_bit;
    }

    bool get_level()
    {
        switch (m_stage) {
            case STAGE::BEGIN:
                return false;
            case STAGE::PILOT:
                m_period = m_meander_up ? PILOT_SGN_UP : PILOT_SGN_DN;
                if (!m_meander_up && m_request_switch) {
                    m_stage = STAGE::SYNC;
                    m_request_switch = false;
                }
                m_meander_up = !m_meander_up;
                return !m_meander_up;
            case STAGE::SYNC:
                m_period = m_meander_up ? SYNC_SGN_UP : SYNC_SGN_DN;
                if (!m_meander_up) m_stage = STAGE::BUFFER;
                m_meander_up = !m_meander_up;
                return !m_meander_up;
            case STAGE::BUFFER:
                if (m_request_switch) m_stage = STAGE::END;
                if (m_meander_up) {
                    if (m_length_out == 0) {
                        if (m_length_in == 0) {
                            m_stage = STAGE::END;
                            return false;
                        } else {
                            move_data();
                            m_index_byte = 0;
                            m_index_bit = 0;
                        }
                    }
                    m_current_bit_one = get_bit();
                }
                if (m_current_bit_one) {
                    m_period = m_meander_up ? LG1_SGN_UP : LG1_SGN_DN;
                } else {
                    m_period = m_meander_up ? LG0_SGN_UP : LG0_SGN_DN;
                }
                m_meander_up = !m_meander_up;
                return !m_meander_up;
            default:
                init();
                return false;
        }
    }

    void switch_next()
    {
        switch (m_stage) {
            case STAGE::PILOT:
                m_request_switch = true;
                break;
            case STAGE::BUFFER:
                m_request_switch = true;
                break;
            default:
                break;
        }
    }

    size_t get_period()
    {
        static const double factor = TIMER_FRQ_Mhz / Z80_FRQ_Mhz;
        return m_period * factor;
    }

    size_t get_duration()
    {
        auto result = m_duration;
        m_duration = 0;
        return result;
    }

    bool is_finished()
    {
        return m_stage == STAGE::END;
    }
};

#endif
