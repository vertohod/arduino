#ifndef COBJECT_H
#define COBJECT_H

class cobject
{
private:
    mutable unsigned int    m_counter;
    unsigned int            m_length;
    cobject**               m_queue_of_commands;

public:
    cobject();

    void add_object(cobject*);
    virtual void execute() const;
    virtual bool is_completed() const;
    virtual bool is_successful() const;

    void clean();
};

#endif
