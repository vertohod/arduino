#ifndef EXTENSION_H
#define EXTENSION_H

#include <Arduino.h>

typedef const char extension_t[5];

const extension_t extensions[] PROGMEM = {".TAP", ".TZX", ".P", ".O", ".AY", ".UEF", ".BMP"};

#define EXTENSION_INDEX_TAP     0
#define EXTENSION_INDEX_TZX     1
#define EXTENSION_INDEX_P       2
#define EXTENSION_INDEX_O       3
#define EXTENSION_INDEX_AY      4
#define EXTENSION_INDEX_UEF     5
#define EXTENSION_INDEX_BMP     6

#endif
