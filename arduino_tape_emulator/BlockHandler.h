#ifndef BLOCK_HANDLER_H
#define BLOCK_HANDLER_H

#include "Types.h"

class BlockHandler {
private:
    byte            mBuffer0[TAPE_BUFFER_SIZE];
    byte            mBuffer1[TAPE_BUFFER_SIZE];
    byte            mBuffer2[TAPE_BUFFER_SIZE];

    byte            *mBufferExternalPtr;
    byte            *mBufferInPtr;
    byte            *mBufferOutPtr;

    uint8_t         mLengthIn;
    uint8_t         mLengthOut;

    enum STATE {
        PILOT,
        SYNC1,
        SYNC2,
        DATA,
        SYNC3,
        FINAL1,
        FINAL2,
        FINISH
    };

    tSignalSettings *mSignalSettings;

    uint8_t         mIndexByte;
    byte            mMask;
    byte            mCurrentByte;

    STATE           mState;
    bool            mCurrentBitOne;
    level           mMeanderUp;
    uint16_t        mPeriod;

    uint16_t        mImpulseCouter;

public:
    BlockHandler();

    bool isBufferEmpty();
    void fillBuffer(byte *buffer, uint16_t length);
    void setSignalSettings(tSignalSettings *signalSettings);

private:
    bool moveData();
    bool getBit();

public:
    void init();
    void start(byte type);
    level getLevel();
    float getPeriod();
    bool isFinished();
    byte* getExternalBuffer();
};

#endif
