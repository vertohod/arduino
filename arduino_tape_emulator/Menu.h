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
    uint8_t         mCurrentPosition;
    uint16_t        mUpVisiblePosition;

    tListString*    mVisibleBuffer;

public:
    Menu(IDataProvider* dataProvider, IMenuDrawer* menuDrawer)
        : mDataProvider(dataProvider)
        , mMenuDrawer(menuDrawer)
        , mCurrentPosition(0)
        , mUpVisiblePosition(0)
    {
        mLength = mMenuDrawer->maxItems() - 1;
        mDataProvider->setSizeDataset(mLength);
        mVisibleBuffer = mDataProvider->getData(0);
        draw();
    }

    ~Menu() {
        deleteBuffer();
        if (nullptr != mDataProvider) {
            delete mDataProvider;
        }
        if (nullptr != mMenuDrawer) {
            delete mMenuDrawer;
        }
    }

    string getDirectory() {
        return mDataProvider->getDirectory();
    }

    string getChosenItem() {
        uint16_t counter = 0;
        if (nullptr == mVisibleBuffer) {
            return string();
        }
        for (auto it = mVisibleBuffer->begin(); it != mVisibleBuffer->end(); ++it) {
            if (mCurrentPosition == counter) {
                return *it;
            }
            ++counter;
        }
        return string();
    }

    void stepUp() {
        if (mCurrentPosition > 0) {
            --mCurrentPosition;
            draw(true);
        } else {
            if (mUpVisiblePosition > 0) {
                mUpVisiblePosition -= mLength;
                mCurrentPosition = mLength - 1;
                deleteBuffer();
                mVisibleBuffer = mDataProvider->getData(mUpVisiblePosition);
            }
            draw(true);
        }
    }

    void stepDn() {
        if (nullptr == mVisibleBuffer) {
            return;
        }
        if (mCurrentPosition < mLength - 1) {
            if (mCurrentPosition < mVisibleBuffer->size() - 1) {
                ++mCurrentPosition;
            }
            draw(true);
        } else {
            deleteBuffer();
            mVisibleBuffer = mDataProvider->getData(mUpVisiblePosition + mLength);
            if (nullptr == mVisibleBuffer) {
                return;
            }
            if (mVisibleBuffer->empty()) {
                deleteBuffer();
                mVisibleBuffer = mDataProvider->getData(mUpVisiblePosition);
            } else {
                mUpVisiblePosition += mLength;
                mCurrentPosition = 0;
            }
            draw(true);
        }
    }

private:
    void draw(bool quickDraw = false, bool superQuick = false) {
        uint16_t counter = 0;
        if (nullptr == mVisibleBuffer) {
            return;
        }
        for (auto it = mVisibleBuffer->begin(); it != mVisibleBuffer->end(); ++it) {
            if (quickDraw) {
                mMenuDrawer->quickDrawItem(*it, counter, mCurrentPosition == counter, !superQuick);
            } else {
                mMenuDrawer->drawItem(*it, counter, mCurrentPosition == counter);
            }
            ++counter;
        }
        if (mVisibleBuffer->size() < mLength) {
            for (auto i = mVisibleBuffer->size(); i < mLength; ++i) {
                mMenuDrawer->drawItem(string(), i, false);
            }
        }
    }

    void deleteBuffer() {
        if (nullptr != mVisibleBuffer) {
            delete mVisibleBuffer;
            mVisibleBuffer = nullptr;
        }
    }
};

#endif
