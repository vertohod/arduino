#include "TZXDescription.h"
#include "FileReader.h" 
#include "Extensions.h"
#include "Functions.h"

FileReader::FileReader(SD& sd, const char* fileName)
    : mSD(sd)
    , mBlockType(0)
    , mBlockTypeKnown(false)
    , mBlockSize(0)
    , mBlockRead(0)
    , mLastBlock(0)
    , mState(STATE::READING)
{
    mFileType = Functions::getFileType(fileName);
    mFile = mSD.open(fileName);
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

uint16_t FileReader::getData(byte *buffer, uint16_t bufferSize) {
    if (mState != STATE::READING) return 0;

    uint16_t counter = 0;
    switch (mFileType) {
        case tFileType::TAP:
            counter = processFileTAP(buffer, bufferSize);
        case tFileType::TZX:
            counter = processFileTZX(buffer, bufferSize);
        default:
            break;
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

void FileReader::readContinue(bool lastBlock) {
    mBlockType = 0;
    mBlockTypeKnown = false;
    mBlockSize = 0;
    mBlockRead = 0;
    mState = STATE::READING;
    if (lastBlock) {
        mFile.seek(mLastBlock);
    }
}

bool FileReader::isFinished() {
    return STATE::END == mState;
}

void FileReader::setNextBlock() {
    mFile.seek(mLastBlock);
    uint8_t i = 2;
    while (true) {
        mLastBlock += Functions::readLE16(mFile) + 2;
        mFile.seek(mLastBlock);
        if (0 != --i) {
            break;
        }
    }
}

void FileReader::setPreviousBlock() {
    mFile.seek(0);
    uint32_t previousPosition = 0;
    uint32_t counter = 0;
    while (0 != mLastBlock) {
        uint32_t blockSize = Functions::readLE16(mFile);
        if (0 == blockSize) {
            break;
        }
        if (0 == mFile.read()) {
            previousPosition = counter;
        }
        counter += blockSize + 2;
        if (mLastBlock == counter) {
            break;
        }
        mFile.seek(counter);
    }
    mLastBlock = previousPosition;
    mFile.seek(mLastBlock);
}

uint16_t FileReader::processFileTAP(byte *buffer, uint16_t bufferSize) {
    auto lastPosition = mFile.position();
    if (mBlockSize == 0) {
        mBlockSize = Functions::readLE16(mFile);
    }
    if (mBlockSize == 0) {
        mState = STATE::END;
        return 0;
    }
    uint16_t counter = 0;
    for (; counter < bufferSize;) {
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
            if (0 != mBlockType) {
                mLastBlock = mFile.position();
            }
            setPause();
            break;
        }
    }
    return counter;
}

uint16_t FileReader::processFileTZX(byte *buffer, uint16_t buffer_size)
{
    return 0;
}
