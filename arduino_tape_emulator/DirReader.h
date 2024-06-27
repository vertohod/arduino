#ifndef DIR_READER_H
#define DIR_READER_H

#include <SPI.h>
#include <SD.h>
#include "Types.h"
#include "list.h"
#include "string.h"

typedef list<string> t_file_list;

class IDataProvider;

class DirReader// : public IDataProvider
{
private:
    string          mPaht;
    size_t          mPosition;
    size_t          mLastAmount;
    File            mDirectory;

public:
    DirReader(uint8_t SDPin) : mPosition(0), mLastAmount(0)
    {
        Serial.println("Initializing SD card...");
        if (!SD.begin(SDPin)) {
            while(true);
        }
    }
    ~DirReader()
    {
    }
    void setDirectory(const char* path)
    {
        mPaht = path;
    }
    t_file_list* readNext(size_t amount)
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
    t_file_list* readPrev(size_t amount)
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
    t_file_list* readFiles(size_t amount)
    {
        auto fileList = new t_file_list();
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
