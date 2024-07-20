#ifndef BLOCK_HANDLER_H
#define BLOCK_HANDLER_H

#include "Types.h"

class BlockHandler {
private:
    byte        mBufferIn[TAPE_BUFFER_SIZE];
    byte        mBufferOut[TAPE_BUFFER_SIZE];
    uint8_t     mLengthIn;
    uint8_t     mLengthOut;

    enum STAGE {
        PILOT,
        SYNC1,
        SYNC2,
        DATA,
        SYNC3,
        FINAL1,
        FINAL2,
        FINISH
    };

    uint8_t         mIndexByte;
    byte            mMask;
    byte            mCurrentByte;

    STAGE           mStage;
    bool            mCurrentBitOne;
    level           mMeanderUp;
    uint16_t        mPeriod;

    uint16_t        mImpulseCouter;

public:
    BlockHandler(const char*);

    bool isBufferEmpty();
    void fillBuffer(const byte* const buffer, uint16_t length);

private:
    bool moveData();
    bool getBit();

public:
    void init();
    void start(byte type);
    level getLevel();
    float getPeriod();
    bool isFinished();
};

#endif
