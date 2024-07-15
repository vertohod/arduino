#ifndef BMPDRAWER_H
#define BMPDRAWER_H

#include "Arduino.h"
#include "Adafruit_ILI9341.h"
#include "SD.h"

void drawBMP(Adafruit_ILI9341 *screenPtr, const char *path);

class BMPDrawer {
private:
    Adafruit_ILI9341 *mScreenPtr;
    File mFile;

public:
    BMPDrawer(Adafruit_ILI9341 *screenPtr);

    void draw(const char *path, int16_t xPosition, int16_t yPosition);

private:
    void drawPixel(uint16_t xPosition, uint16_t yPosition, uint16_t color);
    uint16_t readLE16();
    uint32_t readLE32();
};

#endif
