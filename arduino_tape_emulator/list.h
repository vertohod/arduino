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

    const iterator& insert(const iterator position, const T& value) {
        auto newItem = new iterator();
        newItem->mValue = value;
        if (*mBegin == position) {
            newItem->mNext = mBegin;
            mBegin = newItem;
            return *mBegin;
        } else {
            auto prev = findPrevPtr(position);
            newItem->mNext = prev->mNext;
            prev->mNext = newItem;
            return *newItem;
        }
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

    void splice(iterator position, list& otherList, iterator otherIt) {
        insert(position, *otherIt);
        otherList.erase(otherIt);
    }

    const iterator& erase(const iterator position) {
        Serial.println(F("(erase)"));
        if (position == *mEnd) {
            Serial.println(F("(erase) position == *mEnd"));
            return *mEnd;
        } else {
            if (position == *mBegin) {
                Serial.println(F("(erase) position == *mBegin"));
                delete mBegin;
                mBegin = position.mNext;
            } else {
                auto prev = findPrevPtr(position); 
                if (*prev != *mEnd) {
                    Serial.println(F("(erase) *prev != *mEnd"));
                    delete prev->mNext;
                    prev->mNext = position.mNext;
                }
            }
            --mSize;
            return *(position.mNext);
        }
    }

    const iterator& findPrev(const iterator& position) {
        return *findPrevPtr(position);
    }

private:
    iterator* findPrevPtr(const iterator& position) {
        auto itPtr = mBegin;
        while (*itPtr != *mEnd) {
            auto nextIt = itPtr->mNext;
            if (*nextIt == position) {
                return nextIt;
            }
            itPtr = nextIt;
        }
        return mEnd;
    }

    void insertPtr(const iterator& position, iterator* ptr) {
        if (*mBegin == position) {
            ptr->mNext = mBegin->mNext;
            mBegin = ptr;
        } else {
            auto prev = findPrevPtr(position);
            ptr->mNext = prev->mNext;
            prev->mNext = ptr;
        }
        ++mSize;
    }
};

#endif
