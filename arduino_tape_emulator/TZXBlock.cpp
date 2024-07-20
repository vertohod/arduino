#include "TZXDescription.h"
#include "Functions.h"
#include "TZXBlock.h"
#include "Timings.h"

bool TZXBlock::readHeader(File& file) {
    byte buffer[TZX_HEADER_LENGTH];
    file.read(&buffer[0], TZX_HEADER_LENGTH);
    // TODO
    // Check header
    return true;
}

bool TZXBlock::readBlock(File& file) {
    if (!readBlockID(file)) {
        return false;
    }
    switch (mCurrentBlockID) {
        case TZX_ID10:
            return readBlockData10(file);
        case TZX_ID11:
            return readBlockData11(file);
        case TZX_ID12:
            return readBlockData12(file);
        case TZX_ID13:
            return readBlockData13(file);
        case TZX_ID14:
            return readBlockData14(file);
        case TZX_ID15:
            return readBlockData15(file);
        case TZX_ID19:
            return readBlockData19(file);
        case TZX_ID20:
            return readBlockData20(file);
        case TZX_ID21:
            return readBlockData21(file);
        case TZX_ID22:
            return readBlockData22(file);
        case TZX_ID23:
            return readBlockData23(file);
        case TZX_ID24:
            return readBlockData24(file);
        case TZX_ID25:
            return readBlockData25(file);
        case TZX_ID26:
            return readBlockData26(file);
        case TZX_ID27:
            return readBlockData27(file);
        case TZX_ID28:
            return readBlockData28(file);
        case TZX_ID2A:
            return readBlockData2A(file);
        case TZX_ID2B:
            return readBlockData2B(file);
        case TZX_ID30:
            return readBlockData30(file);
        case TZX_ID31:
            return readBlockData31(file);
        case TZX_ID32:
            return readBlockData32(file);
        case TZX_ID33:
            return readBlockData33(file);
        case TZX_ID35:
            return readBlockData35(file);
        case TZX_ID4B:
            return readBlockData4B(file);
        case TZX_ID5A:
            return readBlockData5A(file);
        default:
            return false;
    }
}

void TZXBlock::setDefaultSettings() {
    mSignalSettings.mPilotHeaderTicks = SPECTRUM_PILOT_HEADER;
    mSignalSettings.mPilotDataTicks = SPECTRUM_PILOT_DATA;
    mSignalSettings.mPeriodPilot = SPECTRUM_PERIOD_PILOT;
    mSignalSettings.mSyncSGN1 = SPECTRUM_SYNC_SGN1;
    mSignalSettings.mSyncSGN2 = SPECTRUM_SYNC_SGN2;
    mSignalSettings.mSyncSGN3 = SPECTRUM_SYNC_SGN3;
    mSignalSettings.mPeriodBitZero = SPECTRUM_BIT_ZERO;
    mSignalSettings.mPeriodBitOne = SPECTRUM_BIT_ONE;
    mSignalSettings.mUsedBits = SPECTRUM_USED_BITS;
}

tSignalSettings* TZXBlock::getSignalSettings() {
    return &mSignalSettings;
}

bool TZXBlock::readBlockID(File& file) {
    if (!file.available()) {
        mCurrentBlockID = TZX_EOF;
        return false;
    }
    mCurrentBlockID = file.read();
    return true; 
}

bool TZXBlock::readBlockData10(File& file) {
    mPauseAfterBlock = Functions::readLE16(file);
    mBlockDataLength = Functions::readLE16(file);

    setDefaultSettings();

    return true; 
}

bool TZXBlock::readBlockData11(File& file) {
    mSignalSettings.mPeriodPilot = Functions::readLE16(file);
    mSignalSettings.mSyncSGN1 = Functions::readLE16(file);
    mSignalSettings.mSyncSGN2 = Functions::readLE16(file);
    mSignalSettings.mPeriodBitZero = Functions::readLE16(file);
    mSignalSettings.mPeriodBitOne = Functions::readLE16(file);
    mSignalSettings.mPilotHeaderTicks = Functions::readLE16(file);
    mSignalSettings.mPilotDataTicks = mSignalSettings.mPilotHeaderTicks;
    mSignalSettings.mUsedBits = file.read();

    mPauseAfterBlock = Functions::readLE16(file);
    mBlockDataLength = Functions::readLE16(file) | static_cast<uint32_t>(file.read()) << 16;

    mSignalSettings.mPulseSequence = 0;

    return true; 
}

bool TZXBlock::readBlockData12(File& file) {
    mSignalSettings.mPeriodPilot = Functions::readLE16(file);
    mSignalSettings.mPilotHeaderTicks = Functions::readLE16(file);
    mSignalSettings.mPilotDataTicks = mSignalSettings.mPilotHeaderTicks;

    mBlockDataLength = 0;
    mSignalSettings.mPulseSequence = 0;

    return true; 
}

