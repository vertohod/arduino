#ifndef LIST_H
#define LIST_H

template <class T>
class list {
public:
    class iterator {
    friend list;
    private:
        T mValue;
        iterator* mNext;

    public:
        iterator() : mNext(nullptr) {}
        iterator(const iterator& obj) {
            mValue = obj.mValue;
            mNext = obj.mNext;
        }
        iterator& operator=(const iterator& obj) {
            mValue = obj.mValue;
            mNext = obj.mNext;
            return *this;
        }
        iterator& operator++() {
            mValue = mNext->mValue;
            mNext = mNext->mNext;
            return *this;
        }
        iterator operator++(int) {
            auto ret = *this;
            mValue = mNext->mValue;
            mNext = mNext->mNext;
            return ret;
        }
        bool operator==(const iterator& obj) const {
            return mNext == obj.mNext;
        }
        bool operator!=(const iterator& obj) const {
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
        clear();
        delete mEnd;
    }

    iterator begin() {
        return *mBegin;
    }

    const iterator& end() {
        return *mEnd;
    }

    void push(const T& value) {
        auto last = mEnd;
        mEnd = new iterator();
        last->mValue = value;
        last->mNext = mEnd;
        ++mSize;
    }

    void clear() {
        auto itPtr = mBegin;
        while (*itPtr != *mEnd) {
            auto nextIt = itPtr->mNext;
            delete itPtr;
            itPtr = nextIt;
        }
        mBegin = mEnd;
        mSize = 0;
    }

    size_t size() {
        return mSize;
    }

    bool empty() {
        return 0 == size();
    }
};

#endif
