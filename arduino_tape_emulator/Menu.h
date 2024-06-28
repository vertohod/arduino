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

    tListString*    mUpBuffer;
    tListString*    mVisibleBuffer;
    tListString*    mDnBuffer;

    uint8_t         mCurrentPosition;
    uint16_t        mUpVisiblePosition;
    uint16_t        mDnPosition;

public:
    Menu(IDataProvider* dataProvider, IMenuDrawer* menuDrawer)
        : mDataProvider(dataProvider)
        , mMenuDrawer(menuDrawer)
        , mLength(0)
        , mUpBuffer(nullptr)
        , mVisibleBuffer(nullptr)
        , mDnBuffer(nullptr)
        , mCurrentPosition(0)
        , mUpVisiblePosition(0)
        , mDnPosition(0)
    {
        mLength = mMenuDrawer->maxItems() - 1;
        mDataProvider->setSizeDataSet(mLength);
        mVisibleBuffer = mDataProvider->next();
        if (mVisibleBuffer) {
            mDnPosition = mVisibleBuffer->size();
        }
        draw();
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

    string getChosenItem() {
        uint16_t counter = 0;
        for (auto it = mVisibleBuffer->begin(); it != mVisibleBuffer->end(); ++it) {
            if (mCurrentPosition == counter) {
                return *it;
            }
            ++counter;
        }
        return string();
    }

    void stepUp() {
        if (mCurrentPosition > mLength / 2) {
            --mCurrentPosition;
            draw(true);
        } else {
            if (!mUpBuffer) {
                mUpBuffer = mDataProvider->prev();
            }
            if (mUpBuffer && mUpBuffer->size() == 0) {
                delete mUpBuffer;
                mUpBuffer = mDataProvider->prev();
            }
            if (mUpBuffer && mUpBuffer->size() > 0) {
                auto it = mVisibleBuffer->end();
                --it;
                mVisibleBuffer->erase(it);
                auto moveIt = mUpBuffer->end();
                --moveIt;
                mVisibleBuffer->splice(mVisibleBuffer->begin(), *mUpBuffer, moveIt);
                draw(true, true);
            } else if (mCurrentPosition > 0) {
                --mCurrentPosition;
                draw(true);
            }
        }
    }

    void stepDn() {
        if (mCurrentPosition < mLength / 2) {
            ++mCurrentPosition;
            draw(true);
        } else {
            if (!mDnBuffer) {
                mDnBuffer = mDataProvider->next();
            }
            if (mDnBuffer && mDnBuffer->size() == 0) {
                delete mDnBuffer;
                mDnBuffer = mDataProvider->next();
            }
            if (mDnBuffer && mDnBuffer->size() > 0) {
                auto it = mVisibleBuffer->begin();
                mVisibleBuffer->erase(mVisibleBuffer->begin());
                mVisibleBuffer->splice(mVisibleBuffer->end(), *mDnBuffer, mDnBuffer->begin());
                draw(true, true);
            } else if (mCurrentPosition < (mLength - 1)) {
                ++mCurrentPosition;
                draw(true);
            }
        }
    }
private:
    void draw(bool quickDraw = false, bool superQuick = false) {
        uint16_t counter = 0;
        for (auto it = mVisibleBuffer->begin(); it != mVisibleBuffer->end(); ++it) {
            if (quickDraw) {
                mMenuDrawer->quickDrawItem(*it, counter, mCurrentPosition == counter, !superQuick);
            } else {
                mMenuDrawer->drawItem(*it, counter, mCurrentPosition == counter);
            }
            ++counter;
        }
    }
};

#endif
