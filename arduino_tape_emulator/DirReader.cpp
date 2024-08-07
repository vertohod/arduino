#include "DirReader.h"
#include "BMPDrawer.h"

#define SLASH '/'

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
    for (uint16_t counter = 0; counter < position;) {
        auto file = directory.openNextFile();
        if (!file) {
            return false;
        }
        if (!BMPDrawer::isItBMPFile(file.name())) {
            ++counter;
        }
        file.close();
    }
    return true;
}

uint8_t DirReader::readFiles(File& directory, tFileNameList& fileNameList, bool addTwoPoints)
{
    uint8_t counter = 0;
    if (addTwoPoints) {
        memcpy(static_cast<void*>(&fileNameList[counter].fileName[0]), static_cast<const void*>(TWO_POINTS), strlen(TWO_POINTS) + 1);
        fileNameList[counter].fileSize = 0;
        ++counter;
    }
    while (counter < MENU_LENGTH) {
        auto file = directory.openNextFile();
        if (!file) {
            return counter;
        }
        if (BMPDrawer::isItBMPFile(file.name())) {
            file.close();
            continue;
        }
        const char* fileNameSrc = file.name();
        auto fileNameSrcSize = strlen(file.name());
        tFileInfo& fileInfoDist = fileNameList[counter];
        memcpy(static_cast<void*>(&fileInfoDist.fileName[0]), static_cast<const void*>(fileNameSrc), fileNameSrcSize + 1);
        if (file.isDirectory()) {
            fileInfoDist.fileName[fileNameSrcSize] = SLASH;
            fileInfoDist.fileName[fileNameSrcSize + 1] = 0;
            fileInfoDist.fileSize = 0;
        } else {
            fileInfoDist.fileSize = file.size();
        }
        file.close();
        ++counter;
    }
    return counter;
}
