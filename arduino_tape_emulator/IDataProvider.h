#ifndef IDATAPROVIDER_H
#define IDATAPROVIDER_H

#include "list.h"
#include "string.h"

typedef list<string> tListString;

class IDataProvider
{
public:
    virtual string getDirectory() = 0;
    virtual void setSizeDataset(uint8_t size) = 0;
    virtual tListString* getData(uint16_t position) = 0; 
    virtual ~IDataProvider(){}
};

#endif
