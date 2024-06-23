#ifndef STRINT_H
#define STRING_H

class string
{
private:
    char* mData;
    size_t mLength;
    size_t mCapacity;

public:
    string() : mData(nullptr), mLength(0), mCapacity(0) {}
    /*
    string(string&& str) {
        auto temp = str.mData;
        str.mData = mData;
        mData = temp;
        mLength = str.mLength;
        mCapacity = str.mCapacity;
    }
    */
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
        if (mData) {
            delete[] mData;
        }
    }
    string& operator=(const string& str)
    {
        resize(str.mLength);
        memcpy(str.mData, mData, str.mLength + 1);
        mLength = str.mLength;
    }
    string& operator+(const char* str) {
        auto len = strlen(str);
        resize(mLength + len);
        memcpy(str, &mData[mLength], len + 1);
        mLength += len;
    }
    const char* c_str() {
        return mData;
    }
    size_t length() {
        return mLength;
    }

private:
    size_t strlen(char* str) {
        size_t i = 0;
        while (str[i] != 0) {
            ++i;
        }
        return i;
    }
    void memcpy(char* source, char* dist, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            dist[i] = source[i];
        }
    }
    void resize(size_t size) {
        if (size > mCapacity) {
            mCapacity = mCapacity == 0 ? size + 1 : size * 2;
            auto temp = new char[mCapacity];
            if (mData) {
                memcpy(mData, temp, mLength + 1);
                delete[] mData;
            }
            mData = temp;
        }
    }
};

#endif
