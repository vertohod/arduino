#ifndef COBJECT_H
#define COBJECT_H

#include "vector.h"

class cobject
{
private:
    vector<cobject*>        m_queue_of_commands;
    mutable unsigned int    m_counter;

public:
    cobject();

    void add_object(cobject*);
    virtual void execute() const;
    virtual bool is_completed() const;
    virtual bool is_successful() const;

    void clean();
};

#endif
