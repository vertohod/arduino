#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

#include "Definitions.h"

typedef uint8_t byte;
typedef char tPath[(FILENAME_LENGTH + 1) * 2];
typedef char tFileName[FILENAME_LENGTH]; 
typedef bool level;

// Look at Extensions.h
enum tFileType : uint8_t {
    TAP,
    TZX,
    P,
    O,
    AY,
    UEF,
    UNKNOWN
};

struct tSignalSettings {
    uint16_t        mPilotHeaderTicks;
    uint16_t        mPilotDataTicks;
    uint16_t        mPeriodPilot;
    uint16_t        mSyncSGN1;
    uint16_t        mSyncSGN2;
    uint16_t        mSyncSGN3;
    uint16_t        mPeriodBitOne;
    uint16_t        mPeriodBitZero;
    uint8_t         mUsedBits;
    uint8_t         mPulseSequence;
};

struct tRange {
    uint32_t        mBegin;
    uint32_t        mEnd;
    uint32_t        mFileSize;
    tRange() : mBegin(0), mEnd(0), mFileSize(0) {}
    tRange(uint32_t begin,  uint32_t end, uint32_t fileSize)
        : mBegin(begin)
        , mEnd(end)
        , mFileSize(fileSize) {}
};

#endif
