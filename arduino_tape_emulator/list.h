#ifndef LIST_H
#define LIST_H

template <class T>
class list {
public:
    class iterator {
    friend list;
    private:
        T mValue;
        iterator*   mPrev;
        iterator*   mNext;
        iterator*   mCurrent;

    public:
        iterator() {
            mPrev = this;
            mNext = this;
            mCurrent = this;
        }
        iterator(const iterator& obj) {
            mValue = obj.mValue;
            mPrev = obj.mPrev;
            mNext = obj.mNext;
            mCurrent = obj.mCurrent;
        }
        iterator& operator=(const iterator& obj) {
            mValue = obj.mValue;
            mPrev = obj.mPrev;
            mNext = obj.mNext;
            mCurrent = obj.mCurrent;
            return *this;
        }
        iterator& operator++() {
            mValue = mNext->mValue;
            mPrev = mNext->mPrev;
            mCurrent = mNext->mCurrent;
            mNext = mNext->mNext;
            return *this;
        }
        iterator operator++(int) {
            auto ret = *this;
            mValue = mNext->mValue;
            mPrev = mNext->mPrev;
            mCurrent = mNext->mCurrent;
            mNext = mNext->mNext;
            return ret;
        }
        iterator& operator--() {
            mValue = mPrev->mValue;
            mNext = mPrev->mNext;
            mCurrent = mPrev->mCurrent;
            mPrev = mPrev->mPrev;
            return *this;
        }
        iterator operator--(int) {
            auto ret = *this;
            mValue = mPrev->mValue;
            mNext = mPrev->mNext;
            mCurrent = mPrev->mCurrent;
            mPrev = mPrev->mPrev;
            return ret;
        }
        bool operator==(const iterator& obj) {
            return mCurrent == obj.mCurrent;
        }
        bool operator!=(const iterator& obj) {
            return !(*this == obj); 
        }
        T& operator*() {
            return mValue;
        }
        T* operator->() {
            return &mValue;
        }
    };

private:
    iterator*   mBegin;
    iterator*   mEnd;
    uint16_t    mSize;

public:
    list() : mSize(0) {
        mEnd = new iterator();
        mBegin = mEnd;
    }

    ~list() {
        clear();
        delete mEnd;
    }

    iterator begin() {
        return *mBegin;
    }

    iterator end() {
        return *mEnd;
    }

    void push(const T& value) {
        auto newItem = new iterator();
        newItem->mValue = value;
        newItem->mNext = mEnd;
        newItem->mPrev = mEnd->mPrev;
        newItem->mPrev->mNext = newItem;
        mEnd->mPrev = newItem;
        if (*mBegin == *mEnd) {
            mBegin = newItem;
        }
        ++mSize;
    }

    void clear() {
        for (auto it = begin(); it != end();) {
            auto eraseIt = it;
            ++it;
            delete eraseIt.mCurrent; 
        }
        mEnd->mPrev = mEnd->mCurrent;
        mEnd->mNext = mEnd->mCurrent;
        mSize = 0;
    }

    uint16_t size() {
        return mSize;
    }

    void splice(iterator position, list& otherList, iterator otherIt) {
        Serial.println(F("(splice)"));
        // check if otherIt valid iterator
        if (otherList.end() == otherIt) {
            Serial.println(F("(splice) otherList.end() == otherIt"));
            return;
        }
        // cut out an item
        if (otherList.begin() == otherIt) {
            Serial.println(F("(splice) otherList.begin() == otherIt"));
            otherList.mBegin = otherIt.mNext;
        }
        {
            otherIt.mNext->mPrev = otherIt.mPrev;
            otherIt.mPrev->mNext = otherIt.mNext;
            otherList.mSize -= 1;
            Serial.print(F("(splice) otherList.mSize: "));
            Serial.println(otherList.mSize);
        }
        // insert into current list
        {
            if (*mBegin == position) {
                Serial.println(F("(splice) *mBegin == position"));
                mBegin = otherIt.mCurrent;
            }
            position.mPrev->mNext = otherIt.mCurrent;
            otherIt.mCurrent->mPrev = position.mPrev;
            otherIt.mCurrent->mNext = position.mCurrent;
            position.mCurrent->mPrev = otherIt.mCurrent;
            ++mSize;
        }
    }

    iterator erase(iterator position) {
        if (position == *mEnd) {
            return *mEnd;
        } else {
            auto prev = position.mPrev;
            auto next = position.mNext;
            prev->mNext = next;
            next->mPrev = prev;
            if (*mBegin == position) {
                mBegin = next;
            }
            delete position.mCurrent;
            --mSize;
            return *next;
        }
    }
};

#endif
