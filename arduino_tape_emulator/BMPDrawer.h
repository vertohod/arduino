#ifndef BMPDRAWER_H
#define BMPDRAWER_H

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>

void drawBMP(Adafruit_ILI9341 &screen, const char *path, bool &isNeededToLoad);
void BMPDrawerInt0Handler();
void BMPDrawerInt1Handler();

class BMPDrawer {
private:
    Adafruit_ILI9341 &mScreen;
    File mFile;

public:
    BMPDrawer(Adafruit_ILI9341 &screen);

    void drawFileInfo(const char *path); 
    void draw(const char *path, int16_t xPosition, int16_t yPosition);
    static uint16_t getLastPoint(const char* path);
    static bool isItBMPFile(const char *path);
    void drawProgressBar();
    static void drawProgress(Adafruit_ILI9341 &screen, float progress);
    static void drawProgressWithClear(Adafruit_ILI9341 &screen, float progress);
    static void drawPause(Adafruit_ILI9341 &screen);
    static void cleanPause(Adafruit_ILI9341 &screen);

private:
    uint16_t readLE16();
    uint32_t readLE32();
};

#endif
