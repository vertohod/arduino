#ifndef STRING_H
#define STRING_H

class string
{
private:
    char* mData;
    uint16_t mLength;
    uint16_t mCapacity;

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
    const char* c_str() {
        return mData;
    }
    uint16_t length() {
        return mLength;
    }
    bool empty() {
        return 0 == strlen(mData);
    }
    void clear() {
        if (mData) {
            delete[] mData;
            mData = nullptr;
        }
    }

private:
    uint16_t strlen(char* str) {
        if (nullptr == str) {
            return 0;
        }
        uint16_t i = 0;
        while (str[i] != 0) {
            ++i;
        }
        return i;
    }
    void memcpy(char* source, char* dist, uint16_t len) {
        for (uint16_t i = 0; i < len; ++i) {
            dist[i] = source[i];
        }
    }
    void resize(uint16_t size) {
        if ((size + 1) > mCapacity) {
            mCapacity = size + 1;
            char* temp = new char[mCapacity];
            if (mData) {
                memcpy(mData, temp, mLength + 1);
                delete[] mData;
            }
            mData = temp;
        }
    }
};

#endif
