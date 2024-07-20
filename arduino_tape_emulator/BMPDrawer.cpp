#include "SwitchExceptions.h"
#include "Definitions.h"
#include "Extensions.h"
#include "MenuDrawer.h"
#include "BMPDrawer.h"
#include "Functions.h"
#include "Types.h"

volatile bool gActiveCycle = true;

void drawBMP(SD& sd, Adafruit_ILI9341 &screen, const char *path, bool &isNeededToLoad) {
    tPath pathImage;
    memcpy(static_cast<void*>(&pathImage[0]), static_cast<const void*>(path), strlen(path) + 1);
    auto index = Functions::getLastPoint(pathImage);
    strcpy_P(&pathImage[index], extensions[EXTENSION_INDEX_BMP]);

    BMPDrawer drawer(sd, screen);
    drawer.drawFileInfo(path);
    drawer.draw(&pathImage[0], 0, 120);

    if (isNeededToLoad) {
        return;
    }
    gActiveCycle = true;
    enableExceptions();
    bool buttonIsClicked = false;
    while (gActiveCycle) {
        if (PIND & B00100000) {
            buttonIsClicked = false;
        } else {
            if (!buttonIsClicked) {
                buttonIsClicked = true;
                isNeededToLoad = true;
                break;
            }
        }
    }
    disableExceptions();
}

BMPDrawer::BMPDrawer(SD& sd, Adafruit_ILI9341 &screen)
    : mSD(sd)
    , mScreen(screen)
{
    mScreen.fillScreen(ILI9341_BLACK);
}

void BMPDrawer::drawFileInfo(const char *path) {
    char fileName[100];
    mScreen.setTextColor(ILI9341_WHITE);
    File file = mSD.open(path);
    file.getName(&fileName[0], sizeof(fileName));
    uint16_t length = strlen(&fileName[0]);

    uint16_t xPosition = (mScreen.width() - length * SYMBOL_WIDTH * TEXT_SIZE) / 2;
    mScreen.setCursor(xPosition, MenuDrawer::getMargin());
    mScreen.println(&fileName[0]);

    mScreen.setCursor(0, MenuDrawer::getTextPosition(0) + 2);
    mScreen.println("Size:");

    xPosition = mScreen.width() - SYMBOL_WIDTH * TEXT_SIZE * (strlen_P(TEXT_BYTES) + 2);
    auto sizeByte = file.size();
    while (true) {
        sizeByte = sizeByte / 10;
        if (sizeByte > 0) {
            xPosition -= SYMBOL_WIDTH * TEXT_SIZE;
        } else {
            break;
        }
    }
    mScreen.setCursor(xPosition, MenuDrawer::getTextPosition(0) + 2);
    mScreen.println(file.size());
    xPosition = mScreen.width() - SYMBOL_WIDTH * TEXT_SIZE * strlen_P(TEXT_BYTES);
    mScreen.setCursor(xPosition, MenuDrawer::getTextPosition(0) + 2);
    mScreen.println(TEXT_BYTES);

    drawProgressBar();
    file.close();
}

void BMPDrawer::drawProgressBar() {
    mScreen.drawRect(0, MenuDrawer::getTextPosition(1), mScreen.width(), MenuDrawer::getTextHeight(), ILI9341_WHITE);
}

uint16_t BMPDrawer::discreteNumber(uint16_t number) {
    return number / 4 * 4;
}

void BMPDrawer::drawProgress(Adafruit_ILI9341 &screen, bool withClear, float progress) {
    uint16_t width = progress * (screen.width() - 2);
    if (withClear) {
        screen.fillRect(width + 1, MenuDrawer::getTextPosition(1) + 3, screen.width() - width - 2, MenuDrawer::getTextHeight() - 6, ILI9341_BLACK);
        for (uint16_t i = 0; i < width; ++i) {
            screen.drawLine(discreteNumber(i), MenuDrawer::getTextPosition(1) + 3, discreteNumber(i), MenuDrawer::getTextPosition(1) + MenuDrawer::getTextHeight() - 4, ILI9341_WHITE);
        }
    }
    auto xPosition = discreteNumber(width);
    screen.drawLine(xPosition, MenuDrawer::getTextPosition(1) + 3, xPosition, MenuDrawer::getTextPosition(1) + MenuDrawer::getTextHeight() - 4, ILI9341_WHITE);
}


