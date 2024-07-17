#include "MenuDrawer.h"
#include "Types.h"

#define OUTPUT_PATH_LENGTH 15
#define THREE_POINTS "..."

MenuDrawer::MenuDrawer(Adafruit_ILI9341 &screen) : mScreen(screen)
{
    mScreen.fillScreen(ILI9341_BLACK);

    mTextHeight = SYMBOL_HEIGHT * TEXT_SIZE;
    mMargin = static_cast<float>(mTextHeight) * MARGIN_MUL;
    mItemHeight = mTextHeight + mMargin * 2;
    mTopPosition = mItemHeight + 1;
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

void MenuDrawer::draw(const char* text, uint16_t position, bool active, bool fillAll, bool drawLine) {
    auto backColor = active ? ILI9341_WHITE : ILI9341_BLACK;
    auto yPosition = mTopPosition + (mItemHeight + 2) * position;
    if (fillAll) {
        mScreen.fillRect(0, yPosition, mScreen.width(), mItemHeight, backColor);
    } else {
        mScreen.fillRect(mMargin, yPosition + mMargin, mScreen.width() - mMargin * 2, mTextHeight, backColor);
    }
    if (drawLine) {
        mScreen.drawLine(mMargin, yPosition + mItemHeight + 1, mScreen.width() - mMargin, yPosition + mItemHeight + 1, ILI9341_DARKGREY);
    }
    mScreen.setTextColor(active ? ILI9341_BLACK : ILI9341_LIGHTGREY);
    mScreen.setCursor(mMargin, yPosition + mMargin + 2);
    mScreen.println(text);
}
