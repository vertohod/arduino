#ifndef BLOCK_HANDLER_H
#define BLOCK_HANDLER_H

#include "Types.h"

// Signal duration in ticks
#define PILOT_SGN       2168
#define SYNC_SGN1       667
#define SYNC_SGN2       735
#define SYNC_SGN3       954
#define LG1_SGN         1710
#define LG0_SGN         855

#define PILOT_HEADER_IMPULSES   3228 
#define PILOT_DATA_IMPULSES     1614 

#define Z80_FRQ_Hz 3500000

class BlockHandler
{
private:
    volatile byte*  mBufferIn;
    volatile size_t mLengthIn;
    volatile byte*  mBufferOut;
    volatile size_t mLengthOut;

    enum STAGE
    {
        PILOT,
        SYNC1,
        SYNC2,
        DATA,
        SYNC3,
        FINAL1,
        FINAL2,
        FINISH
    };

    size_t          mIndexByte;
    byte            mMask;
    byte            mCurrentByte;

    volatile STAGE  mStage;
    bool            mCurrentBitOne;
    bool            mMeanderUp;
    size_t          mPeriod;

    size_t          mImpulseCouter;

public:
    BlockHandler(size_t bufferSize) : mStage(STAGE::FINISH)
    {
        mBufferIn = new byte[bufferSize];
        mBufferOut = new byte[bufferSize];
        init();
    }

    ~BlockHandler()
    {
        delete[] mBufferOut;
        delete[] mBufferIn;
    }

    bool isBufferEmpty()
    {
        return 0 == mLengthIn;
    }

    volatile byte* fillBuffer(volatile byte* buffer, size_t length)
    {
        volatile byte* old_buffer = mBufferIn;
        mBufferIn = buffer;
        mLengthIn = length;
        return old_buffer;
    }

private:
    void init()
    {
        mLengthIn = 0;
        mLengthOut = 0;
        mIndexByte = 0;
        mMask = 0;
        mCurrentBitOne = false;
        mMeanderUp = true;
        mPeriod = 0;
    }

    bool moveData()
    {
        if (mLengthIn == 0) return false;

        auto temp = mBufferOut;
        mBufferOut = mBufferIn;
        mLengthOut = mLengthIn;
        mBufferIn = temp;
        mLengthIn = 0;

        return true;
    }

public:
    void start(byte type)
    {
        mStage = STAGE::PILOT;
        mImpulseCouter = (0 == type ? PILOT_HEADER_IMPULSES : PILOT_DATA_IMPULSES);
    }

    void stop()
    {
        mStage = STAGE::FINISH;
    }

    bool getBit()
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

    bool getLevel()
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
                        if (moveData()) {
                            mIndexByte = 0;
                            mMask = 0;
                        } else {
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
    double getPeriod()
    {
        return static_cast<double>(mPeriod) / Z80_FRQ_Hz * 2;
    }
    bool isPilot()
    {
        return STAGE::PILOT == mStage;
    }
    bool isFinished()
    {
        return STAGE::FINISH == mStage;
    }
};

#endif
