#include <Arduino.h>
#include "SwitchExceptions.h"

void enableExceptions() {
    // Enable INT0, INT1
    EICRA = 1 << ISC11 | 1 << ISC10 | 1 << ISC01 | 1 << ISC00;
    EIMSK = 1 << INT1 | 1 << INT0;
}

void disableExceptions() {
    // Disable INT0, INT1
    EICRA = 0; 
    EIMSK = 0; 
}


