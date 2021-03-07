#ifndef FILE_READER_H
#define FILE_READER_H

#include <SPI.h>
#include <SD.h>

#define SDPIN 8

class file_reader
{
private:
    File    m_file;
    byte    m_block_type;
    size_t  m_block_size;
    size_t  m_block_read;

    enum STATE {
        READING,
        PAUSE,
        END
    };

    STATE m_state;

public:
    file_reader(char* file_name) :
        m_block_type(0),
        m_block_size(0),
        m_block_read(0),
        m_state(STATE::READING)
    {
        if (!SD.begin(SDPIN)) {
            while (1);
        }
        m_file = SD.open(file_name);
        // TODO
        // if (!m_file) throw "File can not be opend";
    }
    ~file_reader()
    {
        m_file.close();
    }
    size_t get_data(byte* buffer, size_t buffer_size)
    {
        if (m_state != STATE::READING) return 0;

        size_t counter = 0;
        for (; counter < buffer_size; ++counter) {
            if (!m_file.available()) {
                m_state = STATE::END;
                break; 
            }

            buffer[counter] = m_file.read();

            if (m_block_size == 0) {
                // 3 bytes were read. It's possible to get
                // the size of block and its type
                if (counter == 2) {
                    m_block_size = buffer[0] | buffer[1] << 8;
                    m_block_type = buffer[2];
                }
            } else if (++m_block_read == m_block_size) {
                m_block_size = 0;
                m_block_read = 0;
                m_state = STATE::PAUSE;
                return counter + 1;
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
            m_state = STATE::READING;
        }
    }
    bool is_finished()
    {
        return STATE::END == m_state;
    }
};

#endif
