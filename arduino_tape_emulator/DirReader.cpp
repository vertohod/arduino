#include "Definitions.h"
#include "DirReader.h"
#include "BMPDrawer.h"
#include "Functions.h"

DirReader::DirReader(SD& sd) : mSD(sd) {
}

uint8_t DirReader::getFileNameList(const char* path, uint16_t position, tFileNameList& fileNameList) {
    // Open
    auto directory = mSD.open(path);
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
    char fileName[100];
    for (uint16_t counter = 0; counter < position;) {
        auto file = directory.openNextFile();
        if (!file) {
            return false;
        }
        file.getName(&fileName[0], sizeof(fileName));
        if (!Functions::isItBMPFile(&fileName[0])) {
            ++counter;
        }
        file.close();
    }
    return true;
}

uint8_t DirReader::readFiles(File& directory, tFileNameList& fileNameList, bool addTwoPoints) {
    char fileName[100];
    uint8_t counter = 0;
    if (addTwoPoints) {
        strcpy_P(&fileNameList[counter].fileName[0], TWO_POINTS);
        fileNameList[counter].fileSize = 0;
        ++counter;
    }
    while (counter < MENU_LENGTH) {
        auto file = directory.openNextFile();
        if (!file) {
            return counter;
        }
        file.getName(&fileName[0], sizeof(fileName));
        if (Functions::isItBMPFile(&fileName[0])) {
            file.close();
            continue;
        }
        const char* fileNameSrc = &fileName[0];
        auto fileNameSrcSize = strlen(fileNameSrc);
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
