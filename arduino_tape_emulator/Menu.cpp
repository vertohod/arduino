#include "Menu.h"

#define TWO_POINTS ".."

Menu* gMenuPtr = nullptr;
tPath gPathFile;

char* getPathFile(const char* path) {
    Menu localMenu(path);
    gMenuPtr = &localMenu;

    // Enable INT0, INT1
    EICRA = 1 << ISC11 | 1 << ISC10 | 1 << ISC01 | 1 << ISC00;
    EIMSK = 1 << INT1 | 1 << INT0;

    bool buttonIsClicked = false;
    while (true) {
        if (!(PIND & B00100000)) {
            if (!buttonIsClicked) {
                buttonIsClicked = true;
                if (gMenuPtr->clickHandler()) {
                    break;
                } else {
                    gMenuPtr->updateMenu();
                }
            }
        } else {
            buttonIsClicked = false;
        }
    }

    // Disable INT0, INT1
    EICRA = 0; 
    EIMSK = 0; 

    return &gPathFile[0];
}

Menu::Menu(const char* path)
{
    memcpy(static_cast<void*>(&mPath[0]), static_cast<const void*>(path), strlen(path) + 1);

    mMenuDrawer.setTextSize(2);

    mLength = MENU_LENGTH;
    mCurrentPosition = 0;
    mUpVisiblePosition = 0;
    mFilesAmount = 0;
    mEncoderInt0 = false;
    mEncoderInt1 = false;

    updateMenu();
}

void Menu::updateMenu()
{
    mFilesAmount = mDirReader.getFileNameList(mPath, mUpVisiblePosition, mFileNameList);
    menuDraw();
}

void Menu::getChosenItem(char result[FILENAME_LENGTH]) {
    for (uint8_t i = 0; i < mLength; ++i) {
        if (mCurrentPosition == i) {
            char* fileName = mFileNameList[i];
            memcpy(static_cast<void*>(result), static_cast<const void*>(&fileName[0]), strlen(fileName) + 1);
        }
    }
}

void Menu::stepUp() {
    if (mCurrentPosition > 0) {
        --mCurrentPosition;
        menuDraw(true);
    } else {
        if (mUpVisiblePosition > 0) {
            mUpVisiblePosition -= mLength;
            mCurrentPosition = mLength - 1;

            mFilesAmount = mDirReader.getFileNameList(mPath, mUpVisiblePosition, mFileNameList);
        }
        menuDraw(true);
    }
}

void Menu::stepDn() {
    if (mCurrentPosition < mLength - 1) {
        if (mCurrentPosition < mFilesAmount - 1) {
            ++mCurrentPosition;
        }
        menuDraw(true);
    } else {
        mFilesAmount = mDirReader.getFileNameList(mPath, mUpVisiblePosition + mLength, mFileNameList);
        if (mFilesAmount == 0) {
            mFilesAmount = mDirReader.getFileNameList(mPath, mUpVisiblePosition, mFileNameList);
        } else {
            mUpVisiblePosition += mLength;
            mCurrentPosition = 0;
        }
        menuDraw(true);
    }
}

bool Menu::clickHandler() {
    char fileName[FILENAME_LENGTH];
    uint16_t prevSlashIndex = 0;
    uint16_t pathLength = strlen(mPath);
    uint16_t fileNameLength = 0; 
    getChosenItem(fileName);
    fileNameLength = strlen(fileName);
    Serial.print(F("(clickHandler) fileName: "));
    Serial.println(fileName);
    if (isStrEq(fileName, TWO_POINTS)) {
        prevSlashIndex = getPrevSlash(mPath);
        // truncate
        mPath[prevSlashIndex + 1] = 0;
        mCurrentPosition = 0;
    } else if (fileNameLength > 0 && fileName[fileNameLength - 1] == '/') {
        memcpy(static_cast<void*>(&mPath[pathLength]), static_cast<void*>(&fileName[0]), fileNameLength + 1);
        mCurrentPosition = 0;
    } else {
        memcpy(static_cast<void*>(&gPathFile[0]), static_cast<void*>(&mPath[0]), pathLength);
        memcpy(static_cast<void*>(&gPathFile[pathLength]), static_cast<void*>(&fileName[0]), fileNameLength + 1);
        return true;
    }
    return false;
}

uint16_t Menu::getPrevSlash(const char* path)
{
    uint16_t lastSlashIndex = 0;
    uint16_t prevSlashIndex = 0;
    uint16_t strLen = strlen(path);
    for (uint16_t i = 0; i < strLen; ++i) {
        if (path[i] == '/') {
            if (0 != lastSlashIndex) {
                prevSlashIndex = lastSlashIndex;
            }
            lastSlashIndex = i;
        }
    }
    return prevSlashIndex;
}

bool Menu::isStrEq(const char* str1, const char* str2)
{
    uint16_t len1;
    uint16_t len2;
    len1 = strlen(str1);
    len2 = strlen(str2);
    if (len1 != len2) {
        return false;
    }
    if (len1 == 0) {
        return true;
    }
    for (uint16_t i = 0; i < len1; ++i) {
        if (str1[i] != str2[i]) {
            return false;
        }
    } 
    return true;
}

void Menu::menuDraw(bool quickDraw, bool superQuick) {
    if (!quickDraw) {
        mMenuDrawer.setHeader(mPath);
    }
    for (uint8_t i = 0; i < mFilesAmount; ++i) {
        const char* fileName = mFileNameList[i];
        if (quickDraw) {
            mMenuDrawer.quickDrawItem(fileName, i, mCurrentPosition == i, !superQuick);
        } else {
            mMenuDrawer.drawItem(fileName, i, mCurrentPosition == i);
        }
    }
    if (mFilesAmount < mLength) {
        for (auto i = mFilesAmount; i < mLength; ++i) {
            mMenuDrawer.drawItem("", i, false);
        }
    }
}

ISR(INT0_vect)
{
    if (gMenuPtr->mEncoderInt1) {
        gMenuPtr->mEncoderInt1 = false;
        gMenuPtr->stepUp();
    } else {
        gMenuPtr->mEncoderInt0 = true;
    }
}

ISR(INT1_vect)
{
    if (gMenuPtr->mEncoderInt0) {
        gMenuPtr->mEncoderInt0 = false;
        gMenuPtr->stepDn();
    } else {
        gMenuPtr->mEncoderInt1 = true;
    }
}
