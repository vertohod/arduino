#include "SwitchExceptions.h"
#include "DurationCounter.h"
#include "Definitions.h"
#include "FileReader.h"
#include "BMPDrawer.h"
#include "Timer1.h"
#include "Menu.h"
#include "SD.h"

#include <SPI.h>

Adafruit_ILI9341 gScreen(TFT_CS, TFT_DC);
SD gSD;

void setup() {
    if (!gSD.begin(SD_CS)) {
        while(true);
    }
    // Enable output to port D
    DDRD = B00010000;
    gScreen.begin();
    gScreen.setTextSize(TEXT_SIZE);
}

FileReader *gFileReader = nullptr;

void writeToPort() {
    PORTD = gFileReader->getLevel() ? 0xff : 0x00;
    Timer1::instance().start(gFileReader->getPeriod());
}

void initReading() {
    if (gFileReader->start()) {
        writeToPort();
    }
}

void drawProgress(bool withClear = false) {
    BMPDrawer::drawProgress(gScreen, withClear, static_cast<float>(gFileReader->getFilePosition()) / gFileReader->getFileSize());
}

void (*int0Functor)() = nullptr;
void (*int1Functor)() = nullptr;

void FileReaderInt0Handler(void) {
    gFileReader->setPreviousBlock();
    drawProgress(true);
}

void FileReaderInt1Handler(void) {
    gFileReader->setNextBlock();
    drawProgress(true);
}

volatile bool gFlipFlop = false;
bool gButtonIsClicked = false; // this is local, but save flash

void startReading(const char *path) {
    FileReader localFileRieader(gSD, path);

    gFileReader = static_cast<FileReader*>(&localFileRieader);

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
        if (gFileReader->isPause() == false) {
            gFileReader->moveData();
            drawProgress();
        }
        if (gFileReader->isFinished()) {
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
    bool isNeededToLoad = getPathFile(gSD, gScreen, gPathFile, gPosition);

    int0Functor = BMPDrawerInt0Handler;
    int1Functor = BMPDrawerInt1Handler;
    drawBMP(gSD, gScreen, gPathFile, isNeededToLoad);

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
    if (gFileReader->isBlockHandlerFinished()) {
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
