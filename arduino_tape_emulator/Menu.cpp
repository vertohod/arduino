#include "Menu.h"
#include "SwitchExceptions.h"

#define KILOBYTE "KB"
#define MENU_TIMEOUT 1000000

Menu* gMenuPtr = nullptr;

bool getPathFile(Adafruit_ILI9341 &screen, char* path, uint16_t& position) {
    Menu localMenu(screen, path, position);
    gMenuPtr = &localMenu;

    enableExceptions();

    bool result = false;
    bool buttonIsClicked = false;
    while (true) {
        if (!(PIND & B00100000)) {
            if (!buttonIsClicked) {
                buttonIsClicked = true;
                if (gMenuPtr->clickHandler(false)) {
                    position = gMenuPtr->getPosition();
                    result = true;
                    break;
                } else {
                    gMenuPtr->updateMenu();
                }
            }
        } else {
            buttonIsClicked = false;
        }
        if (gMenuPtr->checkTimeout()) {
            if (gMenuPtr->clickHandler(true)) {
                position = gMenuPtr->getPosition();
                break;
            } else {
                gMenuPtr->clearTimeout();
            }
        }
    }
    disableExceptions();

    return result;
}

Menu::Menu(Adafruit_ILI9341 &screen, char* path, uint16_t position)
    : mMenuDrawer(screen)
    , mPath(path)
{
    uint16_t length = strlen(path);
    if (path[length - 1] != '/') {
        uint16_t lastSlash = getLastSlash(path);
        path[lastSlash + 1] = 0;
    }

    mTimeoutCounter = MENU_TIMEOUT;
    mUpVisiblePosition = (position / MENU_LENGTH) * MENU_LENGTH;
    mCurrentPosition = position % MENU_LENGTH;
    mEncoderInt0 = false;
    mEncoderInt1 = false;

    updateMenu();
}

void Menu::updateMenu()
{
    mFilesAmount = mDirReader.getFileNameList(mPath, mUpVisiblePosition, mFileNameList);
    menuDraw();
}

void Menu::getChosenItem(char* result) {
    for (uint8_t i = 0; i < MENU_LENGTH; ++i) {
        if (mCurrentPosition == i) {
            char* fileName = mFileNameList[i].fileName;
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
            mUpVisiblePosition -= MENU_LENGTH;
            mCurrentPosition = MENU_LENGTH - 1;
            mFilesAmount = mDirReader.getFileNameList(mPath, mUpVisiblePosition, mFileNameList);
            menuDraw(true);
        }
    }
}

void Menu::stepDn() {
    if (mCurrentPosition < MENU_LENGTH - 1) {
        if (mCurrentPosition < mFilesAmount - 1) {
            ++mCurrentPosition;
            menuDraw(true);
        }
    } else {
        mFilesAmount = mDirReader.getFileNameList(mPath, mUpVisiblePosition + MENU_LENGTH, mFileNameList);
        if (mFilesAmount == 0) {
            mFilesAmount = mDirReader.getFileNameList(mPath, mUpVisiblePosition, mFileNameList);
        } else {
            mUpVisiblePosition += MENU_LENGTH;
            mCurrentPosition = 0;
            menuDraw(true);
        }
    }
}

bool Menu::clickHandler(bool checkOnly) {
    char fileName[FILENAME_LENGTH];
    getChosenItem(&fileName[0]);
    uint16_t fileNameLength = strlen(fileName);
    if (0 == strcmp(&fileName[0], TWO_POINTS)) {
        if (!checkOnly) {
            uint16_t prevSlashIndex = getPrevSlash(mPath);
            // truncate
            mPath[prevSlashIndex + 1] = 0;
            mCurrentPosition = 0;
        }
    } else if (fileNameLength > 0 && fileName[fileNameLength - 1] == '/') {
        if (!checkOnly) {
            uint16_t pathLength = strlen(mPath);
            memcpy(static_cast<void*>(&mPath[pathLength]), static_cast<void*>(&fileName[0]), fileNameLength + 1);
            mCurrentPosition = 0;
        }
    } else {
        uint16_t pathLength = strlen(mPath);
        memcpy(static_cast<void*>(&mPath[0]), static_cast<void*>(&mPath[0]), pathLength);
        memcpy(static_cast<void*>(&mPath[pathLength]), static_cast<void*>(&fileName[0]), fileNameLength + 1);
        return true;
    }
    return false;
}

uint16_t Menu::getPosition() {
    return mUpVisiblePosition + mCurrentPosition;
}

uint16_t Menu::getLastSlash(const char* path, uint16_t length) {
    uint16_t i = length > 0 ? length : strlen(path);
    while (0 < --i) {
        if (path[i] == '/') {
            return i;
        }
    }
    return 0;
}

uint16_t Menu::getPrevSlash(const char* path) {
    uint16_t lastSlashIndex = getLastSlash(path); 
    return getLastSlash(path, lastSlashIndex - 1);
}

void Menu::clearTimeout() {
    mTimeoutCounter = MENU_TIMEOUT;
}

bool Menu::checkTimeout() {
    --mTimeoutCounter;
    return 0 == mTimeoutCounter;
}

void Menu::menuDraw(bool quickDraw) {
    char textNumber[9];

    if (!quickDraw) {
        mMenuDrawer.setHeader(mPath);
    }
    for (uint8_t i = 0; i < mFilesAmount; ++i) {
        char textOutput[OUTPUT_ITEM_LENGTH];
        for (auto i = 0; i < OUTPUT_ITEM_LENGTH; ++i) {
            textOutput[i] = ' ';
        }
        textOutput[OUTPUT_ITEM_LENGTH - 1] = 0;

        // format text
        const char* fileNameSrc = mFileNameList[i].fileName;
        memcpy(static_cast<void*>(&textOutput[0]), static_cast<const void*>(fileNameSrc), strlen(fileNameSrc));
        if (mFileNameList[i].fileSize > 0) {
            uint16_t sizePosition = OUTPUT_ITEM_LENGTH - strlen(KILOBYTE) - 1;
            memcpy(static_cast<void*>(&textOutput[sizePosition]), static_cast<const void*>(KILOBYTE), strlen(KILOBYTE));
            uint32_t sizeKb = mFileNameList[i].fileSize / 1024;
            sprintf(textNumber, "%lu", sizeKb);
            --sizePosition;
            for(;;) {
                sizeKb = sizeKb / 10;
                if (sizeKb > 0) {
                    --sizePosition;
                } else {
                    break;
                }
            }
            memcpy(static_cast<void*>(&textOutput[sizePosition]), static_cast<const void*>(&textNumber[0]), strlen(textNumber));
        }
        auto active = mCurrentPosition == i;
        if (quickDraw) {
            mMenuDrawer.quickDrawItem(textOutput, i, active);
        } else {
            mMenuDrawer.drawItem(textOutput, i, active);
        }
    }
    for (auto i = mFilesAmount; i < MENU_LENGTH; ++i) {
        mMenuDrawer.drawItem("", i, false);
    }
}

void MenuInt0Handler() {
    gMenuPtr->clearTimeout();
    if (gMenuPtr->mEncoderInt1) {
        gMenuPtr->mEncoderInt1 = false;
        gMenuPtr->stepUp();
    } else {
        gMenuPtr->mEncoderInt0 = true;
    }
}

void MenuInt1Handler() {
    gMenuPtr->clearTimeout();
    if (gMenuPtr->mEncoderInt0) {
        gMenuPtr->mEncoderInt0 = false;
        gMenuPtr->stepDn();
    } else {
        gMenuPtr->mEncoderInt1 = true;
    }
}
