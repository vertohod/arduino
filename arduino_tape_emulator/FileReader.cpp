#include "FileReader.h" 

FileReader::FileReader(const char* fileName)
    : mBlockType(0)
    , mBlockTypeKnown(false)
    , mBlockSize(0)
    , mBlockRead(0)
    , mLastBlock(0)
    , mState(STATE::READING)
{
    mFile = SD.open(fileName);
}

FileReader::~FileReader() {
    mFile.close();
}

uint32_t FileReader::getFilePosition() {
    return mFile.position();
}

uint32_t FileReader::getFileSize() {
    return mFile.size();
}

uint32_t FileReader::getLastBlock() {
    return mLastBlock;
}

uint16_t FileReader::getBlockSize() {
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

uint16_t FileReader::getData(byte *buffer, uint16_t buffer_size) {
    if (mState != STATE::READING) return 0;

    auto lastPosition = mFile.position();
    if (mBlockSize == 0) {
        mBlockSize = getBlockSize();
    }
    if (mBlockSize == 0) {
        mState = STATE::END;
        return 0;
    }

    uint16_t counter = 0;
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
            if (0 == mBlockType) {
                mLastBlock = lastPosition;
            }
        }
        if (mBlockRead == mBlockSize) {
            setPause();
            break;
        }
    }
    return counter;
}

byte FileReader::getBlockType() {
    return mBlockType;
}

void FileReader::setPause() {
    mState = STATE::PAUSE;
}

bool FileReader::isPause() {
    return STATE::PAUSE == mState;
}

void FileReader::readContinue(uint32_t position) {
    if (isPause()) {
        mBlockType = 0;
        mBlockTypeKnown = false;
        mBlockSize = 0;
        mBlockRead = 0;
        mState = STATE::READING;

        if (0 != position) {
            mLastBlock = position;
            mFile.seek(position);
        }
    }
}

bool FileReader::isFinished() {
    return STATE::END == mState;
}
