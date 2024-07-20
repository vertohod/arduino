#ifndef BMPDRAWER_H
#define BMPDRAWER_H

#include <Adafruit_ILI9341.h>
#include <Arduino.h>

#include "SD.h"

void drawBMP(SD &sd, Adafruit_ILI9341 &screen, const char *path, bool &isNeededToLoad);
void BMPDrawerInt0Handler();
void BMPDrawerInt1Handler();

class BMPDrawer {
private:
    SD                  &mSD;
    Adafruit_ILI9341    &mScreen;
    File                mFile;

public:
    BMPDrawer(SD& sd, Adafruit_ILI9341 &screen);

    void drawFileInfo(const char *path); 
    void draw(const char *path, int16_t xPosition, int16_t yPosition);
    void drawProgressBar();
    static uint16_t discreteNumber(uint16_t number); 
    static void drawProgress(Adafruit_ILI9341 &screen, bool withClear, float progress);
    static void drawPause(Adafruit_ILI9341 &screen);
    static void cleanPause(Adafruit_ILI9341 &screen);
};

#endif
