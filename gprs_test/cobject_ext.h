#ifndef COBJECT_EXT_H
#define COBJECT_EXT_H

#include "cobject.h"

template <class REQUEST, class RESPONSE, class TIMEOUT>
class cobject_ext : public cobject
{
private:
    mutable bool    m_request_done;
    mutable bool    m_response_gotten;
    char*           m_command;
    char*           m_answer;
    unsigned int    m_timeout;

public:
    cobject_ext() :
        m_request_done(false),
        m_response_gotten(false),
        m_command(nullptr),
        m_answer(nullptr),
        m_timeout(0) {}

    cobject_ext(char* command, char* answer, unsigned int timeout) :
        m_request_done(false),
        m_response_gotten(false),
        m_command(command),
        m_answer(answer),
        m_timeout(timeout) {}

    init(char* command, char* answer, unsigned int timeout)
    {
        m_command = command;
        m_answer = answer;
        m_timeout = timeout;
    }

    virtual void execute() const override
    {
        if (!m_request_done) {
            REQUEST()(m_command);
            m_request_done = true;
        } else {
            m_response_gotten = RESPONSE()(m_answer);
        }
    }

    virtual bool is_completed() const override
    {
        return TIMEOUT()(m_timeout) || m_response_gotten;
    }

    virtual bool is_successful() const override
    {
        return m_response_gotten;
    }
};

#endif
