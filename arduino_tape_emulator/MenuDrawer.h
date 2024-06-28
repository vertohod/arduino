#ifndef DRAW_MENU_H
#define DRAW_MENU_H

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "string.h"
#include "IMenuDrawer.h"

#define SYMBOL_HEIGHT   8 
#define SYMBOL_WIDTH    6
#define MARGIN_MUL      0.5
#define MENUDRAWER_DARKGREY 0x5555

class MenuDrawer : public IMenuDrawer
{
private:
    Adafruit_ILI9341 mScreen;
    uint8_t mTopPosition;
    uint8_t mMargin;
    uint16_t mWidth;
    uint16_t mHeight;
    uint8_t mTextHeight;
    uint8_t mItemHeight;

public:
    MenuDrawer(int8_t cs, int8_t dc, uint8_t textSize) : mScreen(Adafruit_ILI9341(cs, dc))
        , mTextHeight(SYMBOL_HEIGHT * textSize)
    {
        Serial.println(F("(DMenuDrawer) call constructor"));
        mScreen.begin();
        mScreen.fillScreen(ILI9341_BLACK);
        mScreen.setTextColor(ILI9341_WHITE);
        mScreen.setTextSize(textSize);

        mWidth = mScreen.width(),
        mHeight = mScreen.height();

        mMargin = static_cast<uint8_t>(static_cast<float>(mTextHeight) * MARGIN_MUL);
        mItemHeight = mTextHeight + mMargin * 2;
        mTopPosition = mItemHeight + 1;
        Serial.println(F("(DMenuDrawer) construction is completed"));
    }
    void setHeader(const string& text) {
        mScreen.fillRect(0, 0, mWidth, mTopPosition, ILI9341_BLACK);
        mScreen.setTextColor(ILI9341_WHITE);
        mScreen.setCursor(mMargin, mMargin);
        mScreen.println(text.c_str());
    }
    void drawItem(const string& text, size_t position, bool active) override {
        auto backColor = ILI9341_BLACK;
        auto textColor = ILI9341_LIGHTGREY;
        if (active) {
            backColor = ILI9341_WHITE;
            textColor = ILI9341_BLACK;
        }
        auto yPosition = mTopPosition + (mItemHeight + 1) * position;
        mScreen.fillRect(0, yPosition, mWidth, mItemHeight, backColor);
        mScreen.drawLine(mMargin, yPosition + mItemHeight, mWidth - mMargin, yPosition + mItemHeight, ILI9341_DARKGREY);
        mScreen.setTextColor(textColor);
        mScreen.setCursor(mMargin, yPosition + mMargin + 1);
        mScreen.println(text.c_str());
    }
    void quickDrawItem(const string& text, size_t position, bool active) override {
        auto backColor = ILI9341_BLACK;
        auto textColor = ILI9341_LIGHTGREY;
        if (active) {
            backColor = ILI9341_WHITE;
            textColor = ILI9341_BLACK;
        }
        auto yPosition = mTopPosition + (mItemHeight + 1) * position;
        mScreen.fillRect(mMargin, yPosition + mMargin, mWidth - 2 * mMargin, mTextHeight, backColor);
        mScreen.setTextColor(textColor);
        mScreen.setCursor(mMargin, yPosition + mMargin + 1);
        mScreen.println(text.c_str());
    }
};

#endif
