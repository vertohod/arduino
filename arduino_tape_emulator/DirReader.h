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
    uint8_t         mPosition;
    uint8_t         mLastAmount;
    File            mDirectory;

// IDataProvider
    uint8_t         mSizeDataSet;

public:
    DirReader(uint8_t SDPin) : mPosition(0), mLastAmount(0), mSizeDataSet(0)
    {
        Serial.println(F("(DirReader) Initializing SD card..."));
        if (!SD.begin(SDPin)) {
            while(true);
        }
        Serial.println(F("(DirReader) SD card is initialized"));
    }
    void setDirectory(const string& path)
    {
        mPaht = path;
    }
    tListString* readNext(uint8_t amount)
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
    tListString* readPrev(uint8_t amount)
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
    void setSizeDataSet(uint8_t size) override
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
    bool skipFiles(uint8_t amount) {
        for (uint8_t i = 0; i < amount; ++i) {
            auto file = mDirectory.openNextFile();
            if (!file) {
                return false;
            }
            file.close();
        }
        return true;
    }
    tListString* readFiles(uint8_t amount)
    {
        auto fileList = new tListString();
        for (uint8_t i = 0; i < amount; ++i) {
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
