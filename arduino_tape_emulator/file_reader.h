#ifndef FILE_READER_H
#define FILE_READER_H

#include <SPI.h>
#include <SD.h>

class file_reader
{
private:
    File    m_file;
    byte    m_block_type;
    byte    m_block_type_known;
    volatile size_t  m_block_size;
    volatile size_t  m_block_read;

    enum STATE {
        READING,
        PAUSE,
        END
    };

    volatile STATE m_state;

public:
    file_reader(uint8_t sdpin, char* file_name) :
        m_block_type(0),
        m_block_type_known(false),
        m_block_size(0),
        m_block_read(0),
        m_state(STATE::READING)
    {
        if (!SD.begin(sdpin)) {
            while (1);
        }
        m_file = SD.open(file_name);
    }
    ~file_reader()
    {
        m_file.close();
    }

    size_t get_block_size()
    {
        byte first_byte = 0;
        byte second_byte = 0;

        if (m_file.available()) {
            first_byte = m_file.read();
        }
        if (m_file.available()) {
            second_byte = m_file.read();
        }
        return first_byte | second_byte << 8;
    }

    size_t get_data(byte *buffer, size_t buffer_size)
    {
        if (m_state != STATE::READING) return 0;

        if (m_block_size == 0) {
            m_block_size = get_block_size();
        }
        if (m_block_size == 0) {
            m_state = STATE::END;
            return 0;
        }

        size_t counter = 0;
        for (; counter < buffer_size;) {
            if (!m_file.available()) {
                m_state = STATE::END;
                break; 
            }
            buffer[counter] = m_file.read();
            ++counter;
            ++m_block_read;

            if (!m_block_type_known) {
                m_block_type = buffer[0];
                m_block_type_known = true;
            }
            if (m_block_read == m_block_size) {
                m_state = STATE::PAUSE;
                break;
            }
        }
        return counter;
    }
    byte get_block_type()
    {
        return m_block_type;
    }
    bool is_pause()
    {
        return STATE::PAUSE == m_state;
    }
    void read_continue()
    {
        if (is_pause()) {
            m_block_type = 0;
            m_block_type_known = false;
            m_block_size = 0;
            m_block_read = 0;
            m_state = STATE::READING;
        }
    }
    bool is_finished()
    {
        return STATE::END == m_state;
    }
};

#endif
