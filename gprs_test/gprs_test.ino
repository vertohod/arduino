#include "vector.h"
#include "cobject.h"
#include "cobject_ext.h"
#include <SoftwareSerial.h>

SoftwareSerial gsmModem(6, 7); // RX, TX

cobject* main_object = nullptr;

typedef vector<char> type_buffer;

#define CREATE_OBJECT(OBJ, ARG1, ARG2, ARG3) { \
    auto temp_object = new cobject_ext<request, response<type_buffer>, timeout>(ARG1, ARG2, ARG3); \
    OBJ->add_object(temp_object); }

class request
{
public:
    void operator()(char* const arg)
    {
        gsmModem.println(arg);
    }
};

template <class BUFFER>
class response
{
public:
    unsigned int operator()(BUFFER& buffer)
    {
        unsigned int counter = 0;

        while (gsmModem.available()) {
            char ch = gsmModem.read();
            Serial.write(ch);
            buffer.push_back(ch);
            ++counter;
        }

        return counter;
    }
};

class timeout
{
public:
    bool operator()(unsigned int arg)
    {
        static unsigned long last_time = 0;

        if (last_time == 0) {
            last_time = millis();
            return false;
        }

        auto time = millis();
        if (last_time + arg < time) {
            last_time = time;
            return true;
        }

        return false;
    }
};

void setup()
{
    Serial.begin(9600);
    gsmModem.begin(9600);

    main_object = new cobject();
    CREATE_OBJECT(main_object, "AT", "OK", 1000)
    CREATE_OBJECT(main_object, "AT", "OK", 1000)
    CREATE_OBJECT(main_object, "AT", "OK", 1000)
    CREATE_OBJECT(main_object, "AT", "OK", 1000)

    delay(1000);
    Serial.println("Ready");
}

bool flag_print_finish = false;

void loop()
{
    if (main_object && !main_object->is_completed()) {
        main_object->execute();
    } else if (!flag_print_finish) {
        Serial.println("Finish");
        flag_print_finish = true;

        main_object->clean();
        delete main_object;
        main_object = nullptr;

        Serial.println("The main object is cleaned");
    }
}
