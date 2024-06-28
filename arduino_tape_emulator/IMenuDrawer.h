#ifndef IMENUDRAWER_H
#define IMENUDRAWER_H

class IMenuDrawer
{
public:
    virtual void drawItem(const string& text, size_t position, bool active) = 0;
    virtual ~IMenuDrawer(){}
};

#endif
