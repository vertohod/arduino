#ifndef DRAW_MENU_H
#define DRAW_MENU_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define OUTPUT_ITEM_LENGTH 20

class MenuDrawer {
private:
    Adafruit_ILI9341 &mScreen;
    uint8_t mTextHeight;
    uint8_t mMargin;
    uint8_t mItemHeight;
    uint8_t mTopPosition;

public:
    MenuDrawer(Adafruit_ILI9341 &screen);
    void setTextSize(uint8_t textSize);
    void setHeader(const char* text);
    inline void drawItem(const char* text, uint16_t position, bool active) {
        draw(text, position, active, true, true);
    }
    inline void quickDrawItem(const char* text, uint16_t position, bool active, bool fillAll) {
        draw(text, position, active, fillAll);
    }
private:
    void draw(const char* text, uint16_t position, bool active, bool fillAll = false, bool drawLine = false);
};

#endif
