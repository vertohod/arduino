
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

#define SIZE_BUFFER     16

#define DURATION_PILOT_HEADER    5 // seconds
#define DURATION_PILOT_DATA      2 // seconds
#define DURATION_PAUSE           2 // seconds

class block_handler
{
private:
    volatile byte **m_buffer_in;
    volatile size_t m_length_in;
    volatile byte **m_buffer_out;
    volatile size_t m_length_out;

public:
    enum BLOCK_TYPE
    {
        HEADER,
        DATA
    };

private:
    enum STAGE
    {
        PILOT,
        SYNC,
        BUFFER,
        PAUSE,
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

public:
    block_handler() : m_length_in(0), m_stage(STAGE::END)
    {
        m_buffer_in = new byte*;
        m_buffer_out = new byte*;

        *m_buffer_in = new byte[SIZE_BUFFER];
        *m_buffer_out = new byte[SIZE_BUFFER];
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
    void start(BLOCK_TYPE type)
    {
        if (m_stage != STAGE::END) return;
        if (!move_data()) return;

        m_stage = STAGE::PILOT;
        m_duration = (type == BLOCK_TYPE::HEADER ? DURATION_PILOT_HEADER : DURATION_PILOT_DATA);
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
            case STAGE::PILOT:
                m_period = m_meander_up ? PILOT_SGN_UP : PILOT_SGN_DN;
                if (!m_meander_up && m_duration == 0) m_stage = STAGE::SYNC;
                m_meander_up = !m_meander_up;
                return !m_meander_up;
            case STAGE::SYNC:
                m_period = m_meander_up ? SYNC_SGN_UP : SYNC_SGN_DN;
                if (!m_meander_up) m_stage = STAGE::BUFFER;
                m_meander_up = !m_meander_up;
                return !m_meander_up;
            case STAGE::BUFFER:
                if (m_meander_up) {
                    if (m_length_out == 0) {
                        if (m_length_in == 0) {
                            m_stage = STAGE::PAUSE;
                            m_duration = DURATION_PAUSE;
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
            case STAGE::PAUSE:
                if (m_duration == 0) m_stage = STAGE::END;
                return false;
            default:
                init();
                return false;
        }
    }

    size_t get_period()
    {
        static const double factor = TIMER_FRQ_Mhz / Z80_FRQ_Mhz;
        return m_period * factor;
    }

    size_t get_duration()
    {
        return m_duration;
    }

    void unset_duration()
    {
        m_duration = 0;
    }
};

block_handler global_block_handler_object;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
