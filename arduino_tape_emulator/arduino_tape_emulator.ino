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

FileReader *gFileReader = nullptr;
BlockHandler *gBlockHandler = nullptr;
byte* gDataBufferPtr = nullptr;

void writeToPort() {
    PORTD = gBlockHandler->getLevel() ? 0xff : 0x00;
    Timer1::instance().start(gBlockHandler->getPeriod());
}

void initReading() {
    uint16_t length = gFileReader->getData(gDataBufferPtr, TAPE_BUFFER_SIZE);
    if (0 < length) {
        gBlockHandler->fillBuffer(gDataBufferPtr, length);
        gBlockHandler->start(gFileReader->getBlockType());
        writeToPort();
    }
}

void drawProgress() {
    BMPDrawer::drawProgress(gScreen, static_cast<float>(gFileReader->getFilePosition()) / gFileReader->getFileSize());
}

void drawProgressWithClear() {
    BMPDrawer::drawProgressWithClear(gScreen, static_cast<float>(gFileReader->getFilePosition()) / gFileReader->getFileSize());
}

void (*int0Functor)() = nullptr;
void (*int1Functor)() = nullptr;

void FileReaderInt0Handler(void) {
    gFileReader->setPreviousBlock();
    drawProgressWithClear();
}

void FileReaderInt1Handler(void) {
    gFileReader->setNextBlock();
    drawProgressWithClear();
}

volatile bool gFlipFlop = false;
bool gButtonIsClicked = false; // this is local, but save flash

void startReading(const char *path) {
    byte localDataBuffer[TAPE_BUFFER_SIZE];

    FileReader localFileRieader(path);
    BlockHandler localBlockHandler(ROOT); // ROOT is not needed

    gFileReader = static_cast<FileReader*>(&localFileRieader);
    gBlockHandler = static_cast<BlockHandler*>(&localBlockHandler);
    gDataBufferPtr = &localDataBuffer[0];

    initReading();
    gFlipFlop = false;
    gButtonIsClicked = false;
    while (true) {
        if (PIND & B00100000) {
            gButtonIsClicked = false;
        } else {
            if (!gButtonIsClicked) {
                gButtonIsClicked = true;
                if (!gFlipFlop) {
                    gFileReader->setPause();
                    gBlockHandler->init();
                    BMPDrawer::drawPause(gScreen);
                    int0Functor = FileReaderInt0Handler;
                    int1Functor = FileReaderInt1Handler;
                    enableExceptions();
                } else {
                    disableExceptions();
                    int0Functor = nullptr;
                    int1Functor = nullptr;
                    BMPDrawer::cleanPause(gScreen);
                    gFileReader->readContinue(true);
                    initReading();
                }
                gFlipFlop = !gFlipFlop;
            }
        }
        if (!(gFileReader->isPause())) {
            if (!(gBlockHandler->isFinished()) && gBlockHandler->isBufferEmpty()) {
                uint16_t length = gFileReader->getData(gDataBufferPtr, TAPE_BUFFER_SIZE);
                if (length > 0) {
                    gBlockHandler->fillBuffer(gDataBufferPtr, length);
                }
                drawProgress();
            }
        }
        if (gFileReader->isFinished() && gBlockHandler->isFinished()) {
            break;
        }
    }
    gFileReader = nullptr;
}

tPath gPathFile = "/";
uint16_t gPosition = 0;

void loop() {
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
    if (!gFileReader || gFlipFlop) {
        return;
    }
    if (gBlockHandler->isFinished()) {
        if (gFileReader->isPause()) {
            if (dc.enabled()) {
                if (dc.check(Timer1::instance().duration())) {
                    gFileReader->readContinue();
                    initReading();
                }
            } else {
                dc.set(DURATION_PAUSE);
                Timer1::instance().start(DURATION_PAUSE);
            }
        }
    } else {
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
