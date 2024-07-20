#ifndef TZXFILE_H
#define TZXFILE_H

#include <Arduino.h>

#include "TZXDescription.h"
#include "SD.h"

class TZXFile {
private:
    File       &mFile;

public:
    TZXFile(File& file);
    bool readHeader();

};

#endif
