#ifndef DIR_READER_H
#define DIR_READER_H

#include <SPI.h>
#include <SD.h>
#include "Types.h"
#include "list.h"
#include "string.h"
#include "IDataProvider.h"

class DirReader : public IDataProvider
{
private:
    string          mPaht;
    size_t          mPosition;
    size_t          mLastAmount;
    File            mDirectory;

// IDataProvider
    size_t          mSizeDataSet;

public:
    DirReader(uint8_t SDPin) : mPosition(0), mLastAmount(0), mSizeDataSet(0)
    {
        Serial.println("Initializing SD card...");
        if (!SD.begin(SDPin)) {
            while(true);
        }
    }
    ~DirReader()
    {
    }
    void setDirectory(const string& path)
    {
        mPaht = path;
    }
    tListString* readNext(size_t amount)
    {
        if (!openDirectory()) {
            return nullptr;
        }
        if (!skipFiles(mPosition)) {
            closeDirectory();
            return nullptr;
        }
        auto result = readFiles(amount);
        if (result) {
            mLastAmount = result->size();
            mPosition += mLastAmount;
        }
        closeDirectory();
        return result;
    }
    tListString* readPrev(size_t amount)
    {
        if (!openDirectory()) {
            return nullptr;
        }
        auto diff = min(mPosition, mLastAmount + amount);
        auto position = mPosition - diff;
        if (!skipFiles(position)) {
            closeDirectory();
            return nullptr;
        }
        auto result = readFiles(amount);
        if (result) {
            mLastAmount = result->size();
            mPosition = position + mLastAmount;
        }
        closeDirectory();
        return result;
    }

// IDataProvider
    void setSizeDataSet(size_t size) override
    {
        mSizeDataSet = size;
    }
    tListString* next() override
    {
        return readNext(mSizeDataSet);
    }
    tListString* prev() override
    {
        return readPrev(mSizeDataSet);
    }

private:
    bool openDirectory() {
        mDirectory = SD.open(mPaht.c_str());
        return mDirectory;
    }
    bool closeDirectory() {
        if (mDirectory) {
            mDirectory.close();
        }
    }
    bool skipFiles(size_t amount) {
        for (size_t i = 0; i < amount; ++i) {
            auto file = mDirectory.openNextFile();
            if (!file) {
                return false;
            }
            file.close();
        }
        return true;
    }
    tListString* readFiles(size_t amount)
    {
        auto fileList = new tListString();
        for (size_t i = 0; i < amount; ++i) {
            auto file = mDirectory.openNextFile();
            if (!file) {
                break;
            }
            if (file.isDirectory()) {
                fileList->push(string(file.name()) + "/");
            } else {
                fileList->push(file.name());
            }
            file.close();
        }
        return fileList;
    }
};

#endif
