#ifndef IDATAPROVIDER_H
#define IDATAPROVIDER_H

#include "list.h"
#include "string.h"

typedef list<string> tListString;

class IDataProvider
{
public:
    virtual void setSizeDataSet(uint8_t size);
    virtual tListString* next();
    virtual tListString* prev();
};

#endif
