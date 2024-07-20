#ifndef FILE_READER_H
#define FILE_READER_H

#include "Types.h"
#include "SD.h"

class FileReader {
private:
    SD          mSD;
    tFileType   mFileType;
    File        mFile;
    byte        mBlockType;
    byte        mBlockTypeKnown;
    uint16_t    mBlockSize;
    uint16_t    mBlockRead;
    uint32_t    mLastBlock;

    enum STATE {
        READING,
        PAUSE,
        END
    };
    STATE       mState;

public:
    FileReader(SD& sd, const char* fileName);
    ~FileReader();

    uint32_t getFilePosition();
    uint32_t getFileSize();
    uint16_t getData(byte *buffer, uint16_t buffer_size);
    byte getBlockType();
    void setPause();
    bool isPause();
    void readContinue(bool lastBlock = false);
    bool isFinished();
    void setNextBlock();
    void setPreviousBlock();

private:
    uint16_t processFileTAP(byte *buffer, uint16_t buffer_size);
    uint16_t processFileTZX(byte *buffer, uint16_t buffer_size);
};

#endif
