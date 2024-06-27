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

    public:
        iterator() : mPrev(nullptr), mNext(nullptr) {}
        iterator(const iterator& obj) {
            mValue = obj.mValue;
            mPrev = obj.mPrev;
            mNext = obj.mNext;
        }
        iterator& operator=(const iterator& obj) {
            mValue = obj.mValue;
            mPrev = obj.mPrev;
            mNext = obj.mNext;
            return *this;
        }
        iterator& operator++() {
            mValue = mNext->mValue;
            mPrev = mNext->mPrev;
            mNext = mNext->mNext;
            return *this;
        }
        iterator operator++(int) {
            auto ret = *this;
            mValue = mNext->mValue;
            mPrev = mNext->mPrev;
            mNext = mNext->mNext;
            return ret;
        }
        iterator& operator--() {
            mValue = mPrev->mValue;
            mPrev = mPrev->mPrev;
            mNext = mPrev->mNext;
            return *this;
        }
        iterator operator--(int) {
            auto ret = *this;
            mValue = mPrev->mValue;
            mPrev = mPrev->mPrev;
            mNext = mPrev->mNext;
            return ret;
        }
        bool operator==(const iterator& obj) {
            return mPrev == obj.mPrev && mNext == obj.mNext; 
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
        mBegin = new iterator();
        mEnd = mBegin;
    }
    ~list() {
        delete mBegin;
    }
    iterator& begin() {
        return *mBegin;
    }
    iterator& end() {
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
};

#endif
