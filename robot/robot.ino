#include "Emakefun_MotorDriver.h"
#include <LiquidCrystal_I2C.h>

#include "irremote_code.h"
#include "timer.h"
#include "stm.h"

namespace global {
    Emakefun_MotorDriver motorDriver = Emakefun_MotorDriver(0x60, MOTOR_DRIVER_BOARD_V5);

    timer timer_command(200);
    timer timer_lamp_status_change(50);
    timer timer_lamp_lr_switch(500);
    timer timer_speed_rise(20);
    timer timer_init_controller(500);
    timer timer_read_controller(50);
}
/*
namespace irremote {
    const int RECV_PIN = 33;
    IRrecv irrecv(RECV_PIN);
    decode_results results;
    bool motor_turn_on = false;
}
*/

namespace ps {
    PS2X *controller = NULL;

    bool motor_turn_on = false;
}

namespace lcd {
    const size_t cols = 20;
    const size_t rows = 4;
    LiquidCrystal_I2C display(0x27, cols, rows);
}

namespace motor {
    const byte speed_max = 255;
    const byte speed_rise = 5;
    unsigned int speed_r_current = 0;
    unsigned int speed_l_current = 0;
    unsigned int speed_r_current_max = speed_max;
    unsigned int speed_l_current_max = speed_max;

    Emakefun_EncoderMotor *mtr_r = NULL;
    Emakefun_EncoderMotor *mtr_l = NULL; 
}

namespace headlamp {
    const int lamp_l = 44;
    const int lamp_r = 40;

    bool headlamp_on = false;
    bool lamp_status_on = false;
    bool lamp_on_lr = false;
}

stm st(lcd::display, lcd::cols, lcd::rows - 1);

void change_settings_of_motors(uint8_t motor_r_dir, uint8_t motor_r_speed, uint8_t motor_l_dir, uint8_t motor_l_speed)
{
    motor::mtr_r->setSpeed(motor_r_speed);
    motor::mtr_r->run(motor_r_dir);

//    motor::mtr_l->setSpeed(motor_l_speed == RELEASE ? RELEASE : (motor_l_speed == FORWARD ? BACKWARD : FORWARD));
    motor::mtr_l->setSpeed(motor_l_speed);
    motor::mtr_l->run(motor_l_dir == RELEASE ? RELEASE : (motor_l_dir == FORWARD ? BACKWARD : FORWARD));
}

void print_speed(byte speed_r, byte speed_l)
{
    lcd::display.setCursor(0, 3);
    lcd::display.print("l:   ");
    lcd::display.setCursor(speed_l < 10 ? 4 : speed_l < 100 ? 3 : 2, 3);
    lcd::display.print(speed_l);

    lcd::display.setCursor(15, 3);
    lcd::display.print("r:   ");
    lcd::display.setCursor(speed_r < 10 ? 19 : speed_r < 100 ? 18 : 17, 3);
    lcd::display.print(speed_r);
}

void setup()
{
    global::motorDriver.begin();

    motor::mtr_r = global::motorDriver.getEncoderMotor(1);
    motor::mtr_l = global::motorDriver.getEncoderMotor(2);

    motor::mtr_r->setSpeed(0);
    motor::mtr_r->run(RELEASE);

    motor::mtr_l->setSpeed(0);
    motor::mtr_l->run(RELEASE);

    ps::controller = global::motorDriver.getSensor(E_PS2X);
/*
    irremote::irrecv.enableIRIn();
    irremote::irrecv.blink13(false);
    */

    lcd::display.init();
    lcd::display.backlight();

    pinMode(headlamp::lamp_l, OUTPUT);
    pinMode(headlamp::lamp_r, OUTPUT);
}

