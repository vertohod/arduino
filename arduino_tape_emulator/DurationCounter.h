#ifndef DURATIONCOUNTER_H
#define DURATIONCOUNTER_H

class DurationCounter
{
private:
    bool mEnabled;
    double mDuration;

public:
    DurationCounter() : mEnabled(false), mDuration(0.0){}
    void set(double duration) {
        mDuration = duration;
        mEnabled = duration != 0.0;
    }
    bool enabled() {
        return mEnabled;
    }
    bool check(double period) {
        if (mDuration <= period) {
            mEnabled = false;
            return true;
        } else {
            mDuration -= period;
        }
        return false;
    }
};

#endif
