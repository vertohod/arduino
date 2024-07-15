#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "DirReader.h"
#include "MenuDrawer.h"

char* getPathFile(Adafruit_ILI9341 *screenPtr, const char* path);

typedef char tPath[(FILENAME_LENGTH + 1) * 2];

class Menu {
private:
    MenuDrawer      mMenuDrawer;
    DirReader       mDirReader;

    tPath           mPath;
    tFileNameList   mFileNameList;

    uint16_t        mUpVisiblePosition;
    uint8_t         mCurrentPosition;
    uint8_t         mFilesAmount;

public:
    bool            mEncoderInt0;
    bool            mEncoderInt1;

public:
    Menu(Adafruit_ILI9341 *screenPtr, const char* path);
    void updateMenu();
    void getChosenItem(char result[FILENAME_LENGTH]);
    void stepUp();
    void stepDn();
    bool clickHandler();
    static uint16_t getPrevSlash(const char* path);
    static bool isStrEq(const char* str1, const char* str2);

private:
    void menuDraw(bool quickDraw = false, bool superQuick = false);
};

#endif
