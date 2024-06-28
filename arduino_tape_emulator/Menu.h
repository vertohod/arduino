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
    size_t          mLength;
    size_t          mHalfLength;

    tListString*    mUpBuffer;
    tListString*    mVisibleBuffer;
    tListString*    mDnBuffer;

    size_t          mUpPosition;
    size_t          mDnPosition;
    size_t          mUpVisiblePosition;
    size_t          mDnVisiblePosition;
    size_t          mCurrentPosition;

public:
    Menu() : mDataProvider(nullptr)
        , mMenuDrawer(nullptr)
        , mLength(0)
        , mHalfLength(0)
        , mUpBuffer(nullptr)
        , mVisibleBuffer(nullptr)
        , mDnBuffer(nullptr)
        , mUpPosition(0)
        , mDnPosition(0)
        , mUpVisiblePosition(0)
        , mDnVisiblePosition(0)
        , mCurrentPosition(0)
    {}
    Menu(IDataProvider* dataProvider) : mDataProvider(dataProvider)
        , mMenuDrawer(nullptr)
        , mLength(0)
        , mHalfLength(0)
        , mUpBuffer(nullptr)
        , mVisibleBuffer(nullptr)
        , mDnBuffer(nullptr)
        , mUpPosition(0)
        , mDnPosition(0)
        , mUpVisiblePosition(0)
        , mDnVisiblePosition(0)
        , mCurrentPosition(0)
    {}
    Menu(IDataProvider* dataProvider, size_t length) : mDataProvider(dataProvider)
        , mMenuDrawer(nullptr)
        , mLength(length)
        , mHalfLength(length / 2)
        , mUpBuffer(nullptr)
        , mVisibleBuffer(nullptr)
        , mDnBuffer(nullptr)
        , mUpPosition(0)
        , mDnPosition(0)
        , mUpVisiblePosition(0)
        , mDnVisiblePosition(0)
        , mCurrentPosition(0)
    {
        if (mDataProvider) {
            mDataProvider->setSizeDataSet(mLength);
            mVisibleBuffer = mDataProvider->next();
            if (mVisibleBuffer) {
                mDnVisiblePosition = mVisibleBuffer->size();
                mDnPosition = mDnVisiblePosition;
            }
            draw();
        }
    }
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
        mDataProvider = dataProvider;
        if (mDataProvider) {
            mDataProvider->setSizeDataSet(mLength);
            mVisibleBuffer = mDataProvider->next();
            if (mVisibleBuffer) {
                mDnVisiblePosition = mVisibleBuffer->size();
                mDnPosition = mDnVisiblePosition;
            }
            draw();
        }
    }
    setMenuDrawer(IMenuDrawer* menuDrawer) {
        mMenuDrawer = menuDrawer;
        draw();
    }
    setLength(size_t length) {
        mLength = length;
        mHalfLength = length / 2;
        if (mDataProvider) {
            mDataProvider->setSizeDataSet(mLength);
            mVisibleBuffer = mDataProvider->next();
            if (mVisibleBuffer) {
                mDnVisiblePosition = mVisibleBuffer->size();
                mDnPosition = mDnVisiblePosition;
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
        if (mMenuDrawer == nullptr) {
            return;
        }
        if (mVisibleBuffer == nullptr) {
            return;
        }
        size_t counter = 0;
        bool active = true;
        for (auto it = mVisibleBuffer->begin(); it != mVisibleBuffer->end(); ++it) {
            mMenuDrawer->drawItem(*it, counter, active);
            ++counter;
            if (active) {
                active = false;
            }
        }
    }
};

#endif