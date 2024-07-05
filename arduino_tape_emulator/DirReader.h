#ifndef DIR_READER_H
#define DIR_READER_H

#include "Arduino.h"
#include "SPI.h"
#include "SD.h"

#define MENU_LENGTH 7
#define FILENAME_LENGTH 14

typedef char tFileName[FILENAME_LENGTH]; 

struct tFileInfo {
    tFileName   fileName;
    uint32_t    fileSize;
};

typedef tFileInfo tFileNameList[MENU_LENGTH];

class DirReader {
public:
    uint8_t getFileNameList(const char* path, uint16_t position, tFileNameList& fileNameList);

private:
    bool skipFiles(File& directory, uint16_t position);
    uint8_t readFiles(File& directory, tFileNameList& fileNameList, bool addTwoPoints);
};

#endif