bool TZXBlock::readBlockData13(File& file) {
    mSignalSettings.mPulseSequence = file.read();

    mSignalSettings.mPilotHeaderTicks = 0;
    mSignalSettings.mPilotDataTicks = 0;
    mBlockDataLength = 0;

    return true; 
}

bool TZXBlock::readBlockData14(File& file) {
    mSignalSettings.mPeriodBitZero = Functions::readLE16(file);
    mSignalSettings.mPeriodBitOne = Functions::readLE16(file);
    mSignalSettings.mUsedBits = file.read();

    mPauseAfterBlock = Functions::readLE16(file);
    mBlockDataLength = Functions::readLE16(file) | static_cast<uint32_t>(file.read()) << 16;

    mSignalSettings.mPilotHeaderTicks = 0;
    mSignalSettings.mPilotDataTicks = mSignalSettings.mPilotHeaderTicks;
    mSignalSettings.mPulseSequence = 0;

    return true; 
}

bool TZXBlock::readBlockData15(File& file) {
    mBlockDataBits = Functions::readLE16(file);
    mPauseAfterBlock = Functions::readLE16(file);
    mSignalSettings.mUsedBits = file.read();
    mBlockDataLength = Functions::readLE16(file) | static_cast<uint32_t>(file.read()) << 16;

    mSignalSettings.mPilotHeaderTicks = 0;
    mSignalSettings.mPilotDataTicks = mSignalSettings.mPilotHeaderTicks;

    return true; 
}

bool TZXBlock::readBlockData19(File& file) {
    // TODO
    return true; 
}

bool TZXBlock::readBlockData20(File& file) {
    mPauseAfterBlock = Functions::readLE16(file);
    mHandlerStage = TZXHandlerStage::PAUSE;
    return true; 
}

bool TZXBlock::readBlockData21(File& file) {
    // TODO
    byte length = file.read();
    for (byte i = 0; i < length; ++i) {
        file.read();
    } 
    mFileStage = TZXFileStage::GET_ID;
    return true; 
}

bool TZXBlock::readBlockData22(File& file) {
    mFileStage = TZXFileStage::GET_ID;
    return true; 
}

bool TZXBlock::readBlockData23(File& file) {
    // TODO
    Functions::readLE16(file);
    mFileStage = TZXFileStage::GET_ID;
    return true; 
}

bool TZXBlock::readBlockData24(File& file) {
    mLoopCounter = Functions::readLE16(file);
    mLoopStartPosition = file.position();
    mFileStage = TZXFileStage::GET_ID;

    return true; 
}

bool TZXBlock::readBlockData25(File& file) {
    if (0 < mLoopCounter--) {
        file.seek(mLoopStartPosition);
    }
    mFileStage = TZXFileStage::GET_ID;

    return true; 
}

bool TZXBlock::readBlockData26(File& file) {
    // TODO
    uint16_t length = Functions::readLE16(file);
    for (uint16_t i = 0; i < length; ++i) {
        Functions::readLE16(file);
    } 
    mFileStage = TZXFileStage::GET_ID;
    return true; 
}

bool TZXBlock::readBlockData27(File& file) {
    // TODO
    mFileStage = TZXFileStage::GET_ID;
    return true; 
}

bool TZXBlock::readBlockData28(File& file) {
    // TODO
    uint16_t length = Functions::readLE16(file);
    file.read();
    for (uint16_t i = 0; i < length; ++i) {
        file.read();
    } 

    return true; 
}

bool TZXBlock::readBlockData2A(File& file) {
    Functions::readLE32(file);
    mHandlerStage = TZXHandlerStage::PAUSE;

    return true; 
}

bool TZXBlock::readBlockData2B(File& file) {
    Functions::readLE32(file);
    mSignalLevel = file.read();

    return true; 
}

bool TZXBlock::readBlockData30(File& file) {
    // TODO
    byte length = file.read();
    for (byte i = 0; i < length; ++i) {
        file.read();
    } 

    return true; 
}

bool TZXBlock::readBlockData31(File& file) {
    // TODO
    file.read();
    byte length = file.read();
    for (byte i = 0; i < length; ++i) {
        file.read();
    } 

    return true; 
}

bool TZXBlock::readBlockData32(File& file) {
    // TODO
    return true; 
}

bool TZXBlock::readBlockData33(File& file) {
    // TODO
    return true; 
}

bool TZXBlock::readBlockData35(File& file) {
    // TODO
    return true; 
}

bool TZXBlock::readBlockData4B(File& file) {
    // TODO
    return true; 
}

bool TZXBlock::readBlockData5A(File& file) {
    // TODO
    return true; 
}
