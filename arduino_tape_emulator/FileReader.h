#ifndef FILE_READER_H
#define FILE_READER_H

#include <SPI.h>
#include <SD.h>
#include "string.h"

class FileReader
{
private:
    File mFile;
    byte mBlockType;
    byte mBlockTypeKnown;
    volatile size_t  mBlockSize;
    volatile size_t  mBlockRead;

    enum STATE {
        READING,
        PAUSE,
        END
    };

    volatile STATE mState;

public:
    FileReader(uint8_t sd_cs, const string& fileName) :
        mBlockType(0),
        mBlockTypeKnown(false),
        mBlockSize(0),
        mBlockRead(0),
        mState(STATE::READING)
    {
        if (!SD.begin(sd_cs)) {
            while (1);
        }
        mFile = SD.open(fileName.c_str());
    }
    ~FileReader()
    {
        mFile.close();
    }

    size_t getBlockSize()
    {
        byte firstByte = 0;
        byte secondByte = 0;

        if (mFile.available()) {
            firstByte = mFile.read();
        }
        if (mFile.available()) {
            secondByte = mFile.read();
        }
        return firstByte | secondByte << 8;
    }

    size_t getData(volatile byte *buffer, size_t buffer_size)
    {
        if (mState != STATE::READING) return 0;

        if (mBlockSize == 0) {
            mBlockSize = getBlockSize();
        }
        if (mBlockSize == 0) {
            mState = STATE::END;
            return 0;
        }

        size_t counter = 0;
        for (; counter < buffer_size;) {
            if (!mFile.available()) {
                mState = STATE::END;
                break; 
            }
            buffer[counter] = mFile.read();
            ++counter;
            ++mBlockRead;

            if (!mBlockTypeKnown) {
                mBlockType = buffer[0];
                mBlockTypeKnown = true;
            }
            if (mBlockRead == mBlockSize) {
                mState = STATE::PAUSE;
                break;
            }
        }
        return counter;
    }
    byte getBlockType()
    {
        return mBlockType;
    }
    bool isPause()
    {
        return STATE::PAUSE == mState;
    }
    void readContinue()
    {
        if (isPause()) {
            mBlockType = 0;
            mBlockTypeKnown = false;
            mBlockSize = 0;
            mBlockRead = 0;
            mState = STATE::READING;
        }
    }
    bool isFinished()
    {
        return STATE::END == mState;
    }
};

#endif
