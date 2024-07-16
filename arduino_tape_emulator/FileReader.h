#ifndef FILE_READER_H
#define FILE_READER_H

#include <SPI.h>
#include <SD.h>
#include "Types.h"

class FileReader
{
private:
    File mFile;
    byte mBlockType;
    byte mBlockTypeKnown;
    uint16_t  mBlockSize;
    uint16_t  mBlockRead;

    enum STATE {
        READING,
        PAUSE,
        END
    };
    STATE mState;

public:
    FileReader(const char* fileName); 
    ~FileReader();

    uint16_t getBlockSize();
    uint16_t getData(byte *buffer, uint16_t buffer_size);
    byte getBlockType();
    bool isPause();
    void readContinue();
    bool isFinished();
    static uint32_t getFileSize(const char *path);
};

#endif
