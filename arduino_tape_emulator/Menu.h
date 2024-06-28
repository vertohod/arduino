#ifndef MENU_H
#define MENU_H

#include "Types.h"
#include "IDataProvider.h"
#include "IMenuDrawer.h"
#include "string.h"

class Menu
{
private:
    IDataProvider*  mDataProvider;
    IMenuDrawer*    mMenuDrawer;
    uint8_t         mLength;
    uint8_t         mHalfLength;

    tListString*    mUpBuffer;
    tListString*    mVisibleBuffer;
    tListString*    mDnBuffer;

    uint8_t         mCurrentPosition;
    uint16_t        mUpVisiblePosition;
//  uint16_t        mDnVisiblePosition; // = mUpVisiblePosition + mLength;
//  uint16_t        mUpPosition; // always 0
    uint16_t        mDnPosition;

public:
    Menu() : mDataProvider(nullptr)
        , mMenuDrawer(nullptr)
        , mLength(0)
        , mHalfLength(0)
        , mUpBuffer(nullptr)
        , mVisibleBuffer(nullptr)
        , mDnBuffer(nullptr)
        , mDnPosition(0)
        , mUpVisiblePosition(0)
        , mCurrentPosition(0)
    {}
    /*
    Menu(IDataProvider* dataProvider) : mDataProvider(dataProvider)
        , mMenuDrawer(nullptr)
        , mLength(0)
        , mHalfLength(0)
        , mUpBuffer(nullptr)
        , mVisibleBuffer(nullptr)
        , mDnBuffer(nullptr)
        , mDnPosition(0)
        , mUpVisiblePosition(0)
        , mCurrentPosition(0)
    {}
    Menu(IDataProvider* dataProvider, size_t length) : mDataProvider(dataProvider)
        , mMenuDrawer(nullptr)
        , mLength(length)
        , mHalfLength(length / 2)
        , mUpBuffer(nullptr)
        , mVisibleBuffer(nullptr)
        , mDnBuffer(nullptr)
        , mDnPosition(0)
        , mUpVisiblePosition(0)
        , mCurrentPosition(0)
    {
        if (mDataProvider) {
            mDataProvider->setSizeDataSet(mLength);
            mVisibleBuffer = mDataProvider->next();
            if (mVisibleBuffer) {
                mDnPosition = mVisibleBuffer->size();
            }
            draw();
        }
    }
    */
    ~Menu() {
        if (mUpBuffer) {
            delete mUpBuffer;
        }
        if (mVisibleBuffer) {
            delete mVisibleBuffer;
        }
        if (mDnBuffer) {
            delete mDnBuffer;
        }
    }
    setDataProvider(IDataProvider* dataProvider) {
        Serial.println(F("(Menu) call setDataProvider"));
        mDataProvider = dataProvider;
        if (mDataProvider) {
            mDataProvider->setSizeDataSet(mLength);
            mVisibleBuffer = mDataProvider->next();
            if (mVisibleBuffer) {
                mDnPosition = mVisibleBuffer->size();
            }
            draw();
        }
    }
    setMenuDrawer(IMenuDrawer* menuDrawer) {
        Serial.println(F("(Menu) call setMenuDrawer"));
        mMenuDrawer = menuDrawer;
        draw();
    }
    setLength(size_t length) {
        Serial.println(F("(Menu) call setLength"));
        mLength = length;
        mHalfLength = length / 2;
        if (mDataProvider) {
            mDataProvider->setSizeDataSet(mLength);
            mVisibleBuffer = mDataProvider->next();
            if (mVisibleBuffer) {
                mDnPosition = mVisibleBuffer->size();
            }
            draw();
        }
    }

    string getChosenItem() {
    }
    void stepUp() {
        draw();
    }
    void stepDown() {
        draw();
    }
    void draw() {
        Serial.println(F("(Menu) call draw"));
        if (mMenuDrawer == nullptr) {
            Serial.println(F("(Menu) mMenuDrawer is nullptr"));
            return;
        }
        if (mVisibleBuffer == nullptr) {
            Serial.println(F("(Menu) mVisibleBuffer is nullptr"));
            return;
        }
        if (mLength == 0) {
            Serial.println(F("(Menu) mLength: 0"));
            return;
        }
        size_t counter = 0;
        bool active = true;
        for (auto it = mVisibleBuffer->begin(); it != mVisibleBuffer->end(); ++it) {
            Serial.println(it->c_str());
            mMenuDrawer->quickDrawItem(*it, counter, active);
            ++counter;
            if (active) {
                active = false;
            }
        }
    }
};

#endif
