#ifndef IDATAPROVIDER_H
#define IDATAPROVIDER_H

#include "list.h"
#include "string.h"

typedef list<string> tListString;

class IDataProvider
{
public:
    virtual void setSizeDataSet(size_t size);
    virtual tListString* next();
    virtual tListString* prev();
    virtual ~IDataProvider(){}
};

#endif
