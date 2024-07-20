#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

#include "Definitions.h"

typedef uint8_t byte;
typedef char tPath[(FILENAME_LENGTH + 1) * 2];
typedef char tFileName[FILENAME_LENGTH]; 
typedef bool level;

// Look at Extensions.h
enum tFileType : uint8_t {
    TAP,
    TZX,
    P,
    O,
    AY,
    UEF,
    UNKNOWN
};

#endif
