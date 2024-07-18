#include "SwitchExceptions.h"
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
byte* gDataBufferPtr = nullptr;

void writeToPort() {
    PORTD = blockHandler->getLevel() ? 0xff : 0x00;
    Timer1::instance().init(blockHandler->getPeriod());
    Timer1::instance().start();
}

void initReading()
{
    uint16_t length = fileReader->getData(gDataBufferPtr, TAPE_BUFFER_SIZE);
    if (length > 0) {
        blockHandler->fillBuffer(gDataBufferPtr, length);
        blockHandler->start(fileReader->getBlockType());
        writeToPort();
    }
}

void drawProgress() {
    BMPDrawer::drawProgress(gScreen, static_cast<float>(fileReader->getFilePosition()) / fileReader->getFileSize());
}

void (*int0Functor)() = nullptr;
void (*int1Functor)() = nullptr;

void FileReaderInt0Handler(void) {
    fileReader->setPreviousBlock();
    drawProgress();
}

void FileReaderInt1Handler(void) {
    fileReader->setNextBlock();
    drawProgress();
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
                    int0Functor = FileReaderInt0Handler;
                    int1Functor = FileReaderInt1Handler;
                    enableExceptions();
                } else {
                    disableExceptions();
                    int0Functor = nullptr;
                    int1Functor = nullptr;
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
        writeToPort();
    }
}

bool gEncoderInt0;
bool gEncoderInt1;
ISR(INT0_vect)
{
    if (gEncoderInt1) {
        gEncoderInt1 = false;
        if (int0Functor) {
            int0Functor();
        }
    } else {
        gEncoderInt0 = true;
    }
}

ISR(INT1_vect)
{
    if (gEncoderInt0) {
        gEncoderInt0 = false;
        if (int1Functor) {
            int1Functor();
        }
    } else {
        gEncoderInt1 = true;
    }
}
