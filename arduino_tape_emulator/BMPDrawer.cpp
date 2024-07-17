#include "BMPDrawer.h"
#include "SwitchExceptions.h"
#include "Types.h"

#define PALETTE_SIZE        16
#define IMAGE_BUFFER_SIZE   16
#define TEXT_BYTES          "Bytes"

volatile bool gActiveCycle = true;

void drawBMP(Adafruit_ILI9341 &screen, const char *path, bool &isNeededToLoad) {
    tPath pathImage;
    memcpy(static_cast<void*>(&pathImage[0]), static_cast<const void*>(path), strlen(path) + 1);
    auto index = BMPDrawer::getLastPoint(pathImage);
    if (0 != index) {
        memcpy(static_cast<void*>(&pathImage[index + 1]), static_cast<const void*>(EXTENSION_BMP), 4);
    }

    BMPDrawer drawer(screen);
    drawer.drawFileInfo(path);
    drawer.draw(&pathImage[0], 0, 120);

    if (isNeededToLoad) {
        return;
    }
    gActiveCycle = true;
    enableExceptions();
    bool buttonIsClicked = false;
    while (gActiveCycle) {
        if (!(PIND & B00100000)) {
            if (!buttonIsClicked) {
                buttonIsClicked = true;
                isNeededToLoad = true;
                break;
            }
        } else {
            buttonIsClicked = false;
        }
    }
    disableExceptions();
}

BMPDrawer::BMPDrawer(Adafruit_ILI9341 &screen) : mScreen(screen) {
    mScreen.fillScreen(ILI9341_BLACK);
}

void BMPDrawer::drawFileInfo(const char *path) {
    mScreen.setTextSize(TEXT_SIZE);
    mScreen.setTextColor(ILI9341_WHITE);
    File file = SD.open(path);
    uint16_t length = strlen(file.name());
    uint32_t fileSize = file.size();

    uint16_t xPosition = (mScreen.width() - length * SYMBOL_WIDTH * TEXT_SIZE) / 2;
    uint16_t yPosition = static_cast<float>(SYMBOL_HEIGHT * TEXT_SIZE) * MARGIN_MUL;
    mScreen.setCursor(xPosition, yPosition);
    mScreen.println(file.name());
    file.close();

    yPosition += static_cast<float>(SYMBOL_HEIGHT * TEXT_SIZE * 2 + 2);
    mScreen.setCursor(0, yPosition);
    mScreen.println(F("Size: "));

    xPosition = mScreen.width() - SYMBOL_WIDTH * TEXT_SIZE * (strlen(TEXT_BYTES) + 2);
    auto sizeByte = fileSize;
    for(;;) {
        sizeByte = sizeByte / 10;
        if (sizeByte > 0) {
            xPosition -= SYMBOL_WIDTH * TEXT_SIZE;
        } else {
            break;
        }
    }
    mScreen.setCursor(xPosition, yPosition);
    mScreen.println(fileSize);
    xPosition = mScreen.width() - SYMBOL_WIDTH * TEXT_SIZE * strlen(TEXT_BYTES);
    mScreen.setCursor(xPosition, yPosition);
    mScreen.println(TEXT_BYTES);

    yPosition += static_cast<float>(SYMBOL_HEIGHT * TEXT_SIZE * 2 + 2);
    mScreen.drawRect(0, yPosition, mScreen.width(), SYMBOL_HEIGHT * TEXT_SIZE * 2, ILI9341_WHITE);
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

void BMPDrawer::drawPixel(uint16_t xPosition, uint16_t yPosition, uint16_t color) {
    mScreen.writePixel(xPosition, yPosition, color);
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

uint16_t BMPDrawer::getLastPoint(const char* path) {
    uint16_t lastPointIndex = 0;
    uint16_t strLen = strlen(path);
    for (uint16_t i = 0; i < strLen; ++i) {
        if (path[i] == '.') {
            lastPointIndex = i;
        }
    }
    return lastPointIndex;
}

bool BMPDrawer::isItBMPFile(const char *path) {
    auto lastPoint = getLastPoint(path);
    if (lastPoint != 0) {
        if (0 == strcmp(&path[lastPoint + 1], EXTENSION_BMP)) {
            return true; 
        }   
    } 
    return false;
}

void BMPDrawerInt0Handler() {
    gActiveCycle = false;
}

void BMPDrawerInt1Handler() {
    gActiveCycle = false;
}