void loop()
{
    auto time_current = millis();
/*
    // IR remote ------------------------------------------
    if (irremote::irrecv.decode(&irremote::results)) {
        auto code = irremote::results.value;
        char command = translate_code(code);
        irremote::irrecv.resume();

        if (command == 'c') {
            global::timer_command.update(time_current);
        } else if (command == 't') {
            change_settings_of_motors(FORWARD, motor::speed_r_current, FORWARD, motor::speed_l_current);
            irremote::motor_turn_on = true;
            global::timer_command.update(time_current);
            st << "Command: " << stm::dec << command << stm::endl;
        } else if (command == 'd') {
            change_settings_of_motors(BACKWARD, motor::speed_r_current, BACKWARD, motor::speed_l_current);
            irremote::motor_turn_on = true;
            global::timer_command.update(time_current);
            st << "Command: " << stm::dec << command << stm::endl;
        } else if (command == 'l') {
            change_settings_of_motors(FORWARD, motor::speed_r_current, BACKWARD, motor::speed_l_current);
            irremote::motor_turn_on = true;
            global::timer_command.update(time_current);
            st << "Command: " << stm::dec << command << stm::endl;
        } else if (command == 'r') {
            change_settings_of_motors(BACKWARD, motor::speed_r_current, FORWARD, motor::speed_l_current);
            irremote::motor_turn_on = true;
            global::timer_command.update(time_current);
            st << "Command: " << stm::dec << command << stm::endl;
        } else if (command == 'o') {
            headlamp::headlamp_on = !headlamp::headlamp_on;
            if (!headlamp::headlamp_on) {
                digitalWrite(headlamp::lamp_l, LOW);
                digitalWrite(headlamp::lamp_r, LOW);
            }
        } else if (command == '1') {
            motor::speed_r_current_max = 50;
            motor::speed_l_current_max = 50;
        } else if (command == '2') {
            motor::speed_r_current_max = 100;
            motor::speed_l_current_max = 100;
        } else if (command == '3') {
            motor::speed_r_current_max = 150;
            motor::speed_l_current_max = 150;
        } else if (command == '4') {
            motor::speed_r_current_max = 200;
            motor::speed_l_current_max = 200;
        } else if (command == '5') {
            motor::speed_r_current_max = motor::speed_max;
            motor::speed_l_current_max = motor::speed_max;
        }
    }
    // ----------------------------------------------------
*/

    // PS -------------------------------------------------
    if (global::timer_read_controller.check(time_current)) {
        ps::controller->read_gamepad(false, 0);

        int ps_ly = ps::controller->Analog(PSS_LY);
        int ps_rx = ps::controller->Analog(PSS_RX);

        int speed_r_temp = -(ps_ly - 127);
        int speed_l_temp = -(ps_ly - 127);
        int rx_127 = ps_rx - 127;

        uint8_t direction_r_temp = RELEASE;
        uint8_t direction_l_temp = RELEASE;

        if (abs(speed_r_temp) < 5) speed_r_temp = 0;
        if (abs(speed_l_temp) < 5) speed_l_temp = 0;
        if (abs(rx_127) < 5) rx_127 = 0;

        if (speed_r_temp != 0 || speed_l_temp != 0 || rx_127 != 0) {
            if (speed_r_temp != 0 || speed_l_temp != 0) {
                speed_r_temp = speed_r_temp - (rx_127 > 0 ? abs(rx_127) : 0) * (speed_r_temp > 0 ? 1 : -1);
                speed_l_temp = speed_l_temp - (rx_127 < 0 ? abs(rx_127) : 0) * (speed_l_temp > 0 ? 1 : -1);
            } else {
                speed_r_temp = -rx_127;
                speed_l_temp = rx_127;
            }
            direction_r_temp = speed_r_temp > 0 ? FORWARD : BACKWARD;
            direction_l_temp = speed_l_temp > 0 ? FORWARD : BACKWARD;
            speed_r_temp = abs(speed_r_temp * 2);
            speed_l_temp = abs(speed_l_temp * 2);
            if (speed_r_temp > 255) speed_r_temp = 255;
            if (speed_l_temp > 255) speed_l_temp = 255;

            change_settings_of_motors(direction_r_temp, speed_r_temp, direction_l_temp, speed_l_temp);
            st << stm::dec << "l:" << speed_l_temp << " r:" << speed_r_temp << stm::endl;

            ps::motor_turn_on = true;
            print_speed(speed_r_temp, speed_l_temp);

        } else if (ps::motor_turn_on) {
            change_settings_of_motors(RELEASE, 0, RELEASE, 0);
            ps::motor_turn_on = false;
        }
    }
    // ----------------------------------------------------

    if (global::timer_command.check_less(time_current)) {
        if (global::timer_speed_rise.check(time_current)) {
            motor::speed_r_current += motor::speed_rise;
            if (motor::speed_r_current > motor::speed_r_current_max) {
                motor::speed_r_current = motor::speed_r_current_max;
            }
            motor::speed_l_current += motor::speed_rise;
            if (motor::speed_l_current > motor::speed_l_current_max) {
                motor::speed_l_current = motor::speed_l_current_max;
            }
            motor::mtr_r->setSpeed(motor::speed_r_current);
            motor::mtr_l->setSpeed(motor::speed_l_current);
        }
    }
    /*
    else if (irremote::motor_turn_on) {
        motor::speed_r_current = 0;
        motor::speed_l_current = 0;
        change_settings_of_motors(RELEASE, motor::speed_r_current, RELEASE, motor::speed_l_current);
        irremote::motor_turn_on = false;
    }
    */

    if (headlamp::headlamp_on && global::timer_lamp_status_change.check(time_current)) {
        headlamp::lamp_status_on = !headlamp::lamp_status_on;

        if (global::timer_lamp_lr_switch.check(time_current)) {
            headlamp::lamp_on_lr = !headlamp::lamp_on_lr;
        }
        digitalWrite(headlamp::lamp_l, (headlamp::lamp_status_on && headlamp::lamp_on_lr) ? HIGH : LOW);
        digitalWrite(headlamp::lamp_r, (headlamp::lamp_status_on && !headlamp::lamp_on_lr) ? HIGH : LOW);
    }
}
