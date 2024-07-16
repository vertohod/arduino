#ifndef BLOCK_HANDLER_H
#define BLOCK_HANDLER_H

#include "Types.h"

#define BUFFER_SIZE 16

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
    byte        mBufferIn[BUFFER_SIZE];
    byte        mBufferOut[BUFFER_SIZE];
    uint8_t     mLengthIn;
    uint8_t     mLengthOut;

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

    uint8_t         mIndexByte;
    byte            mMask;
    byte            mCurrentByte;

    STAGE           mStage;
    bool            mCurrentBitOne;
    bool            mMeanderUp;
    uint16_t        mPeriod;

    uint16_t        mImpulseCouter;

public:
    BlockHandler(const char*);

    bool isBufferEmpty();
    void fillBuffer(const byte* const buffer, uint16_t length);

private:
    void init();
    bool moveData();

public:
    void start(byte type);
    void stop();
    bool getBit();
    bool getLevel();
    float getPeriod();
    bool isPilot();
    bool isFinished();
};

#endif
