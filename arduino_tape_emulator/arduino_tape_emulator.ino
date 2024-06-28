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

bool encoderInt0 = false;
bool encoderInt1 = false;

Menu *menu = nullptr;
DirReader *dirReader = nullptr;
MenuDrawer *menuDrawer = nullptr;
FileReader *fileReader = nullptr;
BlockHandler *bh = nullptr;

byte* buffer = new byte[BUFFER_SIZE];

void setup()
{
    Serial.begin(9600);
    Serial.print(F("RAM left: "));
    Serial.println(FreeRam());

    // Enable output to port D
    DDRD = B00010000;

    // Enable INT0, INT1
    EICRA = 1 << ISC11 | 1 << ISC10 | 1 << ISC01 | 1 << ISC00;
    EIMSK = 1 << INT0 | 1 << INT1;

    string path("/");
    dirReader = new DirReader(SD_CS);
    dirReader->setDirectory(path);
    menuDrawer = new MenuDrawer(TFT_CS, TFT_DC, TEXT_SIZE);
    menuDrawer->setHeader(path);
    menu = new Menu(dirReader, menuDrawer);

    Serial.print(F("In the end: "));
    Serial.println(FreeRam());
}

void loadFile(const string& path)
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    fileReader = new FileReader(SD_CS, path.c_str());
    bh = new BlockHandler(BUFFER_SIZE);

    digitalWrite(LED_BUILTIN, LOW);
}

DurationCounter dc;
bool nextLevelUp = false;
double nextPeriod = 0.0;

void startReading()
{
    size_t length = fileReader->getData(buffer, BUFFER_SIZE);
    if (length > 0) {
        buffer = bh->fillBuffer(buffer, length);
        bh->start(fileReader->getBlockType());

        // initialization
        nextLevelUp = bh->getLevel();
        nextPeriod = bh->getPeriod();

        // just launch the process with some period
        Timer1::instance().init(nextPeriod);
        Timer1::instance().start();
    }
}

void loop()
{
    if (fileReader && bh) {
        if (!(fileReader->isPause())) {
            if (!(bh->isFinished()) && bh->isBufferEmpty()) {
                size_t length = fileReader->getData(buffer, BUFFER_SIZE);
                if (length > 0) {
                    buffer = bh->fillBuffer(buffer, length);
                }
            }
        }
    }
    if (PIND & B00100000) {
        if (menu) {
            auto fileName = menu->getChosenItem();
            delete menu;
            menu = nullptr;
            delete menuDrawer;
            menuDrawer = nullptr;
            delete dirReader;
            dirReader = nullptr;

            loadFile(fileName);
            startReading();
        }
    }
}

// -------------- Handlers ----------------

ISR(TIMER1_COMPA_vect)
{
    cli();

    if (fileReader->isPause() && bh->isFinished()) {
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

    if (!bh->isFinished()) {
        PORTD = nextLevelUp ? 0xff : 0x00;
        Timer1::instance().start();

        nextLevelUp = bh->getLevel();
        nextPeriod = bh->getPeriod();

        // initialize the timer now do not waste time
        Timer1::instance().init(nextPeriod);
    }

    sei();
}

ISR(INT0_vect)
{
    cli();
    if (encoderInt1) {
        encoderInt1 = false;
        if (menu) {
            menu->stepUp();
        }
    } else {
        encoderInt1 = true;
    }
    sei();
}

ISR(INT1_vect)
{
    cli();
    if (encoderInt0) {
        encoderInt0 = false;
        if (menu) {
            menu->stepDn();
        }
    } else {
        encoderInt0 = true;
    }
    sei();
}
