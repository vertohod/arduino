#include <MemoryFree.h>

#include "Types.h"
#include "BlockHandler.h"
#include "FileReader.h"
#include "DirReader.h"
#include "MenuDrawer.h"
#include "Timer1.h"
#include "Menu.h"

#define SDPIN 10
#define OUTPUTPIN 4
#define BUFFER_SIZE 32
#define DURATION_PAUSE 2.0 // seconds

BlockHandler *bh = nullptr;
FileReader *reader = nullptr;
Menu *menu = nullptr;

byte* buffer = new byte[BUFFER_SIZE];

void setup()
{
    Serial.begin(9600);

    Serial.print(F("RAM left: "));
    Serial.println(FreeRam());

    string path("/");

    auto dirReader = new DirReader(SDPIN);
    dirReader->setDirectory(path);
    Serial.print(F("label 1: "));
    Serial.println(FreeRam());
    auto menuDrawer = new MenuDrawer(7, 9);
    menuDrawer->setHeader(path);
    Serial.print(F("label 2: "));
    Serial.println(FreeRam());

    menu = new Menu();
    menu->setMenuDrawer(menuDrawer);
    menu->setDataProvider(dirReader);
    menu->setLength(9);

    delete menu;
    delete menuDrawer;
    delete dirReader;

    Serial.print(F("In the end: "));
    Serial.println(FreeRam());
}

void loadFile()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    reader = new FileReader(SDPIN, "tape.tap");
    bh = new BlockHandler(BUFFER_SIZE);

    // Enable INT0
    EICRA = 1 << ISC01 | 1 << ISC00;
    EIMSK = 1 << INT0;

    DDRD = B00010000;

    digitalWrite(LED_BUILTIN, LOW);
}


void loop()
{
    if (reader->isPause()) return;

    if (!bh->isFinished() && bh->isBufferEmpty()) {
        size_t length = reader->getData(buffer, BUFFER_SIZE);
        if (length > 0) {
            buffer = bh->fillBuffer(buffer, length);
        }
    }
}

class DurationCounter
{
private:
    bool mEnabled;
    double mDuration;

public:
    DurationCounter() : mEnabled(false), mDuration(0.0) {}
    void set(double duration)
    {
        mDuration = duration;
        mEnabled = duration != 0.0;
    }
    bool enabled() { return mEnabled; }
    bool check(double period)
    {
        if (mDuration <= period) {
            mEnabled = false;
            return true;
        } else {
            mDuration -= period;
        }
        return false;
    }
};

DurationCounter dc;
bool nextLevelUp = false;
double nextPeriod = 0.0;

void startReading()
{
    size_t length = reader->getData(buffer, BUFFER_SIZE);
    if (length > 0) {
        buffer = bh->fillBuffer(buffer, length);
        bh->start(reader->getBlockType());

        // initialization
        nextLevelUp = bh->getLevel();
        nextPeriod = bh->getPeriod();

        // just launch the process with some period
        Timer1::instance().init(nextPeriod);
        Timer1::instance().start();
    }
}

// -------------- Handlers ----------------

ISR(TIMER1_COMPA_vect)
{
    cli();

    if (reader->isPause() && bh->isFinished()) {
        if (dc.enabled()) {
            if (dc.check(Timer1::instance().duration())) {
                reader->readContinue();
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
    startReading();
}
