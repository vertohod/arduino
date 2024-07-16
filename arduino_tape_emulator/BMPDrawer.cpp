#include "BMPDrawer.h"

#define PALETTE_SIZE 16
#define IMAGE_BUFFER_SIZE 16

void drawBMP(Adafruit_ILI9341 *screenPtr, const char *path) {
    BMPDrawer drawer(screenPtr);
    drawer.draw(path, 0, 120);
}

BMPDrawer::BMPDrawer(Adafruit_ILI9341 *screenPtr) : mScreenPtr(screenPtr) {
    mScreenPtr->begin();
    mScreenPtr->fillScreen(ILI9341_BLACK);
    mScreenPtr->setTextColor(ILI9341_WHITE);
}

void BMPDrawer::draw(const char *path, int16_t xPosition, int16_t yPosition) {
    uint32_t offset = 0;                        // Start of image data in file
    uint32_t headerSize = 0;                    // Indicates BMP version
    uint16_t bmpWidth = 0, bmpHeight = 0;       // BMP width & height in pixels
    uint16_t palette[PALETTE_SIZE];             // 16-bit 5/6/5 color palette
    byte buffer[IMAGE_BUFFER_SIZE];
    uint16_t outBuffer[IMAGE_BUFFER_SIZE * 2];

    mFile = SD.open(path);
    if (!mFile) {
        return;
    }

    if (readLE16() == 0x4D42) {     // BMP signature
        readLE32();                 // Read & ignore file size
        readLE32();                 // Read & ignore creator bytes
        offset = readLE32();        // Start of image data

        // Read DIB header
        headerSize = readLE32();
        bmpWidth = readLE32();
        int height = readLE32();
        bmpHeight = height < 0 ? -height : height;

        mFile.seek(14 + headerSize);
        for (uint16_t counter = 0; counter < PALETTE_SIZE; ++counter) {
            uint16_t blue = mFile.read();
            uint16_t green = mFile.read();
            uint16_t red = mFile.read();
            mFile.read(); // Ignore 4th byte
            palette[counter] = static_cast<uint16_t>((red & 0x00F8) << 8) | static_cast<uint16_t>((green & 0x00FC) << 3) | static_cast<uint16_t>(blue >> 3);
        }

        float factor = static_cast<float>(mScreenPtr->width()) / bmpWidth;

        mScreenPtr->startWrite();
        mScreenPtr->setAddrWindow(xPosition, yPosition, mScreenPtr->width(), bmpHeight);

        for (uint16_t yCounter = 0; yCounter < bmpHeight; ++yCounter) {
            uint32_t filePosition = offset + (bmpHeight - yCounter - 1) * bmpWidth / 2;
            mScreenPtr->dmaWait();
            mScreenPtr->endWrite();
            mFile.seek(filePosition);
            for (uint16_t xCounter = 0; xCounter < bmpWidth;) {
                mScreenPtr->dmaWait();
                mScreenPtr->endWrite();
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
                mScreenPtr->startWrite();
                mScreenPtr->writePixels(&outBuffer[0], index + 1, true);

                xCounter += IMAGE_BUFFER_SIZE * 2;
            }
        }
    } 
    mFile.close();
}

void BMPDrawer::drawPixel(uint16_t xPosition, uint16_t yPosition, uint16_t color) {
    mScreenPtr->writePixel(xPosition, yPosition, color);
}

uint16_t BMPDrawer::readLE16() {
  return static_cast<uint16_t>(mFile.read()) | static_cast<uint16_t>(mFile.read()) << 8;
}

uint32_t BMPDrawer::readLE32() {
  return static_cast<uint32_t>(mFile.read())
      | static_cast<uint32_t>(mFile.read()) << 8
      | static_cast<uint32_t>(mFile.read()) << 16
      | static_cast<uint32_t>(mFile.read()) << 24;
}
