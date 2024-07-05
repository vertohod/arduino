#include "MenuDrawer.h"

#define TFT_CS 7
#define TFT_DC 9
#define SYMBOL_HEIGHT   8 
#define SYMBOL_WIDTH    6
#define MARGIN_MUL      0.5
#define OUTPUT_PATH_LENGTH 15
#define MENUDRAWER_DARKGREY 0x5555
#define THREE_POINTS "..."

MenuDrawer::MenuDrawer()
    : mScreen(Adafruit_ILI9341(TFT_CS, TFT_DC))
{
    mScreen.begin();
    mScreen.fillScreen(ILI9341_BLACK);
    mScreen.setTextColor(ILI9341_WHITE);
}

void MenuDrawer::setTextSize(uint8_t textSize)
{
    mTextHeight = textSize * SYMBOL_HEIGHT;
    mMargin = static_cast<float>(mTextHeight) * MARGIN_MUL;
    mItemHeight = mTextHeight + mMargin * 2;
    mTopPosition = mItemHeight + 1;

    mScreen.setTextSize(textSize);
}

void MenuDrawer::setHeader(const char* text) {
    char path[OUTPUT_PATH_LENGTH];

    mScreen.fillRect(0, 0, mScreen.width(), mItemHeight, ILI9341_BLACK);
    mScreen.setTextColor(ILI9341_WHITE);
    mScreen.setCursor(mMargin, mMargin);
    auto length = min(strlen(text), OUTPUT_PATH_LENGTH);
    memcpy(static_cast<void*>(path), static_cast<const void*>(text), length);
    if (length < strlen(text)) {
        memcpy(static_cast<void*>(&path[length]), static_cast<const void*>(THREE_POINTS), strlen(THREE_POINTS) + 1);
    } else {
        path[length] = 0;
    }
    mScreen.println(path);
}

uint8_t MenuDrawer::maxItems() {
    return mScreen.height() / (mItemHeight + 2) - 1;
}

void MenuDrawer::drawItem(const char* text, uint16_t position, bool active) {
    draw(text, position, active, true, true);
}

void MenuDrawer::quickDrawItem(const char* text, uint16_t position, bool active, bool fillAll) {
    draw(text, position, active, fillAll);
}

void MenuDrawer::draw(const char* text, uint16_t position, bool active, bool fillAll, bool drawLine) {
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
        mScreen.fillRect(mMargin, yPosition + mMargin, mScreen.width() - mMargin * 2, mTextHeight, backColor);
    }
    if (drawLine) {
        mScreen.drawLine(mMargin, yPosition + mItemHeight + 1, mScreen.width() - mMargin, yPosition + mItemHeight + 1, ILI9341_DARKGREY);
    }
    mScreen.setTextColor(textColor);
    mScreen.setCursor(mMargin, yPosition + mMargin + 2);
    mScreen.println(text);
}
