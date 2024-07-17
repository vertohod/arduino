#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "DirReader.h"
#include "MenuDrawer.h"
#include "Types.h"

bool getPathFile(Adafruit_ILI9341 &screen, char* path, uint16_t& position);
void MenuInt0Handler();
void MenuInt1Handler();

class Menu {
private:
    MenuDrawer      mMenuDrawer;
    DirReader       mDirReader;

    char            *mPath;
    tFileNameList   mFileNameList;

    uint32_t        mTimeoutCounter;
    uint16_t        mUpVisiblePosition;
    uint8_t         mCurrentPosition;
    uint8_t         mFilesAmount;

public:
    bool            mEncoderInt0;
    bool            mEncoderInt1;

public:
    Menu(Adafruit_ILI9341 &screen, char* path, uint16_t position);
    void updateMenu();
    void stepUp();
    void stepDn();
    bool clickHandler(bool checkOnly = true);
    uint16_t getPosition();
    static uint16_t getLastSlash(const char* path, uint16_t length = 0);
    static uint16_t getPrevSlash(const char* path);
    static bool isStrEq(const char* str1, const char* str2);

    void clearTimeout();
    bool checkTimeout();

private:
    void getChosenItem(char* result);
    void menuDraw(bool quickDraw = false, bool superQuick = false);
};

#endif
