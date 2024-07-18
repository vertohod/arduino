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
    gScreen.setTextSize(TEXT_SIZE);
}

FileReader *fileReader = nullptr;
BlockHandler *blockHandler = nullptr;
bool gNextLevelUp = false;
byte* gDataBufferPtr = nullptr;

void initReading()
{
    uint16_t length = fileReader->getData(gDataBufferPtr, TAPE_BUFFER_SIZE);
    if (length > 0) {
        blockHandler->fillBuffer(gDataBufferPtr, length);
        blockHandler->start(fileReader->getBlockType());

        gNextLevelUp = blockHandler->getLevel();

        Timer1::instance().init(blockHandler->getPeriod());
        Timer1::instance().start();
    }
}

void drawProgress() {
    BMPDrawer::drawProgress(gScreen, static_cast<float>(fileReader->getFilePosition()) / fileReader->getFileSize());
}

volatile bool gFlipFlop = false;

void startReading(const char *path) {
    byte localDataBuffer[TAPE_BUFFER_SIZE];

    FileReader localFileRieader(path);
    BlockHandler localBlockHandler(ROOT); // ROOT is not needed

    fileReader = static_cast<FileReader*>(&localFileRieader);
    blockHandler = static_cast<BlockHandler*>(&localBlockHandler);
    gDataBufferPtr = &localDataBuffer[0];

    initReading();
    gFlipFlop = false;
    bool buttonIsClicked = false;
    while (true) {
        if (!(PIND & B00100000)) {
            if (!buttonIsClicked) {
                buttonIsClicked = true;
                if (!gFlipFlop) {
                    fileReader->setPause();
                    blockHandler->init();
                    BMPDrawer::drawPause(gScreen);
                } else {
                    BMPDrawer::cleanPause(gScreen);
                    fileReader->readContinue(true);
                    initReading();
                }
                gFlipFlop = !gFlipFlop;
            }
        } else {
            buttonIsClicked = false;
        }
        if (!(fileReader->isPause())) {
            if (!(blockHandler->isFinished()) && blockHandler->isBufferEmpty()) {
                uint16_t length = fileReader->getData(gDataBufferPtr, TAPE_BUFFER_SIZE);
                if (length > 0) {
                    blockHandler->fillBuffer(gDataBufferPtr, length);
                }
                drawProgress();
            }
        }
        if (fileReader->isFinished() && blockHandler->isFinished()) {
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
    if (!fileReader || gFlipFlop) {
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
        PORTD = gNextLevelUp ? 0xff : 0x00;
        Timer1::instance().start();

        gNextLevelUp = blockHandler->getLevel();

        // initialize the timer now do not waste time
        Timer1::instance().init(blockHandler->getPeriod());
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
