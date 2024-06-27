#ifndef MENU_H
#define MENU_H

#include "Types.h"
#include "IDataProvider.h"
#include "string.h"

class Menu
{
private:
    IDataProvider*  mDataProvider;
    size_t          mLength;

public:
    Menu() : mDataProvider(nullptr), mLength(0) {
    }
    Menu(IDataProvider* dataProvider) : mDataProvider(dataProvider), mLength(0) {
    }
    Menu(IDataProvider* dataProvider, size_t length) : mDataProvider(dataProvider), mLength(length) {
        if (mDataProvider) {
            mDataProvider->setSizeDataSet(mLength);
        }
    }
    setDataProvider(IDataProvider* dataProvider) {
        mDataProvider = dataProvider;
        if (mDataProvider) {
            mDataProvider->setSizeDataSet(mLength);
        }
    }
    setLength(size_t length) {
        mLength = length;
        if (mDataProvider) {
            mDataProvider->setSizeDataSet(mLength);
        }
    }

    string getChosenItem();

private:
    void stepUp();
    void stepDown();
};

#endif
