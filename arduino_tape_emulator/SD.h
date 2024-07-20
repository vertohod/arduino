#ifndef SD_H
#define SD_H

#define SPI_SPEED                   SPI_FULL_SPEED
#define SD_INCLUDE_EXFAT_SUPPORT    0
#define SD_FAT_TYPE                 1

#include <Arduino.h>

// It works with version 2.1.1
#include <SdFat.h>

typedef SdFat SD;

#endif
