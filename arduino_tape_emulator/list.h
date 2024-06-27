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
        iterator() : mPrev(nullptr), mNext(nullptr) {
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
            mNext = mNext->mNext;
            mCurrent = mNext->mCurrent;
            return *this;
        }
        iterator operator++(int) {
            auto ret = *this;
            mValue = mNext->mValue;
            mPrev = mNext->mPrev;
            mNext = mNext->mNext;
            mCurrent = mNext->mCurrent;
            return ret;
        }
        iterator& operator--() {
            mValue = mPrev->mValue;
            mPrev = mPrev->mPrev;
            mNext = mPrev->mNext;
            mCurrent = mPrev->mCurrent;
            return *this;
        }
        iterator operator--(int) {
            auto ret = *this;
            mValue = mPrev->mValue;
            mPrev = mPrev->mPrev;
            mNext = mPrev->mNext;
            mCurrent = mPrev->mCurrent;
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
    size_t      mSize;

public:
    list() : mSize(0) {
        mEnd = new iterator();
        mBegin = mEnd;
    }
    ~list() {
        delete mBegin;
    }
    iterator begin() {
        return *mBegin;
    }
    iterator end() {
        return *mEnd;
    }
    void push(const T& value) {
        auto last = mEnd;
        last->mValue = value;

        mEnd = new iterator();
        last->mNext = mEnd;
        mEnd->mPrev = last;
        mEnd->mNext = mBegin;

        ++mSize;
    }
    void clear() {
        auto last = mEnd;
        mEnd = mBegin;
        while (*mBegin != *last) {
            auto prev = last->mPrev;
            delete last;
            last = prev;
        }
        last->mPrev = nullptr;
        last->mNext = nullptr;

        mSize = 0;
    }
    size_t size() {
        return mSize;
    }
    void splice(iterator position, list& otherList, iterator otherIt) {
        // check if otherIt valid iterator
        if (otherList.end() == otherIt) {
            return;
        }
        auto it = otherList.begin();
        while (it != otherList.end()) {
            if (it == otherIt) {
                break;
            }
        }
        if (it != otherList.end()) {
            return;
        }
        // cut out an item
        if (otherList.begin() == otherIt) {
            otherList.mBegin = otherIt.mNext;
        }
        {
            auto prev = otherIt.mPrev;
            auto next = otherIt.mNext;
            prev->mNext = next;
            next->mPrev = prev;
            otherList.mSize -= 1;
        }
        // insert into current list
        {
            if (*mBegin == position) {
                mBegin = otherIt.mCurrent;
            }
            auto prev = position.mPrev;
            prev->mNext = otherIt.mCurrent;
            otherIt.mPrev = prev;
            otherIt.mNext = &position;
            position.mPrev = otherIt.mCurrent;
            ++mSize;
        }
    }
    iterator erase(iterator position) {
        if (position != *mEnd) {
            if (*mBegin == position) {
                mBegin = position.mNext;
            }
            auto prev = position.mPrev;
            auto next = position.mNext;
            prev->mNext = next;
            next->mPrev = prev;
            delete position.mCurrent;
            return *next;
        } else {
            return *mEnd;
        }
    }
};

#endif
