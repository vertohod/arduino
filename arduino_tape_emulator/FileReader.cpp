#include "FileReader.h" 

FileReader::FileReader(const char* fileName, uint32_t filePosition)
    : mFilePosition(filePosition)
    , mFileSize(0)
    , mLastBlock(0)
    , mBlockType(0)
    , mBlockTypeKnown(false)
    , mBlockSize(0)
    , mBlockRead(0)
    , mState(STATE::READING)
{
    mFile = SD.open(fileName);
    mFileSize = mFile.size();

    if (0 < mFilePosition) {
        mFile.seek(mFilePosition);
    }
}

FileReader::~FileReader() {
    mFile.close();
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
        }
        if (mBlockRead == mBlockSize) {
            setPause();
            break;
        }
    }
    mFilePosition += counter;
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

        if (0 == position) {
            mLastBlock = mFilePosition;
        } else {
            mLastBlock = position;
            mFilePosition = position;
            mFile.seek(mFilePosition);
        }
    }
}

bool FileReader::isFinished() {
    return STATE::END == mState;
}

uint32_t FileReader::getFilePosition() {
    return mFilePosition;
}

uint32_t FileReader::getFileSize() {
    return mFileSize;
}

uint32_t FileReader::getLastBlock() {
    return mLastBlock;
}
