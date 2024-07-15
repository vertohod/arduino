#include "DurationCounter.h"
#include "BlockHandler.h"
#include "FileReader.h"
#include "BMPDrawer.h"
#include "Timer1.h"
#include "Menu.h"

#include "SPI.h"
#include "SD.h"

#define TFT_CS 7
#define TFT_DC 9
#define SD_CS 10
#define OUTPUT_PIN 4
#define DURATION_PAUSE 2.0 // seconds
#define TEXT_SIZE 2
#define ROOT "/"

Adafruit_ILI9341 *gScreenPtr = nullptr;

void setup() {
    Serial.begin(115200);
    Serial.println(F("setup"));

    gScreenPtr = new Adafruit_ILI9341(TFT_CS, TFT_DC);
    if (!SD.begin(SD_CS)) {
        while(true);
    }
    // Enable output to port D
    DDRD = B00010000;
}

FileReader *fileReader = nullptr;
BlockHandler *blockHandler = nullptr;
bool nextLevelUp = false;
double nextPeriod = 0.0;
byte* dataBuffer = nullptr;

void initReading()
{
    size_t length = fileReader->getData(dataBuffer, BUFFER_SIZE);
    if (length > 0) {
        blockHandler->fillBuffer(dataBuffer, length);
        blockHandler->start(fileReader->getBlockType());

        // initialization
        nextLevelUp = blockHandler->getLevel();
        nextPeriod = blockHandler->getPeriod();

        // just launch the process with some period
        Timer1::instance().init(nextPeriod);
        Timer1::instance().start();
    }
}

void startReading(const char *path) {
    byte localDataBuffer[BUFFER_SIZE];

    FileReader localFileRieader(path);
    BlockHandler localBlockHandler(ROOT); // ROOT is not needed

    fileReader = static_cast<FileReader*>(&localFileRieader);
    blockHandler = static_cast<BlockHandler*>(&localBlockHandler);
    dataBuffer = localDataBuffer;

    initReading();
    while (true) {
        if (!(localFileRieader.isPause())) {
            if (!(blockHandler->isFinished()) && blockHandler->isBufferEmpty()) {
                size_t length = localFileRieader.getData(dataBuffer, BUFFER_SIZE);
                if (length > 0) {
                    blockHandler->fillBuffer(dataBuffer, length);
                }
            }
        }
        if (localFileRieader.isFinished() && blockHandler->isFinished()) {
            break;
        }
    }
}

void loop()
{
    char* path = getPathFile(gScreenPtr, "/");

    drawBMP(gScreenPtr, path);
    startReading(path);
}

// ---------- Interuptrs Handlers ------------
DurationCounter dc;

ISR(TIMER1_COMPA_vect)
{
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
