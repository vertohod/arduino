#include "MenuDrawer.h"
#include "Types.h"

#define OUTPUT_PATH_LENGTH 15
#define THREE_POINTS "..."

MenuDrawer::MenuDrawer(Adafruit_ILI9341 &screen) : mScreen(screen)
{
    mScreen.fillScreen(ILI9341_BLACK);
}

void MenuDrawer::setHeader(const char* text) {
    char path[OUTPUT_PATH_LENGTH];

    mScreen.fillRect(0, 0, mScreen.width(), getItemHeight(), ILI9341_BLACK);
    mScreen.setTextColor(ILI9341_WHITE);
    mScreen.setCursor(getMargin(), getMargin());
    auto length = min(strlen(text), OUTPUT_PATH_LENGTH);
    memcpy(static_cast<void*>(path), static_cast<const void*>(text), length);
    if (length < strlen(text)) {
        memcpy(static_cast<void*>(&path[length]), static_cast<const void*>(THREE_POINTS), strlen(THREE_POINTS) + 1);
    } else {
        path[length] = 0;
    }
    mScreen.println(path);
}

uint8_t MenuDrawer::getTextHeight() {
    return SYMBOL_HEIGHT * TEXT_SIZE;
}

uint8_t MenuDrawer::getMargin() {
    return static_cast<float>(getTextHeight()) * MARGIN_MUL;
}

uint8_t MenuDrawer::getItemHeight() {
    return getTextHeight() + getMargin() * 2;
}

uint8_t MenuDrawer::getTopPosition() {
    return getItemHeight() + 1;
}

uint16_t MenuDrawer::getYCoord(uint8_t line) {
    return getTopPosition() + (getItemHeight() + 2) * line;
}

void MenuDrawer::draw(const char* text, uint16_t position, bool active, bool fillAll, bool drawLine) {
    auto backColor = active ? ILI9341_WHITE : ILI9341_BLACK;
    auto yPosition = getYCoord(position);
    if (fillAll) {
        mScreen.fillRect(0, yPosition, mScreen.width(), getItemHeight(), backColor);
    } else {
        mScreen.fillRect(getMargin(), yPosition + getMargin(), mScreen.width() - getMargin() * 2, getTextHeight(), backColor);
    }
    if (drawLine) {
        auto yCoordLine = yPosition + getItemHeight() + 1;
        mScreen.drawLine(getMargin(), yCoordLine, mScreen.width() - getMargin(), yCoordLine, ILI9341_DARKGREY);
    }
    mScreen.setTextColor(active ? ILI9341_BLACK : ILI9341_LIGHTGREY);
    mScreen.setCursor(getMargin(), yPosition + getMargin() + 2);
    mScreen.println(text);
}
