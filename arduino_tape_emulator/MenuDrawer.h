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

public:
    MenuDrawer(Adafruit_ILI9341 &screen);
    void setHeader(const char* text);
    inline void drawItem(const char* text, uint16_t position, bool active) {
        draw(text, position, active, true, true);
    }
    inline void quickDrawItem(const char* text, uint16_t position, bool active) {
        draw(text, position, active, true);
    }
    static uint8_t getTextHeight();
    static uint8_t getMargin();
    static uint8_t getItemHeight();
    static uint8_t getTopPosition();
    static uint16_t getItemPosition(uint8_t line);
    static uint16_t getTextPosition(uint8_t line);

private:
    void draw(const char* text, uint16_t position, bool active, bool fillAll = false, bool drawLine = false);
};

#endif
