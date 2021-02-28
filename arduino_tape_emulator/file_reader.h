#ifndef FILE_READER_H
#define FILE_READER_H

class file_reader
{
private:
    File    m_file;
    byte    m_block_type;
    size_t  m_block_size;
    size_t  m_block_read;

    enum STATE {
        READING,
        PAUSE
    };

    STATE m_state;

public:
    file_reader(char* file_name) :
        m_block_type(0),
        m_block_size(0),
        m_block_read(0),
        m_state(STATE::READING)
    {
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
        if (m_state == STATE::PAUSE) return 0;

        size_t counter = 0;
        for (; counter < buffer_size; ++counter) {
            if (!m_file.available()) break; 

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
    STATE get_state()
    {
        m_state;
    }
    void read_continue()
    {
        m_state = STATE::READING;
    }
};

#endif
