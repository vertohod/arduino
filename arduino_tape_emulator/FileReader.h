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
    size_t  mBlockSize;
    size_t  mBlockRead;

    enum STATE {
        READING,
        PAUSE,
        END
    };
    STATE mState;

public:
    FileReader(const char* fileName); 
    ~FileReader();

    size_t getBlockSize();
    size_t getData(byte *buffer, size_t buffer_size);
    byte getBlockType();
    bool isPause();
    void readContinue();
    bool isFinished();
};

#endif
