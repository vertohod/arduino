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

        mMargin = static_cast<uint8_t>(static_cast<float>(mTextHeight) * MARGIN_MUL);
        mItemHeight = mTextHeight + mMargin * 2;
        mTopPosition = mItemHeight + 1;
        Serial.println(F("(DMenuDrawer) construction is completed"));
    }
    void setHeader(const string& text) {
        mScreen.fillRect(0, 0, mScreen.width(), mTopPosition, ILI9341_BLACK);
        mScreen.setTextColor(ILI9341_WHITE);
        mScreen.setCursor(mMargin, mMargin);
        mScreen.println(text.c_str());
    }
private:
    void draw(const string& text, uint16_t position, bool active, bool fillAll = false, bool drawLine = false) {
        auto backColor = ILI9341_BLACK;
        auto textColor = ILI9341_LIGHTGREY;
        if (active) {
            backColor = ILI9341_WHITE;
            textColor = ILI9341_BLACK;
        }
        auto yPosition = mTopPosition + (mItemHeight + 2) * position;
        if (fillAll) {
            mScreen.fillRect(0, yPosition, mScreen.width(), mItemHeight, backColor);
        } else {
            mScreen.fillRect(mMargin, yPosition + mMargin, mScreen.width() - 2 * mMargin, mTextHeight, backColor);
        }
        if (drawLine) {
            mScreen.drawLine(mMargin, yPosition + mItemHeight + 1, mScreen.width() - mMargin, yPosition + mItemHeight, ILI9341_DARKGREY);
        }
        mScreen.setTextColor(textColor);
        mScreen.setCursor(mMargin, yPosition + mMargin + 2);
        mScreen.println(text.c_str());
    }
public:
    uint16_t maxItems() override {
        return mScreen.height() / (mItemHeight + 2);
    }
    void drawItem(const string& text, uint16_t position, bool active) override {
        draw(text, position, active, true, true);
    }
    void quickDrawItem(const string& text, uint16_t position, bool active, bool fillAll) override {
        draw(text, position, active, fillAll);
    }
};

#endif