void BMPDrawer::drawPause(Adafruit_ILI9341 &screen) {
    screen.setCursor(86, MenuDrawer::getItemPosition(2) - 2);
    screen.setTextColor(ILI9341_RED);
    screen.println(TEXT_PAUSED);
}

void BMPDrawer::cleanPause(Adafruit_ILI9341 &screen) {
    screen.fillRect(0, MenuDrawer::getItemPosition(2) - 2, screen.width(), MenuDrawer::getTextHeight(), ILI9341_BLACK);
}

void BMPDrawer::draw(const char *path, int16_t xPosition, int16_t yPosition) {
    uint16_t palette[PALETTE_SIZE]; // 16-bit 5/6/5 color palette
    byte buffer[IMAGE_BUFFER_SIZE];
    uint16_t outBuffer[IMAGE_BUFFER_SIZE * 2];

    mFile = mSD.open(path);
    if (!mFile) {
        return;
    }

    if (Functions::readLE16(mFile) == 0x4D42) {     // BMP signature
        Functions::readLE32(mFile);                 // Read & ignore file size
        Functions::readLE32(mFile);                 // Read & ignore creator bytes
        auto offset = Functions::readLE32(mFile);   // Start of image data

        // Read DIB header
        uint32_t headerSize = Functions::readLE32(mFile);
        uint16_t bmpWidth = Functions::readLE32(mFile);
        int height = Functions::readLE32(mFile);
        uint16_t bmpHeight = height < 0 ? -height : height;

        mFile.seek(14 + headerSize);
        for (uint16_t counter = 0; counter < PALETTE_SIZE; ++counter) {
            uint16_t blue = mFile.read();
            uint16_t green = mFile.read();
            // It looks strange, but reduces binary code
            // uint16_t red = mFile.read();
            palette[counter] = static_cast<uint16_t>((mFile.read() & 0x00F8) << 8) | static_cast<uint16_t>((green & 0x00FC) << 3) | static_cast<uint16_t>(blue >> 3);
            mFile.read(); // Ignore 4th byte
        }

        float factor = static_cast<float>(mScreen.width()) / bmpWidth;

        mScreen.startWrite();
        mScreen.setAddrWindow(xPosition, yPosition, mScreen.width(), bmpHeight);
        mScreen.dmaWait();
        mScreen.endWrite();

        for (uint16_t yCounter = 0; yCounter < bmpHeight; ++yCounter) {
            uint32_t filePosition = offset + (bmpHeight - yCounter - 1) * bmpWidth / 2;
            mFile.seek(filePosition);
            for (uint16_t xCounter = 0; xCounter < bmpWidth;) {
                mFile.read(&buffer[0], IMAGE_BUFFER_SIZE);
                uint16_t index = 0;
                for (uint16_t i = 0; i < IMAGE_BUFFER_SIZE; ++i) {
                    byte data = buffer[i];
                    byte fourBitsHi = data >> 4;
                    byte fourBitsLo = data & 0x0f;

                    index = static_cast<uint16_t>(static_cast<float>(i * 2) * factor);
                    outBuffer[index] = palette[fourBitsHi];
                    index = static_cast<uint16_t>(static_cast<float>(i * 2 + 1) * factor);
                    outBuffer[index] = palette[fourBitsLo];
                }
                mScreen.startWrite();
                mScreen.writePixels(&outBuffer[0], index + 1, true);
                mScreen.dmaWait();
                mScreen.endWrite();
                xCounter += IMAGE_BUFFER_SIZE * 2;
            }
        }
    } 
    mFile.close();
}

void BMPDrawerInt0Handler() {
    gActiveCycle = false;
}

void BMPDrawerInt1Handler() {
    gActiveCycle = false;
}
