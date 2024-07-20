#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Types.h"
#include "SD.h"

namespace Functions {

uint16_t getLastPoint(const char* path);

bool isItBMPFile(const char *path);
tFileType getFileType(const char* path);

uint16_t readLE16(File& file);
uint32_t readLE32(File& file);

} //namespace Functions

#endif
