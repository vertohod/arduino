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
    unsigned int operator()(BUFFER& buffer, char* const str)
    {
        unsigned int counter = 0;

        while (gsmModem.available()) {
            char ch = gsmModem.read();

            Serial.print(ch);
            Serial.flush();

            buffer.push_back(ch);
            ++counter;
        }

        // DEBUG FIXME
        /*
        Serial.print("Buffer: ");
        for (unsigned int i = 0; i < buffer.size(); ++i) {
            Serial.print(buffer[i]);
        }
        Serial.println("");
        Serial.print("Waiting for string: ");
        Serial.println(str);
        */
        // ---------

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
    CREATE_OBJECT5(main_object, "AT", "OK", 1000, true, 50)
    CREATE_OBJECT(main_object, "ATE0", "OK", 1000)
    CREATE_OBJECT(main_object, "AT+CMEE=2", "OK", 1000)     // make more information of error
    auto sim_connect = new cobject();
    main_object->add_object(sim_connect);
    CREATE_OBJECT(sim_connect, "AT+CPIN=?", "OK", 1000)     // check command
    CREATE_OBJECT(sim_connect, "AT+CPIN?", "READY", 1000)   // check a sim card
    CREATE_OBJECT(sim_connect, "AT+CREG=?", "+CREG", 1000)  // list of value for registration in network
    CREATE_OBJECT(sim_connect, "AT+CREG=2", "OK", 1000)     // registration in network
    CREATE_OBJECT(sim_connect, "AT+CREG?", "+CREG", 1000)   // check registration
    CREATE_OBJECT(sim_connect, "AT+CSQ=?", "+CSQ", 1000)    // check signal's quality: +CSQ: (0-31,99),(0-7,99)
    CREATE_OBJECT(sim_connect, "AT+CSQ", "OK", 1000)        // check signal's quality: +CSQ: 13, 99
    auto attach_network = new cobject();
    sim_connect->add_object(attach_network);
    CREATE_OBJECT(attach_network, "AT+CGATT=1", "OK", 5000) // attach to network
    CREATE_OBJECT5(attach_network, "AT+CGATT?", "+CGATT:1", 5000, true, 10)   // check is it attached
    auto activate_pdp = new cobject();
    attach_network->add_object(activate_pdp);
    CREATE_OBJECT(activate_pdp, "AT+CGACT=?", "OK", 5000)   // check supported PDP context
    CREATE_OBJECT(activate_pdp, "AT+CGACT=1", "OK", 5000)   // activate PDP context
    CREATE_OBJECT5(activate_pdp, "AT+CGACT?", "+CGACT: 1", 5000, true, 10)  // check is it activated
    auto define_context = new cobject();
    activate_pdp->add_object(define_context);
    CREATE_OBJECT(define_context, "AT+CGDCONT=1,\"IP\",\"web.vodafone.de\"","OK", 5000)    // define PDP context
    CREATE_OBJECT5(define_context, "AT+CGDCONT?","OK", 5000, true, 5)       // check is it defined
    auto http_connect = new cobject();
    define_context->add_object(http_connect);
    CREATE_OBJECT(activate_pdp, "AT+CGACT=0", "OK", 5000)   // deactivate PDP context
    CREATE_OBJECT(attach_network, "AT+CGATT=0", "OK", 5000) // dettach from network
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

    delay(5000);
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
