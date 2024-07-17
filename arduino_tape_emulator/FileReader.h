#ifndef FILE_READER_H
#define FILE_READER_H

#include <SPI.h>
#include <SD.h>
#include "Types.h"

class FileReader
{
private:
    uint32_t    mFilePosition;
    uint32_t    mFileSize;
    uint32_t    mLastBlock;
    File        mFile;
    byte        mBlockType;
    byte        mBlockTypeKnown;
    uint16_t    mBlockSize;
    uint16_t    mBlockRead;

    enum STATE {
        READING,
        PAUSE,
        END
    };
    STATE       mState;

public:
    FileReader(const char* fileName, uint32_t filePosition = 0);
    ~FileReader();

    uint16_t getBlockSize();
    uint16_t getData(byte *buffer, uint16_t buffer_size);
    byte getBlockType();
    void setPause();
    bool isPause();
    void readContinue(uint32_t position = 0);
    bool isFinished();
    uint32_t getFilePosition();
    uint32_t getFileSize();
    uint32_t getLastBlock();
};

#endif
