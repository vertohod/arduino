#ifndef DURATIONCOUNTER_H
#define DURATIONCOUNTER_H

class DurationCounter
{
private:
    bool mEnabled;
    float mDuration;

public:
    DurationCounter() : mEnabled(false), mDuration(0.0){}
    void set(float duration) {
        mDuration = duration;
        mEnabled = duration != 0.0;
    }
    bool enabled() {
        return mEnabled;
    }
    bool check(float period) {
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
