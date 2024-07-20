#include <Arduino.h>

#include "Extensions.h"
#include "Functions.h"

namespace Functions {

uint16_t getLastPoint(const char* path) {
    uint16_t i = strlen(path);
    while (0 < --i) {
        if (path[i] == '.') {
            return i;
        }
    }
    return 0;
}

bool isItBMPFile(const char *path) {
    auto lastPoint = getLastPoint(path);
    if (0 == strcmp_P(&path[lastPoint], extensions[EXTENSION_INDEX_BMP])) {
        return true;
    }
    return false;
}

tFileType getFileType(const char* path) {
    auto pathLength = strlen(path);
    for (uint8_t i = 0; i < sizeof(extensions); ++i) {
        auto length = strlen_P(extensions[i]);
        if (0 == strcmp_P(&path[pathLength - length], extensions[i])) {
            return static_cast<tFileType>(i);
        }
    }
    return tFileType::UNKNOWN;
}

uint16_t readLE16(File& file) {
    if (!file.available()) {
        return 0;
    }
    return static_cast<uint16_t>(file.read()) | static_cast<uint16_t>(file.read()) << 8;
}

uint32_t readLE32(File& file) {
    return static_cast<uint32_t>(readLE16(file)) | static_cast<uint32_t>(readLE16(file)) << 16;
}

} //namespace Functions
