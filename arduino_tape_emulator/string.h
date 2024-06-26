#ifndef STRING_H
#define STRING_H

class string
{
private:
    char*  mData;
    size_t mLength;
    size_t mCapacity;

public:
    string() : mData(nullptr), mLength(0), mCapacity(0) {}

    string(const string& str) {
        resize(str.mLength);
        memcpy(str.mData, mData, str.mLength + 1);
        mLength = str.mLength;
    }

    string(const char* str) : mData(nullptr), mLength(0), mCapacity(0) {
        mLength = strlen(str);
        resize(mLength);
        memcpy(str, mData, mLength + 1);
    }

    ~string() {
        clear();
    }

    string& operator=(const string& str)
    {
        resize(str.mLength);
        memcpy(str.mData, mData, str.mLength + 1);
        mLength = str.mLength;
        return *this;
    }

    string& operator+(const char* str) {
        auto len = strlen(str);
        resize(mLength + len);
        memcpy(str, &mData[mLength], len + 1);
        mLength += len;
        return *this;
    }

    string& operator+(const string& str) {
        operator+(str.c_str());
        return *this;
    }

    const char* c_str() const {
        return mData;
    }

    size_t length() const {
        return mLength;
    }

    bool empty() const {
        return 0 == strlen(mData);
    }

    void clear() {
        if (mData) {
            delete[] mData;
            mData = nullptr;
        }
    }

    bool operator==(const string& str) const {
        if (length() != str.length()) {
            return false;
        }
        for (size_t i = 0; i < length(); ++i) {
            if (c_str()[i] != str.c_str()[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const string& str) const {
        return !(operator==(str));
    }

    bool truncate(size_t position) {
        if (position <= mLength) {
            mData[position] = 0;
            mLength = position;
            return true;
        }
        return false;
    }

private:
    static size_t strlen(const char* str) {
        if (nullptr == str) {
            return 0;
        }
        size_t i = 0;
        while (str[i] != 0) {
            ++i;
        }
        return i;
    }

    void memcpy(const char* const source, char* const dist, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            dist[i] = source[i];
        }
    }

    void resize(size_t size) {
        if ((size + 1) > mCapacity) {
            mCapacity = size + 1;
            char* temp = new char[mCapacity];
            if (nullptr != mData) {
                memcpy(mData, temp, mLength + 1);
                delete[] mData;
            }
            mData = temp;
        }
    }
};

#endif
