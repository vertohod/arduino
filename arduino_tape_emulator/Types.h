#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

#define TEXT_SIZE           2
#define SYMBOL_HEIGHT       8 
#define SYMBOL_WIDTH        6
#define MARGIN_MUL          0.5
#define FILENAME_LENGTH     14
#define EXTENSION_BMP       "BMP"
#define TAPE_BUFFER_SIZE    64

typedef uint8_t byte;
typedef char tPath[(FILENAME_LENGTH + 1) * 2];
typedef char tFileName[FILENAME_LENGTH]; 

#endif
