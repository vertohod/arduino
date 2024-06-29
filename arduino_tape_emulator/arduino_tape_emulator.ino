#include <MemoryFree.h>

#include "Types.h"
#include "BlockHandler.h"
#include "FileReader.h"
#include "DirReader.h"
#include "MenuDrawer.h"
#include "Timer1.h"
#include "Menu.h"
#include "DurationCounter.h"

#define SD_CS 10
#define TFT_DC 9
#define TFT_CS 7
#define OUTPUT_PIN 4
#define BUFFER_SIZE 32
#define DURATION_PAUSE 2.0 // seconds
#define TEXT_SIZE 2

string currentDirectory("/");
Menu *menu = nullptr;
FileReader *fileReader = nullptr;
BlockHandler *blockHandler = nullptr;
volatile byte* dataBuffer = nullptr;

void setup() {
    Serial.begin(115200);
    Serial.println(F("******** setup *********"));
    Serial.print(F("RAM left: "));
    Serial.println(FreeRam());

    dataBuffer = new byte[BUFFER_SIZE]; // never free;

    // Enable output to port D
    DDRD = B00010000;

    // Enable INT0, INT1
    EICRA = 1 << ISC11 | 1 << ISC10 | 1 << ISC01 | 1 << ISC00;
    EIMSK = 1 << INT1 | 1 << INT0;

    showMenu(currentDirectory);
}

void loadFile(const string& path)
{
    fileReader = new FileReader(SD_CS, path);
    blockHandler = new BlockHandler(BUFFER_SIZE);
}

DurationCounter dc;
bool nextLevelUp = false;
double nextPeriod = 0.0;

void startReading()
{
    size_t length = fileReader->getData(dataBuffer, BUFFER_SIZE);
    Serial.print(F("(startReading) length: "));
    Serial.println(length);
    if (length > 0) {
        dataBuffer = blockHandler->fillBuffer(dataBuffer, length);
        blockHandler->start(fileReader->getBlockType());

        // initialization
        nextLevelUp = blockHandler->getLevel();
        nextPeriod = blockHandler->getPeriod();

        // just launch the process with some period
        Timer1::instance().init(nextPeriod);
        Timer1::instance().start();
    }
}

bool buttonIsClicked = false;

void loop()
{
    if (nullptr != fileReader && nullptr != blockHandler) {
        if (!(fileReader->isPause())) {
            if (!(blockHandler->isFinished()) && blockHandler->isBufferEmpty()) {
                size_t length = fileReader->getData(dataBuffer, BUFFER_SIZE);
                if (length > 0) {
                    dataBuffer = blockHandler->fillBuffer(dataBuffer, length);
                }
            }
        }
        if (fileReader->isFinished() && blockHandler->isFinished()) {
            delete fileReader;
            fileReader = nullptr;
            delete blockHandler;
            blockHandler = nullptr;
            showMenu(currentDirectory);
        }
    }
    if (!(PIND & B00100000)) {
        if (!buttonIsClicked) {
            buttonIsClicked = true;
            clickHandler();
        }
    } else {
        buttonIsClicked = false;
    }
}

void showMenu(const string& path) {
    auto dirReader = new DirReader(SD_CS);
    dirReader->setDirectory(path);
    auto menuDrawer = new MenuDrawer(TFT_CS, TFT_DC, TEXT_SIZE);
    menuDrawer->setHeader(path);
    menu = new Menu(dirReader, menuDrawer);
    Serial.print(F("RAM left: "));
    Serial.println(FreeRam());
}

void removeMenu() {
    if (menu) {
        delete menu;
        menu = nullptr;
        Serial.print(F("RAM left: "));
        Serial.println(FreeRam());
    }
}

void clickHandler() {
    if (nullptr != menu) {
        auto directory = menu->getDirectory();
        auto fileName = menu->getChosenItem();
        Serial.print(F("(clickHandler) fileName: "));
        Serial.println(fileName.c_str());
        removeMenu(); // free memory
        if (fileName == string("..")) {
            Serial.println(F("(clickHandler) this is two points"));
            size_t lastSlashIndex = 0;
            size_t prevSlashIndex = 0;
            for (size_t i = 0; i < directory.length(); ++i) {
                if (directory.c_str()[i] == '/') {
                    if (0 != lastSlashIndex) {
                        prevSlashIndex = lastSlashIndex;
                    }
                    lastSlashIndex = i;
                }
            }
            directory.truncate(prevSlashIndex + 1);
            currentDirectory = directory; 
            Serial.print(F("(clickHandler) currentDirectory (out): "));
            Serial.println(currentDirectory.c_str());
            showMenu(currentDirectory);
        } else if (fileName.length() > 0 && fileName.c_str()[fileName.length() - 1] == '/') {
            Serial.println(F("(clickHandler) this is directory"));
            currentDirectory = directory + fileName; 
            Serial.print(F("(clickHandler) currentDirectory (in): "));
            Serial.println(currentDirectory.c_str());
            showMenu(currentDirectory);
        } else {
            Serial.println(F("(clickHandler) this is file"));
            auto fullPath = directory + fileName;
            Serial.print(F("(clickHandler) read file: "));
            Serial.println(fullPath.c_str());
            loadFile(fullPath);
            startReading();
        }
    }
}

// -------------- Handlers ----------------

ISR(TIMER1_COMPA_vect)
{
    if (fileReader->isPause() && blockHandler->isFinished()) {
        if (dc.enabled()) {
            if (dc.check(Timer1::instance().duration())) {
                fileReader->readContinue();
                startReading();
            }
        } else {
            dc.set(DURATION_PAUSE);
            Timer1::instance().init(DURATION_PAUSE);
            Timer1::instance().start();

            sei();
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

volatile bool encoderInt0 = false;
volatile bool encoderInt1 = false;
volatile bool handlerInProcess = false;

ISR(INT0_vect)
{
    if (!handlerInProcess) {
        handlerInProcess = true;
        if (encoderInt1) {
            encoderInt1 = false;
            if (menu) {
                menu->stepUp();
                encoderInt0 = false;
                encoderInt1 = false;
            }
        } else {
            encoderInt0 = true;
        }
        handlerInProcess = false;
    }
}

ISR(INT1_vect)
{
    if (!handlerInProcess) {
        handlerInProcess = true;
        if (encoderInt0) {
            encoderInt0 = false;
            if (menu) {
                menu->stepDn();
                encoderInt0 = false;
                encoderInt1 = false;
            }
        } else {
            encoderInt1 = true;
        }
        handlerInProcess = false;
    }
}
