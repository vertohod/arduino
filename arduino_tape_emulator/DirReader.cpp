#include "DirReader.h"

#define SLASH '/'
#define TWO_POINTS ".."

uint8_t DirReader::getFileNameList(const char* path, uint16_t position, tFileNameList& fileNameList) {
    // Open
    auto directory = SD.open(path);
    // Skip
    if (!skipFiles(directory, position)) {
        directory.close();
        return 0;
    }
    auto addTwoPoints = strlen(path) > 1 && position == 0;
    auto result = readFiles(directory, fileNameList, addTwoPoints);

    directory.close();
    return result;
}

bool DirReader::skipFiles(File& directory, uint16_t position) {
    for (uint16_t i = 0; i < position; ++i) {
        auto file = directory.openNextFile();
        if (!file) {
            return false;
        }
        file.close();
    }
    return true;
}

uint8_t DirReader::readFiles(File& directory, tFileNameList& fileNameList, bool addTwoPoints)
{
    uint8_t counter = 0;
    if (addTwoPoints) {
        memcpy(static_cast<void*>(&fileNameList[counter][0]), static_cast<const void*>(TWO_POINTS), strlen(TWO_POINTS) + 1);
        ++counter;
    }
    while (counter < MENU_LENGTH) {
        auto file = directory.openNextFile();
        if (!file) {
            return counter;
        }
        char* fileName = file.name();
        memcpy(static_cast<void*>(&fileNameList[counter][0]), static_cast<void*>(fileName), strlen(fileName) + 1);
        if (file.isDirectory()) {
            fileNameList[counter][strlen(fileName)] = SLASH;
            fileNameList[counter][strlen(fileName) + 1] = 0;
        }
        file.close();
        ++counter;
    }
    return counter;
}

uint16_t DirReader::strlen(const char* str) {
    uint16_t i = 0;
    while (str[i] != 0) {
        ++i;
    }
    return i;
}
