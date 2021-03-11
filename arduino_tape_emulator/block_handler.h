#ifndef BLOCK_HANDLER_H
#define BLOCK_HANDLER_H

// Signal duration in ticks
#define PILOT_SGN       2168
#define SYNC_SGN_UP     667
#define SYNC_SGN_DN     735
#define LG1_SGN         1710
#define LG0_SGN         855

#define DURATION_PILOT_HEADER    8.0 // seconds
#define DURATION_PILOT_DATA      4.0 // seconds

#define Z80_FRQ_Mhz     3.5

class block_handler
{
private:
    volatile byte *m_buffer_in;
    volatile size_t m_length_in;
    volatile byte *m_buffer_out;
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
    bool  m_meander_up;
    size_t m_period;
    volatile double m_duration;
    bool m_request_switch;

public:
    block_handler(size_t buffer_size) : m_stage(STAGE::BEGIN)
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
        m_index_bit = 0;
        m_current_bit_one = false;
        m_meander_up = true;
        m_period = 0;
        m_duration = 0.0;
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
        if (!move_data()) {
//            Serial.println("block_handler::start, move_date returned FALSE");
            return;
        }
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
            m_current_byte = m_buffer_out[m_index_byte++];
            if (m_index_byte == m_length_out) m_length_out = 0;
        }
        return (m_current_byte & (1 << --m_index_bit)) != 0;
    }

    bool get_level()
    {
        switch (m_stage) {
            case STAGE::BEGIN:
                return false;
            case STAGE::PILOT:
                m_period = PILOT_SGN;
                if (m_meander_up && m_request_switch) {
                    m_stage = STAGE::SYNC;
                    m_request_switch = false;
                }
                m_meander_up = !m_meander_up;
                return !m_meander_up;
            case STAGE::SYNC:
                m_period = m_meander_up ? SYNC_SGN_UP : SYNC_SGN_DN;
                if (m_meander_up) {
                    m_stage = STAGE::BUFFER;
                }
                m_meander_up = !m_meander_up;
                return !m_meander_up;
            case STAGE::BUFFER:
                if (m_request_switch) {
                    m_stage = STAGE::END;
                }
                if (m_meander_up) {
                    if (m_length_out == 0) {
                        if (move_data()) {
                            m_index_byte = 0;
                            m_index_bit = 0;
                        } else {
                            m_stage = STAGE::END;
                            return false;
                        }
                    }
                    m_current_bit_one = get_bit();
                }
                m_period = m_current_bit_one ? LG1_SGN : LG0_SGN;
                m_meander_up = !m_meander_up;
                return !m_meander_up;
            case STAGE::END:
                init();
                return false;
            default:
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

    double get_period()
    {
        return (static_cast<double>(m_period) / Z80_FRQ_Mhz) / 1000000;
    }

    byte get_period_byte()
    {
        if (PILOT_SGN == m_period) return 142;
        if (SYNC_SGN_UP == m_period) return 43;
        if (SYNC_SGN_DN == m_period) return 48; 
        if (LG1_SGN == m_period) return 112;
        if (LG0_SGN == m_period) return 56;

        return 0;
    }

    double get_duration()
    {
        auto result = m_duration;
        m_duration = 0.0;
        return result;
    }
    bool is_pilot()
    {
        return STAGE::PILOT == m_stage;
    }
    bool is_finished()
    {
        return STAGE::END == m_stage;
    }
};

#endif
