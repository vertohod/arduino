#include "FileReader.h" 

FileReader::FileReader(const char* fileName)
    : mBlockType(0)
    , mBlockTypeKnown(false)
    , mBlockSize(0)
    , mBlockRead(0)
    , mState(STATE::READING)
{
    mFile = SD.open(fileName);
}

FileReader::~FileReader()
{
    mFile.close();
}

size_t FileReader::getBlockSize()
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

size_t FileReader::getData(byte *buffer, size_t buffer_size)
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

byte FileReader::getBlockType()
{
    return mBlockType;
}

bool FileReader::isPause()
{
    return STATE::PAUSE == mState;
}

void FileReader::readContinue()
{
    if (isPause()) {
        mBlockType = 0;
        mBlockTypeKnown = false;
        mBlockSize = 0;
        mBlockRead = 0;
        mState = STATE::READING;
    }
}

bool FileReader::isFinished()
{
    return STATE::END == mState;
}
