#ifndef MENU_H
#define MENU_H

#include "list.h"

class menu
{
private:
    list m_data;

public:
    void add_item(char file_name);
    void clean();
    char* get_chosen_item();

private:
    void step_up();
    void step_down();
};

#endif
