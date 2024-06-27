#ifndef MENU_H
#define MENU_H

#include "list.h"

class Menu
{
private:
    IDataProvider* mDataProvider;

public:
    setDataProvider(IDataProvider* provider) {
    }




    void clean();
    char* getChosenItem();

private:
    void stepUp();
    void stepDown();
};

#endif
