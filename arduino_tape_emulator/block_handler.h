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

#define DURATION_PILOT_HEADER    5.0 // seconds
#define DURATION_PILOT_DATA      2.0 // seconds

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
    byte m_current_bit_one;
    byte  m_meander_up;
    size_t m_period;
    volatile double m_duration;
    byte m_request_switch;

public:
    block_handler(size_t buffer_size) :
        m_length_in(0),
        m_length_out(0),
        m_stage(STAGE::BEGIN)
    {
        m_buffer_in = new byte[buffer_size];
        m_buffer_out = new byte[buffer_size];
    }

    ~block_handler()
    {
        delete[] m_buffer_out;
        delete[] m_buffer_in;
    }

    byte is_buffer_empty()
    {
        return 0 == m_length_in ? 1 : 0;
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
        m_index_byte = 0;
        m_index_bit = 0;
        m_current_bit_one = true;
        m_meander_up = 1;
        m_period = 0;
        m_duration = 0.0;
        m_request_switch = 0;
    }

    byte move_data()
    {
        if (m_length_in == 0) return 0;

        auto temp = m_buffer_out;
        m_buffer_out = m_buffer_in;
        m_length_out = m_length_in;
        m_buffer_in = temp;
        m_length_in = 0;

        return 1;
    }

public:
    void start(byte type)
    {
        if (m_stage != STAGE::BEGIN) return;

        if (move_data() == 0) return;

        m_stage = STAGE::PILOT;
        m_duration = (0 == type ? DURATION_PILOT_HEADER : DURATION_PILOT_DATA);
    }

    void stop()
    {
        m_stage = STAGE::END;
    }

    byte get_bit()
    {
        if (m_index_bit == 0) {
            m_index_bit = 8;
            m_current_byte = m_buffer_out[m_index_byte++];
            if (m_index_byte == m_length_out) m_length_out = 0;
        }
        return (m_current_byte & 1 << --m_index_bit) == 0 ? 0 : 1;
    }

    byte get_level()
    {
        switch (m_stage) {
            case STAGE::BEGIN:
                return false;
            case STAGE::PILOT:
                m_period = 1 == m_meander_up ? PILOT_SGN_UP : PILOT_SGN_DN;
                if (0 == m_meander_up && 1 == m_request_switch) {
                    Serial.println("Switch from PILOT to SYNC");
                    m_stage = STAGE::SYNC;
                    m_request_switch = 0;
                }
                m_meander_up = 1 == m_meander_up ? 0 : 1;
                return m_meander_up ? 0 : 1;
            case STAGE::SYNC:
                m_period = 1 == m_meander_up ? SYNC_SGN_UP : SYNC_SGN_DN;
                if (0 == m_meander_up) {
                    Serial.println("Switch from SYNC to BUFFER");
                    Serial.print("The size of output buffer is: ");
                    Serial.println(m_length_out);
                    Serial.print("The size of input buffer is: ");
                    Serial.println(m_length_in);
                    m_stage = STAGE::BUFFER;
                }
                m_meander_up = 1 == m_meander_up ? 0 : 1;
                return 1 == m_meander_up ? 0 : 1;
            case STAGE::BUFFER:
                if (1 == m_request_switch) {
                    Serial.println("Switch from BUFFER to END");
                    m_stage = STAGE::END;
                }
                if (1 == m_meander_up) {
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
                    m_period = 1 == m_meander_up ? LG1_SGN_UP : LG1_SGN_DN;
                } else {
                    m_period = 1 == m_meander_up ? LG0_SGN_UP : LG0_SGN_DN;
                }
                m_meander_up = 1 == m_meander_up ? 0 : 1;
                return 1 == m_meander_up ? 0 : 1;
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
                m_request_switch = 1;
                break;
            case STAGE::BUFFER:
                m_request_switch = 1;
                break;
            default:
                break;
        }
    }

    double get_period()
    {
        return (static_cast<double>(m_period) / Z80_FRQ_Mhz) / 1000000;
    }

    double get_duration()
    {
        auto result = m_duration;
        m_duration = 0.0;
        return result;
    }
    byte is_pilot()
    {
        return STAGE::PILOT == m_stage ? 1 : 0;
    }
    byte is_finished()
    {
        return STAGE::END == m_stage ? 1 : 0;
    }
};

#endif
