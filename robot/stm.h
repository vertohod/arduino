#ifndef STM_H
#define STM_H

#include <LiquidCrystal_I2C.h>

class stm
{
public:
    enum symbols : char {
        dec = 0x00,
        hex = 0x01,
        flush = 0x10,
        endl = 0x13
    };

private:
    LiquidCrystal_I2C& lcd;
    symbols base;
    const size_t cols;
    const size_t rows;
    char **buffer;
    volatile size_t current_col;
    volatile size_t current_row;

public:
    stm(LiquidCrystal_I2C& lcd_arg, size_t cols_arg, size_t rows_arg) :
        lcd(lcd_arg),
        base(stm::dec),
        cols(cols_arg),
        rows(rows_arg),
        buffer(nullptr),
        current_col(0),
        current_row(0)
    {
        buffer = new char*[rows];
        for (size_t i = 0; i < rows; ++i) {
            buffer[i] = nullptr; 
        }
        for (size_t i = 0; i < rows; ++i) {
            buffer[i] = new char[cols + 1];
            clear_row(buffer[i]);
        }
    }

    ~stm()
    {
        if (buffer != nullptr) {
            for (size_t i = 0; i < rows; ++i) {
                if (buffer[i] != nullptr) {
                    delete[] buffer[i];
                }
            }
            delete[] buffer;
        }
    }

    stm& operator<<(const char* buff)
    {
        fill_buffer(buff);
        return *this;
    }

    stm& operator<<(char ch)
    {
        fill_buffer(String(ch).c_str());
        return *this;
    }

    stm& operator<<(String str)
    {
        fill_buffer(str.c_str());
        return *this;
    }

    stm& operator<<(int num)
    {
        if (base == stm::dec) {
            fill_buffer(String(num, DEC).c_str());
        } else if (base == stm::hex) {
            fill_buffer(String(num, HEX).c_str());
        }
        return *this;
    }

    stm& operator<<(long unsigned int num)
    {
        if (base == stm::dec) {
            fill_buffer(String(num, DEC).c_str());
        } else if (base == stm::hex) {
            fill_buffer(String(num, HEX).c_str());
        }
        return *this;
    }

    stm& operator<<(symbols sm)
    {
        switch (sm) {
            case stm::dec:
                base = stm::dec;
                break;
            case stm::hex:
                base = stm::hex;
                break;
            case stm::flush:
                do_flush();
                break;
            case stm::endl:
                do_flush();
                do_endl();
                break;
            default:
                break;
        }
        return *this;
    }

private:
    void clear_row(char* buff)
    {
        for (size_t i = 0; i < cols + 1; ++i) {
            buff[i] = 0;
        }
    }

    void fill_spaces_row(char* buff)
    {
        for (size_t i = 0; i < cols; ++i) {
            buff[i] = ' ';
        }
    }

    void fill_buffer(const char* buff)
    {
        size_t counter = 0;
        while (buff[counter] != 0) {
            buffer[current_row][current_col] = buff[counter];
            ++counter;
            ++current_col;
            if (current_col == cols) {
                do_endl();
            }
        }
    }

    void do_endl()
    {
        buffer[current_row][current_col] = 0;
        current_col = 0;
        ++current_row;
        if (current_row == rows) {
            if (rows > 1) {
                for (size_t i = 0; i < (rows - 1); ++i) {
                    copy_row(buffer[i], buffer[i + 1]);
                }
            }
            --current_row;
            fill_spaces_row(buffer[current_row]);
        }
    }

    void copy_row(char* buff, const char* buff_source)
    {
        bool flag_end = false;
        for (size_t i = 0; i < cols; ++i) {
            char ch = buff_source[i];

            if (ch == 0) flag_end = true;

            if (flag_end) buff[i] = ' ';
            else buff[i] = ch;
        }
    }

    void do_flush()
    {
        for (size_t i = 0; i < rows; ++i) {
            lcd.setCursor(0, i);
            lcd.print(buffer[i]);
        }
    }
};

#endif
