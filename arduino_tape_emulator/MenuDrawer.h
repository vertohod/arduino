#ifndef DRAW_MENU_H
#define DRAW_MENU_H

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "string.h"
#include "IMenuDrawer.h"

#define TEXT_SIZE       3
#define SYMBOL_HEIGHT   8 
#define SYMBOL_WIDTH    6
#define MARGIN_MUL      0.25 

class MenuDrawer : public IMenuDrawer
{
private:
    int8_t mCS;
    int8_t mDC;
    Adafruit_ILI9341* mScreen;
    int8_t mTextSize;
    size_t mTopPosition;
    size_t mMargin;
    size_t mWidth;
    size_t mHeight;
    size_t mItemHeight;

public:
    MenuDrawer(int8_t cs, int8_t dc) : mCS(cs), mDC(dc), mTextSize(TEXT_SIZE) {
        mScreen = new Adafruit_ILI9341(mCS, mDC);
        mScreen->fillScreen(ILI9341_BLACK);
        mScreen->setTextColor(ILI9341_WHITE);
        mScreen->setTextSize(mTextSize);

        mWidth = mScreen->width(),
        mHeight = mScreen->height();

        mMargin = mTextSize * SYMBOL_HEIGHT * MARGIN_MUL;
        mItemHeight = mMargin * 2 + mTextSize * SYMBOL_HEIGHT;
        mTopPosition = mItemHeight;
    }
    ~MenuDrawer() {
        delete mScreen;
    }
    void setHeader(const string& text) {
        mScreen->fillRect(0, 0, mWidth, mTopPosition, ILI9341_BLACK);
        mScreen->setTextColor(ILI9341_WHITE);
        mScreen->setCursor(mMargin, mMargin);
        mScreen->println(text.c_str());
    }
    void drawItem(const string& text, size_t position, bool active) override {
        auto backColor = ILI9341_BLACK;
        auto textColor = ILI9341_LIGHTGREY;
        if (active) {
            backColor = ILI9341_WHITE;
            textColor = ILI9341_DARKGREY;
        }
        auto yPosition = mTopPosition + mItemHeight * position;
        mScreen->fillRect(0, yPosition, mWidth, yPosition + mItemHeight, backColor);
        mScreen->drawLine(mMargin, yPosition + mItemHeight, mWidth - mMargin, yPosition + mItemHeight, ILI9341_DARKGREY);
        mScreen->setTextColor(textColor);
        mScreen->setCursor(mMargin, mTopPosition + mItemHeight * position + mMargin);
        mScreen->println(text.c_str());
    }
};

#endif
