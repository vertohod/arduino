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
    bool            mLastDirectionUp;

    tListString*    mVisibleBuffer;
    tListString*    mAddBuffer;

    uint8_t         mCurrentPosition;
    uint16_t        mUpVisiblePosition;
    uint16_t        mDnPosition;

public:
    Menu(IDataProvider* dataProvider, IMenuDrawer* menuDrawer)
        : mDataProvider(dataProvider)
        , mMenuDrawer(menuDrawer)
        , mLength(0)
        , mLastDirectionUp(false)
        , mVisibleBuffer(nullptr)
        , mAddBuffer(nullptr)
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
        if (mVisibleBuffer) {
            delete mVisibleBuffer;
        }
        if (mAddBuffer) {
            delete mAddBuffer;
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
            if (!mLastDirectionUp) {
                deleteAddBuffer();
            }
            if (loadBufferAndSplice(true)) {
                draw(true, true);
            } else {
                if (mCurrentPosition > 0) {
                    --mCurrentPosition;
                }
                draw(true);
            }
        }
        mLastDirectionUp = true;
    }

    void stepDn() {
        if (mCurrentPosition < mLength / 2) {
            ++mCurrentPosition;
            draw(true);
        } else {
            if (mLastDirectionUp) {
                deleteAddBuffer();
            }
            if (loadBufferAndSplice(false)) {
                draw(true, true);
            } else {
                if (mCurrentPosition < mLength) {
                    ++mCurrentPosition;
                }
                draw(true);
            }
        }
        mLastDirectionUp = false;
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

    bool loadBufferAndSplice(bool up) {
        if (nullptr == mAddBuffer) {
            mAddBuffer = mDataProvider->next();
        }
        if (nullptr != mAddBuffer && mAddBuffer->size() == 0) {
            deleteAddBuffer();
            if (up) {
                mAddBuffer = mDataProvider->prev();
            } else {
                mAddBuffer = mDataProvider->next();
            }
        }
        if (nullptr != mAddBuffer && mAddBuffer->size() > 0) {
            auto positionIt = mVisibleBuffer->begin();
            auto eraseIt = mVisibleBuffer->end();
            auto moveIt = mAddBuffer->end();
            if (up) {
                eraseIt = mVisibleBuffer->findPrev(eraseIt);
                moveIt = mAddBuffer->findPrev(moveIt);
            } else {
                positionIt = mVisibleBuffer->end();
                eraseIt = mVisibleBuffer->begin();
                moveIt = mAddBuffer->begin();
                Serial.print(F("moveIt: "));
                Serial.println(moveIt->c_str());
            }
            mVisibleBuffer->erase(eraseIt);
            mVisibleBuffer->splice(positionIt, *mAddBuffer, moveIt);
            return true;
        }
        return false;
    }

    void deleteAddBuffer() {
        if (nullptr != mAddBuffer) {
            delete mAddBuffer;
            mAddBuffer = nullptr;
        }
    }
};

#endif
