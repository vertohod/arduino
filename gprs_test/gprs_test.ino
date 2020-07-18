#include "vector.h"
#include "cobject.h"
#include "cobject_ext.h"
#include <SoftwareSerial.h>

SoftwareSerial gsmModem(6, 7); // RX, TX

cobject* main_object = nullptr;

typedef vector<char, 8> response_type_buffer;

#define CREATE_OBJECT(OBJ, ARG1, ARG2, ARG3) { \
    auto temp_object = new cobject_ext<request, response<response_type_buffer>, timeout>(ARG1, ARG2, ARG3); \
    OBJ->add_object(temp_object); }

#define CREATE_OBJECT4(OBJ, ARG1, ARG2, ARG3, ARG4) { \
    auto temp_object = new cobject_ext<request, response<response_type_buffer>, timeout, ARG4>(ARG1, ARG2, ARG3); \
    OBJ->add_object(temp_object); }

#define CREATE_OBJECT5(OBJ, ARG1, ARG2, ARG3, ARG4, ARG5) { \
    auto temp_object = new cobject_ext<request, response<response_type_buffer>, timeout, ARG4, ARG5>(ARG1, ARG2, ARG3); \
    OBJ->add_object(temp_object); }

class request
{
public:
    void operator()(char* const arg)
    {
        gsmModem.println(arg);
        Serial.println(arg);
    }
};

template <class BUFFER>
class response
{
public:
    typedef BUFFER type_buffer;

public:
    unsigned int operator()(BUFFER& buffer)
    {
        unsigned int counter = 0;

        while (gsmModem.available()) {
            char ch = gsmModem.read();

            Serial.print(ch);
            Serial.flush();

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

void init_commands()
{
    main_object = new cobject();
    CREATE_OBJECT5(main_object, "AT", "OK", 1000, true, 10)
    CREATE_OBJECT(main_object, "ATE0", "OK", 1000)
}

void free_commands()
{
    main_object->clean();
    delete main_object;
    main_object = nullptr;
}

void setup()
{
    Serial.begin(9600);
    gsmModem.begin(9600);

    init_commands();

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
        free_commands();
        Serial.println("The main object is cleaned");
    }
}
