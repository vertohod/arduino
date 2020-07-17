#include <SoftwareSerial.h>
#include "cobject.h"

SoftwareSerial gsmModem(6, 7); // RX, TX

void setup()
{
    Serial.begin(115200);
    gsmModem.begin(115200);

    Serial.println("Ready...");

    cobject obj[1000];
}

void loop()
{
    if (Serial.available()) {
        char ch = Serial.read();
        if (ch == '*') {
            gsmModem.write(0x1a);
        } else if (ch == '\n') {
            // send nothing
        } else if (ch == '\n') {
            gsmModem.write('\r');
            gsmModem.write('\n');
        } else {
            gsmModem.write(ch);
        }
    }
    if (gsmModem.available()) {
        char ch = gsmModem.read();
        /*
        if (ch == '\r') {
            // send nothing
        } else if (ch == '\n') {
            Serial.write('\r');
            Serial.write('\n');
        } else {
        */
            Serial.write(ch);
//        }
    }
}
