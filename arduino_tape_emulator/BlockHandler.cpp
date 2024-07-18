#include "BlockHandler.h"

BlockHandler::BlockHandler(const char*)
{
    init();
}

bool BlockHandler::isBufferEmpty()
{
    return 0 == mLengthIn;
}

void BlockHandler::fillBuffer(const byte* const buffer, uint16_t length)
{
    memcpy(static_cast<void*>(&mBufferIn[0]), static_cast<const void*>(buffer), length);
    mLengthIn = length;
}

void BlockHandler::init()
{
    mLengthIn = 0;
    mLengthOut = 0;
    mIndexByte = 0;
    mMask = 0;
    mStage = STAGE::FINISH;
    mMeanderUp = true;
    mPeriod = 0;
}

bool BlockHandler::moveData()
{
    if (mLengthIn == 0) return false;

    memcpy(static_cast<void*>(&mBufferOut[0]), static_cast<const void*>(&mBufferIn[0]), mLengthIn);
    mLengthOut = mLengthIn;
    mIndexByte = 0;
    mMask = 0;
    mLengthIn = 0;

    return true;
}

void BlockHandler::start(byte type)
{
    mStage = STAGE::PILOT;
    mImpulseCouter = (0 == type ? PILOT_HEADER_IMPULSES : PILOT_DATA_IMPULSES);
}

void BlockHandler::stop()
{
    mStage = STAGE::FINISH;
}

bool BlockHandler::getBit()
{
    if (0 == mMask) {
        mMask = 0x80;
        mCurrentByte = mBufferOut[mIndexByte++];
        if (mIndexByte == mLengthOut) mLengthOut = 0;
    }
    bool result = mCurrentByte & mMask;
    mMask >>= 1;
    return result;
}

bool BlockHandler::getLevel()
{
    switch (mStage) {
        case STAGE::PILOT:
            mPeriod = PILOT_SGN;
            if (!mMeanderUp) {
                --mImpulseCouter;
                if (0 == mImpulseCouter) mStage = STAGE::SYNC1;
            }
            mMeanderUp = !mMeanderUp;
            return !mMeanderUp;
        case STAGE::SYNC1:
            mPeriod = SYNC_SGN1;
            mStage = STAGE::SYNC2;
            return true;
        case STAGE::SYNC2:
            mPeriod = SYNC_SGN2;
            mStage = STAGE::DATA;
            mMeanderUp = true;  // for any case
            return false;
        case STAGE::DATA:
            if (mMeanderUp) {
                if (0 == mLengthOut && 0 == mMask) {
                    if (!moveData()) {
                        mPeriod = SYNC_SGN3;
                        mStage = STAGE::FINAL1;
                        return true;
                    }
                }
                mCurrentBitOne = getBit();
            }
            mPeriod = mCurrentBitOne ? LG1_SGN : LG0_SGN;
            mMeanderUp = !mMeanderUp;
            return !mMeanderUp;
        case STAGE::FINAL1:
            mStage = STAGE::FINAL2;
            return false;
        case STAGE::FINAL2:
            mStage = STAGE::FINISH;
            return false;
        case STAGE::FINISH:
            init();
            return false;
        default:
            return false;
    }
}

float BlockHandler::getPeriod()
{
    return static_cast<float>(mPeriod) / Z80_FRQ_Hz * 2;
}

bool BlockHandler::isPilot()
{
    return STAGE::PILOT == mStage;
}

bool BlockHandler::isFinished()
{
    return STAGE::FINISH == mStage;
}
