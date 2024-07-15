#include "MenuDrawer.h"

#define SYMBOL_HEIGHT   8 
#define SYMBOL_WIDTH    6
#define MARGIN_MUL      0.5
#define OUTPUT_PATH_LENGTH 15
#define MENUDRAWER_DARKGREY 0x5555
#define THREE_POINTS "..."

MenuDrawer::MenuDrawer(Adafruit_ILI9341 *screenPtr) : mScreenPtr(screenPtr)
{
    mScreenPtr->begin();
    mScreenPtr->fillScreen(ILI9341_BLACK);
}

void MenuDrawer::setTextSize(uint8_t textSize)
{
    mTextHeight = textSize * SYMBOL_HEIGHT;
    mMargin = static_cast<float>(mTextHeight) * MARGIN_MUL;
    mItemHeight = mTextHeight + mMargin * 2;
    mTopPosition = mItemHeight + 1;

    mScreenPtr->setTextSize(textSize);
}

void MenuDrawer::setHeader(const char* text) {
    char path[OUTPUT_PATH_LENGTH];

    mScreenPtr->fillRect(0, 0, mScreenPtr->width(), mItemHeight, ILI9341_BLACK);
    mScreenPtr->setTextColor(ILI9341_WHITE);
    mScreenPtr->setCursor(mMargin, mMargin);
    auto length = min(strlen(text), OUTPUT_PATH_LENGTH);
    memcpy(static_cast<void*>(path), static_cast<const void*>(text), length);
    if (length < strlen(text)) {
        memcpy(static_cast<void*>(&path[length]), static_cast<const void*>(THREE_POINTS), strlen(THREE_POINTS) + 1);
    } else {
        path[length] = 0;
    }
    mScreenPtr->println(path);
}

void MenuDrawer::draw(const char* text, uint16_t position, bool active, bool fillAll, bool drawLine) {
    auto backColor = active ? ILI9341_WHITE : ILI9341_BLACK;
    auto yPosition = mTopPosition + (mItemHeight + 2) * position;
    if (fillAll) {
        mScreenPtr->fillRect(0, yPosition, mScreenPtr->width(), mItemHeight, backColor);
    } else {
        mScreenPtr->fillRect(mMargin, yPosition + mMargin, mScreenPtr->width() - mMargin * 2, mTextHeight, backColor);
    }
    if (drawLine) {
        mScreenPtr->drawLine(mMargin, yPosition + mItemHeight + 1, mScreenPtr->width() - mMargin, yPosition + mItemHeight + 1, ILI9341_DARKGREY);
    }
    mScreenPtr->setTextColor(active ? ILI9341_BLACK : ILI9341_LIGHTGREY);
    mScreenPtr->setCursor(mMargin, yPosition + mMargin + 2);
    mScreenPtr->println(text);
}
