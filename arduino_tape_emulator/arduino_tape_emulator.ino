#include "DurationCounter.h"
#include "BlockHandler.h"
#include "FileReader.h"
#include "BMPDrawer.h"
#include "Timer1.h"
#include "Menu.h"

#include <SPI.h>
#include <SD.h>

#define TFT_CS 7
#define TFT_DC 9
#define SD_CS 10
#define OUTPUT_PIN 4
#define DURATION_PAUSE 2.0 // seconds
#define ROOT "/"

Adafruit_ILI9341 gScreen(TFT_CS, TFT_DC);

void setup() {
    if (!SD.begin(SD_CS)) {
        while(true);
    }
    // Enable output to port D
    DDRD = B00010000;
    gScreen.begin();
}

FileReader *fileReader = nullptr;
BlockHandler *blockHandler = nullptr;
bool nextLevelUp = false;
float nextPeriod = 0.0;
byte* gDataBufferPtr = nullptr;
uint32_t gFileSize = 0;
uint32_t gFileRead = 0;

void initReading()
{
    uint16_t length = fileReader->getData(gDataBufferPtr, TAPE_BUFFER_SIZE);
    gFileRead += length;
    if (length > 0) {
        blockHandler->fillBuffer(gDataBufferPtr, length);
        blockHandler->start(fileReader->getBlockType());

        // initialization
        nextLevelUp = blockHandler->getLevel();
        nextPeriod = blockHandler->getPeriod();

        // just launch the process with some period
        Timer1::instance().init(nextPeriod);
        Timer1::instance().start();
    }
}

void drawProgress() {
    uint16_t yPosition = SYMBOL_HEIGHT * TEXT_SIZE * MARGIN_MUL;
    yPosition += (SYMBOL_HEIGHT * TEXT_SIZE + 1) * 4;
    uint16_t width = static_cast<float>(gFileRead) / gFileSize * gScreen.width();
    uint16_t xPosition = width > 4 ? width - 4 : 0;
    width = width > 4 ? 4 : width;
    gScreen.fillRect(xPosition, yPosition, width, SYMBOL_HEIGHT * TEXT_SIZE, ILI9341_WHITE);
}

void startReading(const char *path) {
    byte localDataBuffer[TAPE_BUFFER_SIZE];

    gFileSize = FileReader::getFileSize(path);
    gFileRead = 0;

    FileReader localFileRieader(path);
    BlockHandler localBlockHandler(ROOT); // ROOT is not needed

    fileReader = static_cast<FileReader*>(&localFileRieader);
    blockHandler = static_cast<BlockHandler*>(&localBlockHandler);
    gDataBufferPtr = localDataBuffer;

    initReading();
    while (true) {
        if (!(localFileRieader.isPause())) {
            if (!(blockHandler->isFinished()) && blockHandler->isBufferEmpty()) {
                uint16_t length = localFileRieader.getData(gDataBufferPtr, TAPE_BUFFER_SIZE);
                gFileRead += length;
                if (length > 0) {
                    blockHandler->fillBuffer(gDataBufferPtr, length);
                }
                drawProgress();
            }
        }
        if (localFileRieader.isFinished() && blockHandler->isFinished()) {
            break;
        }
    }
    fileReader = nullptr;
}

tPath gPathFile = "/";
uint16_t gPosition = 0;
void (*int0Functor)() = nullptr;
void (*int1Functor)() = nullptr;

void loop()
{
    int0Functor = MenuInt0Handler;
    int1Functor = MenuInt1Handler;
    bool isNeededToLoad = getPathFile(gScreen, gPathFile, gPosition);

    int0Functor = BMPDrawerInt0Handler;
    int1Functor = BMPDrawerInt1Handler;
    drawBMP(gScreen, gPathFile, isNeededToLoad);

    int0Functor = nullptr;
    int1Functor = nullptr;

    if (isNeededToLoad) {
        startReading(gPathFile);
    }
}

// ---------- Interuptrs Handlers ------------
DurationCounter dc;

ISR(TIMER1_COMPA_vect)
{
    if (!fileReader) {
        return;
    }
    if (fileReader->isPause() && blockHandler->isFinished()) {
        if (dc.enabled()) {
            if (dc.check(Timer1::instance().duration())) {
                fileReader->readContinue();
                initReading();
            }
        } else {
            dc.set(DURATION_PAUSE);
            Timer1::instance().init(DURATION_PAUSE);
            Timer1::instance().start();
            return;
        }
    }
    if (!blockHandler->isFinished()) {
        PORTD = nextLevelUp ? 0xff : 0x00;
        Timer1::instance().start();

        nextLevelUp = blockHandler->getLevel();
        nextPeriod = blockHandler->getPeriod();

        // initialize the timer now do not waste time
        Timer1::instance().init(nextPeriod);
    }
}

ISR(INT0_vect)
{
    if (int0Functor) {
        int0Functor();
    }
}

ISR(INT1_vect)
{
    if (int1Functor) {
        int1Functor();
    }
}
