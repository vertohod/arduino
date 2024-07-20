#include "BlockHandler.h"
#include "Timings.h"

BlockHandler::BlockHandler()
    : mBufferExternalPtr(&mBuffer0[0])
    , mBufferInPtr(&mBuffer1[0])
    , mBufferOutPtr(&mBuffer2[0])
    , mSignalSettings(nullptr)
{
    init();
}

bool BlockHandler::isBufferEmpty() {
    return 0 == mLengthIn;
}

void BlockHandler::fillBuffer(byte *buffer, uint16_t length) {
    // mBufferInPtr <- mBufferExternalPtr 
    // mBufferInPtr -> mBufferExternalPtr 
    mBufferExternalPtr = mBufferInPtr;
    mBufferInPtr = buffer;
    mLengthIn = length;
}

void BlockHandler::setSignalSettings(tSignalSettings *signalSettings) {
    mSignalSettings = signalSettings;
}

void BlockHandler::init() {
    mLengthIn = 0;
    mLengthOut = 0;
    mIndexByte = 0;
    mMask = 0;
    mState = STATE::FINISH;
    mPeriod = 0;
}

bool BlockHandler::moveData() {
    if (0 == mLengthIn) return false;

    // mBufferInPtr -> mBufferOutPtr
    // mBufferInPtr <- mBufferOutPtr
    auto tempPtr = mBufferOutPtr;
    mBufferOutPtr = mBufferInPtr;
    mBufferInPtr = tempPtr;
    mLengthOut = mLengthIn;
    mLengthIn = 0;
    mIndexByte = 0;
    mMask = 0;

    return true;
}

void BlockHandler::start(byte type) {
    mState = STATE::PILOT;
    mImpulseCouter = (0 == type ? mSignalSettings->mPilotHeaderTicks : mSignalSettings->mPilotDataTicks);
}

bool BlockHandler::getBit() {
    if (0 == mMask) {
        mMask = 0x80;
        mCurrentByte = mBufferOutPtr[mIndexByte++];
        if (mIndexByte == mLengthOut) mLengthOut = 0;
    }
    bool result = mCurrentByte & mMask;
    mMask >>= 1;
    return result;
}

level BlockHandler::getLevel() {
    switch (mState) {
        case STATE::PILOT:
            mPeriod = mSignalSettings->mPeriodPilot;
            if (!mMeanderUp) {
                --mImpulseCouter;
                if (0 == mImpulseCouter) mState = STATE::SYNC1;
            }
            mMeanderUp = !mMeanderUp;
            return !mMeanderUp;
        case STATE::SYNC1:
            mPeriod = mSignalSettings->mSyncSGN1;
            mState = STATE::SYNC2;
            return HIGH;
        case STATE::SYNC2:
            mPeriod = mSignalSettings->mSyncSGN2;
            mState = STATE::DATA;
            return LOW;
        case STATE::DATA:
            if (mMeanderUp) {
                if (0 == mLengthOut && 0 == mMask) {
                    if (!moveData()) {
                        mPeriod = mSignalSettings->mSyncSGN3;
                        mState = STATE::FINAL1;
                        return HIGH;
                    }
                }
                mCurrentBitOne = getBit();
            }
            mPeriod = mCurrentBitOne ? mSignalSettings->mPeriodBitOne : mSignalSettings->mPeriodBitZero;
            mMeanderUp = !mMeanderUp;
            return !mMeanderUp;
        case STATE::FINAL1:
            mState = STATE::FINAL2;
            return LOW;
        case STATE::FINAL2:
            mState = STATE::FINISH;
            return LOW;
        case STATE::FINISH:
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
    return STATE::FINISH == mState;
}

byte* BlockHandler::getExternalBuffer() {
    return mBufferExternalPtr;
}

