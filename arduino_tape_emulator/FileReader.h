#ifndef FILE_READER_H
#define FILE_READER_H

#include "BlockHandler.h"
#include "TZXBlock.h"
#include "Types.h"
#include "SD.h"

class FileReader {
private:
    tFileType       mFileType;
    File            mFile;
    byte            mBlockType;
    byte            mBlockTypeKnown;
    uint16_t        mBlockSize;
    uint16_t        mBlockRead;
    uint32_t        mLastBlock;

    enum STATE {
        READING,
        PAUSE,
        END
    };
    STATE           mState;

    TZXBlock        mTZXBlock;
    BlockHandler    mBlockHandler;
    tRange          mRange;

public:
    FileReader(SD& sd, const char* fileName);
    ~FileReader();

    uint32_t getFilePosition();
    uint32_t getFileSize();
    void setPause();
    bool isPause();
    void readContinue(bool lastBlock = false);
    bool isFinished();
    bool isBlockHandlerFinished();
    void setNextBlock();
    void setPreviousBlock();

    level getLevel();
    float getPeriod();
    bool start();
    void moveData();

private:
    void prepearData();
    uint16_t getData(byte *buffer, uint16_t buffer_size);
    byte getBlockType();
    uint16_t processData(byte *buffer, uint16_t bufferSize);
    void processFile();
    tRange processFileTAP();
    tRange processFileTZX();
};

#endif
