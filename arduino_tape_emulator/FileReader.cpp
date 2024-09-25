#include "TZXDescription.h"
#include "FileReader.h" 
#include "Extensions.h"
#include "Functions.h"

FileReader::FileReader(SD& sd, const char* fileName)
    : mBlockType(0)
    , mBlockTypeKnown(false)
    , mBlockSize(0)
    , mBlockRead(0)
    , mLastBlock(0)
    , mState(STATE::END)
{
    mFileType = Functions::getFileType(fileName);
    mFile = sd.open(fileName);
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

byte FileReader::getBlockType() {
    return mBlockType;
}

void FileReader::setPause() {
    mState = STATE::PAUSE;
    mBlockHandler.init();
}

bool FileReader::isPause() {
    return STATE::PAUSE == mState && mBlockHandler.isFinished() && mBlockHandler.isBufferEmpty();
}

void FileReader::readContinue(bool lastBlock) {
    mBlockType = 0;
    mBlockTypeKnown = false;
    mBlockSize = 0;
    mBlockRead = 0;
    mState = STATE::READ;
    if (lastBlock) {
        mFile.seek(mLastBlock);
    }
}

bool FileReader::isFinished() {
    return STATE::END == mState && mBlockHandler.isFinished();
}

bool FileReader::isBlockHandlerFinished() {
    return mBlockHandler.isFinished();
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

uint16_t FileReader::processData(byte *buffer, uint16_t bufferSize) {
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

tRange FileReader::processFile(const tRange& range) {
    switch (mFileType) {
        case tFileType::TAP:
            return processFileTAP();
        case tFileType::TZX:
            return processFileTZX(range);
        default:
            return tRange();
            break;
    }
}

tRange FileReader::processFileTAP() {
    mTZXBlock.setDefaultSettings();
    mBlockHandler.setSignalSettings(mTZXBlock.getSignalSettings());
    return tRange(0, mFile.size(), mFile.size());
}

tRange FileReader::processFileTZX(const tRange& range) {
    mTZXBlock.readHeader(mFile);
    mTZXBlock.readBlock(mFile);
    mBlockHandler.setSignalSettings(mTZXBlock.getSignalSettings());
    return tRange(0, 0, 0);
}

void FileReader::prepearData() {
    auto dataBuffer = mBlockHandler.getExternalBuffer();
    uint16_t length = processData(dataBuffer, TAPE_BUFFER_SIZE);
    if (0 < length) {
        mBlockHandler.fillBuffer(dataBuffer, length);
        mBlockHandler.start(getBlockType());
    }
    return 0 != length;
}

level FileReader::getLevel() {
    return mBlockHandler.getLevel();
}

float FileReader::getPeriod() {
    return mBlockHandler.getPeriod();
}

void FileReader::start() {
    mState = STATE::BEGIN;
}

void FileReader::processing() {
    if (STATE::BEGIN == mState) {
        mRange = processFile(tRange());
        prepearData();
        mState = STATE::READ;
    }
    if (STATE::READ == mState) {
        auto dataBuffer = mBlockHandler.getExternalBuffer();
        uint16_t length = processData(dataBuffer, TAPE_BUFFER_SIZE);
        if (length > 0) {
            mBlockHandler.fillBuffer(dataBuffer, length);
        }
    }
}
