#ifndef IMENUDRAWER_H
#define IMENUDRAWER_H

class IMenuDrawer
{
public:
    virtual uint8_t maxItems() = 0;
    virtual void drawItem(const string& text, uint16_t position, bool active) = 0;
    virtual void quickDrawItem(const string& text, uint16_t position, bool active, bool fillAll) = 0;
    virtual ~IMenuDrawer(){}
};

#endif
