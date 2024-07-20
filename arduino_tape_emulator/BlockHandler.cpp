#include "BlockHandler.h"
#include "Timings.h"

BlockHandler::BlockHandler(const char*) {
    init();
}

bool BlockHandler::isBufferEmpty() {
    return 0 == mLengthIn;
}

void BlockHandler::fillBuffer(const byte* const buffer, uint16_t length) {
    memcpy(static_cast<void*>(&mBufferIn[0]), static_cast<const void*>(buffer), length);
    mLengthIn = length;
}

void BlockHandler::init() {
    mLengthIn = 0;
    mLengthOut = 0;
    mIndexByte = 0;
    mMask = 0;
    mStage = STAGE::FINISH;
    mPeriod = 0;
}

bool BlockHandler::moveData() {
    if (0 == mLengthIn) return false;

    memcpy(static_cast<void*>(&mBufferOut[0]), static_cast<const void*>(&mBufferIn[0]), mLengthIn);
    mLengthOut = mLengthIn;
    mIndexByte = 0;
    mMask = 0;
    mLengthIn = 0;

    return true;
}

void BlockHandler::start(byte type) {
    mStage = STAGE::PILOT;
    mImpulseCouter = (0 == type ? SPECTRUM_PILOT_HEADER : SPECTRUM_PILOT_DATA);
}

bool BlockHandler::getBit() {
    if (0 == mMask) {
        mMask = 0x80;
        mCurrentByte = mBufferOut[mIndexByte++];
        if (mIndexByte == mLengthOut) mLengthOut = 0;
    }
    bool result = mCurrentByte & mMask;
    mMask >>= 1;
    return result;
}

level BlockHandler::getLevel() {
    switch (mStage) {
        case STAGE::PILOT:
            mPeriod = SPECTRUM_PILOT;
            if (!mMeanderUp) {
                --mImpulseCouter;
                if (0 == mImpulseCouter) mStage = STAGE::SYNC1;
            }
            mMeanderUp = !mMeanderUp;
            return !mMeanderUp;
        case STAGE::SYNC1:
            mPeriod = SPECTRUM_SYNC_SGN1;
            mStage = STAGE::SYNC2;
            return HIGH;
        case STAGE::SYNC2:
            mPeriod = SPECTRUM_SYNC_SGN2;
            mStage = STAGE::DATA;
            return LOW;
        case STAGE::DATA:
            if (mMeanderUp) {
                if (0 == mLengthOut && 0 == mMask) {
                    if (!moveData()) {
                        mPeriod = SPECTRUM_SYNC_SGN3;
                        mStage = STAGE::FINAL1;
                        return HIGH;
                    }
                }
                mCurrentBitOne = getBit();
            }
            mPeriod = mCurrentBitOne ? SPECTRUM_BIT_ONE : SPECTRUM_BIT_ZERO;
            mMeanderUp = !mMeanderUp;
            return !mMeanderUp;
        case STAGE::FINAL1:
            mStage = STAGE::FINAL2;
            return LOW;
        case STAGE::FINAL2:
            mStage = STAGE::FINISH;
            return LOW;
        case STAGE::FINISH:
            init();
            return LOW;
        default:
            return LOW;
    }
}

float BlockHandler::getPeriod() {
    return static_cast<float>(mPeriod) / Z80_FRQ_Hz * 2;
}

bool BlockHandler::isFinished() {
    return STAGE::FINISH == mStage;
}
