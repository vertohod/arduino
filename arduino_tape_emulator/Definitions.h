#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// Main
#define TFT_CS              7
#define TFT_DC              9
#define SD_CS               10
#define OUTPUT_PIN          4
#define DURATION_PAUSE      2.0 // seconds
#define ROOT                PSTR("/")

#define TEXT_SIZE           2
#define SYMBOL_HEIGHT       8 
#define SYMBOL_WIDTH        6
#define MARGIN_MUL          0.5
#define FILENAME_LENGTH     14
#define TAPE_BUFFER_SIZE    64
#define TWO_POINTS          PSTR("..")
#define TEXT_PAUSED         PSTR("PAUSED")

// BMPDrawer
#define PALETTE_SIZE        16
#define IMAGE_BUFFER_SIZE   16
#define TEXT_BYTES          PSTR("Bytes")

// DirReader
#define SLASH               '/'
#define MENU_LENGTH         7

// Menu
#define KILOBYTE            PSTR("KB")
#define MENU_TIMEOUT        1000000

// MenuDrawer
#define OUTPUT_ITEM_LENGTH  20
#define OUTPUT_PATH_LENGTH  15
#define THREE_POINTS        PSTR("...")

#endif
