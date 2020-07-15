#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include "timer.h"

/*
   This sample code demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
   */
static const int RXPin = 7, TXPin = 6;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
LiquidCrystal_I2C lcd(0x27,20,4);

timer read_gps_data_timer(1000);
timer paint_satellite_timer(300);

uint8_t conveyer[8] = {0x04,0x04,0x04,0x0e,0x04,0x04,0x04};
uint8_t slash[8]    = {0x00,0x01,0x0a,0x04,0x0a,0x10,0x00};
uint8_t dash[8]     = {0x00,0x00,0x04,0x1f,0x04,0x00,0x00};
uint8_t bslash[8]   = {0x00,0x10,0x0a,0x04,0x0a,0x01,0x00};

void setup()
{
    ss.begin(GPSBaud);

    lcd.init();
    lcd.backlight();

    lcd.createChar(0, conveyer);
    lcd.createChar(1, slash);
    lcd.createChar(2, dash);
    lcd.createChar(3, bslash);
}

void loop()
{
    auto time_current = millis();

    if (paint_satellite_timer.check(time_current)) paintSat();

    if (read_gps_data_timer.check(time_current)) {
        printSat(gps.satellites.value(), gps.satellites.isValid());
        printKmph(gps.speed.mph(), gps.speed.isValid());
        printLat(gps.location.lat(), gps.location.isValid());
        printAltitude(gps.altitude.meters(), gps.altitude.isValid());
        printLon(gps.location.lng(), gps.location.isValid());
        printCourse(gps.course.deg(), gps.course.isValid());
        printDateTime(gps.date, gps.time);
    }

    while (ss.available()) gps.encode(ss.read());
}

static void paintSat()
{
    static const char sat[] = {0x00, 0x01, 0x02, 0x03};
    static size_t index1 = 0;
    static size_t index2 = sizeof(sat) - 1;

    lcd.setCursor(0, 0);
    lcd.print(sat[index1]);
    lcd.setCursor(1, 0);
    lcd.print(sat[index2]);

    if (index1 == sizeof(sat) - 1) index1 = 0; else ++index1;
    if (index2 == 0) index2 = sizeof(sat) - 1;
    else --index2;
}

static void printSat(unsigned long val, bool valid)
{
    char res[10];
    for (size_t i = 0; i < sizeof(res) - 1; ++i) res[i] = ' ';
    res[sizeof(res) - 1] = 0;

    lcd.setCursor(3, 0);
    if (valid) {
        sprintf(res, "%d  ", val);
    } else {
        sprintf(res, "%d  ", 0);
    }
    lcd.print(res);  
}

#define ABS(x) ((x) < 0.0 ? -(x) : (x))

static void printDouble(double val, char* str)
{
    char *tmpSign = (val < 0) ? "-" : "";
    double tmpVal = ABS(val); 

    size_t tmpInt = trunc(tmpVal);
    double tmpFrac = tmpVal - tmpInt;
    size_t tmpIntHi = trunc(tmpFrac * 1000);
    size_t tmpIntLo = trunc((tmpFrac * 1000 - tmpIntHi) * 1000);

    sprintf (str, "%s%d.%03d%03d ", tmpSign, tmpInt, tmpIntHi, tmpIntLo);
}

static void printKmph(double val, bool valid)
{
    lcd.setCursor(7, 0);
    lcd.print("Speed:       ");

    static char res[8];
    for (size_t i = 0; i < sizeof(res) - 1; ++i) res[i] = '*';
    res[sizeof(res) - 1] = 0;

    lcd.setCursor(13, 0);

    if (valid) {
        size_t tmpInt = ABS(trunc(val));
        sprintf(res, " %s%d %s", tmpInt < 10 ? "  " : tmpInt < 100 ? " " : "", tmpInt, "Km");
    }
    lcd.print(res);
}

static void printAltitude(double val, bool valid)
{
    lcd.setCursor(11, 1);
    lcd.print("A:       ");

    static char res[8];
    for (size_t i = 0; i < sizeof(res) - 1; ++i) res[i] = '*';
    res[sizeof(res) - 1] = 0;

    lcd.setCursor(13, 1);

    if (valid) {
        static char* sign = " ";
        if (val < 0.0) sign[0] = '-'; else sign[0] = ' ';
        size_t tmpInt = ABS(trunc(val));
        sprintf(res, "%s%s%d %s", tmpInt < 10 ? "  " : tmpInt < 100 ? " " : "", sign, tmpInt, "m ");
    }
    lcd.print(res);
}

static void printCourse(double val, bool valid)
{
    lcd.setCursor(11, 2);
    lcd.print("C:       ");

    static char res[8];
    for (size_t i = 0; i < sizeof(res) - 1; ++i) res[i] = '*';
    res[sizeof(res) - 1] = 0;

    lcd.setCursor(13, 2);

    if (valid) {
        static char* sign = " ";
        if (val < 0.0) sign[0] = '-'; else sign[0] = ' ';
        int tmpInt1 = trunc(val);
        int tmpInt2 = trunc((val - tmpInt1) * 100);
        tmpInt1 = ABS(tmpInt1);
        tmpInt2 = ABS(tmpInt2);
        sprintf(res, "%s%s%d.%02d", tmpInt1 < 10 ? "  " : tmpInt1 < 100 ? " " : "", sign, tmpInt1, tmpInt2);
    }
    lcd.print(res);
}

static void printLat(double val, bool valid)
{
    char res[20];
    for (size_t i = 0; i < sizeof(res) - 1; ++i) res[i] = ' ';
    res[sizeof(res) - 1] = 0;

    lcd.setCursor(0, 1);
    if (valid) {
        printDouble(val, res);
        lcd.print(res);
    } else {
        lcd.print("*********");
    }
}

static void printLon(double val, bool valid)
{
    char res[20];
    for (size_t i = 0; i < sizeof(res) - 1; ++i) res[i] = ' ';
    res[sizeof(res) - 1] = 0;

    lcd.setCursor(0, 2);
    if (valid) {
        printDouble(val, res);
        lcd.print(res);
    } else {
        lcd.print("*********");
    }
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
    lcd.setCursor(0, 3);

    if (!d.isValid()) {
        lcd.print("******** ");
    } else {
        char res[32];
        sprintf(res, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
        lcd.print(res);
    }

    lcd.setCursor(11, 3);

    if (!t.isValid()) {
        lcd.print("******** ");
    } else {
        char res[32];
        sprintf(res, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
        lcd.print(res);
    }
}
