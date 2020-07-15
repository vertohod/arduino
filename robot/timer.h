#ifndef TIMER_H
#define TIMER_H

class timer
{
private:
    const unsigned long time_wait;
    unsigned long time_in_millis;

public:
    timer(unsigned long time_wait_arg, unsigned long time_in_millis_arg = 0) :
        time_wait(time_wait_arg), time_in_millis(time_in_millis_arg) {}

    bool check(unsigned long time_current)
    {
        if (time_current - time_in_millis >= time_wait) {
            time_in_millis = time_current;
            return true;
        }
        return false;
    }

    bool check_more(unsigned long time_current)
    {
        return time_current - time_in_millis > time_wait;
    }

    bool check_less(unsigned long time_current)
    {
        return time_current - time_in_millis < time_wait;
    }

    void update(unsigned long time_current)
    {
        time_in_millis = time_current;
    }
};

#endif
