#ifndef TZXFILE_H
#define TZXFILE_H

#include <Arduino.h>

#include "TZXDescription.h"
#include "Types.h"
#include "SD.h"

class TZXBlock {
private:
    uint32_t        mBlockDataLength;
    uint32_t        mBlockDataBits;
    uint16_t        mPauseAfterBlock;

    tSignalSettings mSignalSettings;

    uint16_t        mLoopCounter;
    uint32_t        mLoopStartPosition;
    byte            mSignalLevel;

    byte            mCurrentBlockID;

    TZXFileStage    mFileStage;
    TZXHandlerStage mHandlerStage;

public:
    bool readHeader(File& file);
    bool readBlock(File& file);
    void setDefaultSettings();
    tSignalSettings* getSignalSettings();

private:
    bool readBlockID(File& file);
    bool readBlockData10(File& file);
    bool readBlockData11(File& file);
    bool readBlockData12(File& file);
    bool readBlockData13(File& file);
    bool readBlockData14(File& file);
    bool readBlockData15(File& file);
    bool readBlockData19(File& file);
    bool readBlockData20(File& file);
    bool readBlockData21(File& file);
    bool readBlockData22(File& file);
    bool readBlockData23(File& file);
    bool readBlockData24(File& file);
    bool readBlockData25(File& file);
    bool readBlockData26(File& file);
    bool readBlockData27(File& file);
    bool readBlockData28(File& file);
    bool readBlockData2A(File& file);
    bool readBlockData2B(File& file);
    bool readBlockData30(File& file);
    bool readBlockData31(File& file);
    bool readBlockData32(File& file);
    bool readBlockData33(File& file);
    bool readBlockData35(File& file);
    bool readBlockData4B(File& file);
    bool readBlockData5A(File& file);
};

#endif
