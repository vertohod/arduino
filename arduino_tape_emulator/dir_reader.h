#ifndef DIR_READER_H
#define DIR_READER_H

#include <SPI.h>
#include <SD.h>
#include "list.h"
#include "string.h"

typedef list<string> t_file_list;

class dir_reader
{
private:
    t_file_list* m_file_list;

public:
    dir_reader(uint8_t sdpin) : m_file_list(nullptr)
    {
        Serial.print("Initializing SD card...");
        if (!SD.begin(sdpin)) {
            Serial.println("initialization failed.");
            while(true);
        }
    }
    ~dir_reader()
    {
    }

    t_file_list* read_root()
    {
        return read_dir("/");
    }

    t_file_list* read_dir(const char* path)
    {
        m_file_list = new t_file_list();
        
        auto root = SD.open(path);
        while (true) {
            auto file = root.openNextFile();
            if (!file) {
                break;
            }
            if (file.isDirectory()) {
                m_file_list->push(string(file.name()) + "/");
            } else {
                m_file_list->push(file.name());
            }
            file.close();
        }
        root.close();

        return m_file_list;
    }
};

#endif
