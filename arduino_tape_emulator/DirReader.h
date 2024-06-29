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
    File            mDirectory;
    uint8_t         mSizeDataSet;

public:
    DirReader(uint8_t SDPin) : mSizeDataSet(0)
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
    tListString* read(uint16_t position)
    {
        if (!openDirectory()) {
            return nullptr;
        }
        if (!skipFiles(position)) {
            closeDirectory();
            return nullptr;
        }
        auto result = readFiles(mSizeDataSet);
        closeDirectory();
        return result;
    }

// IDataProvider
    void setSizeDataSet(uint8_t size) override
    {
        mSizeDataSet = size;
    }
    tListString* getData(uint16_t position) override
    {
        return read(position);
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
        if (nullptr == fileList) {
            Serial.print(F("DirReader::readFiles, fileList is nullptr"));
            return nullptr;
        }
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
