#ifndef COBJECT_EXT_H
#define COBJECT_EXT_H

#include "vector.h"
#include "cobject.h"
#include "functions.h"

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
        static vector<char> response_buffer;

        if (!m_request_done) {
            response_buffer.erase();
            m_request_done = true;

            REQUEST()(m_command);
        } else {
            auto size = RESPONSE()(response_buffer);

            if (size > 0) {
                auto string_length = get_string_length(m_answer);
                m_response_gotten = find(response_buffer, response_buffer.size(), m_answer, string_length);
            }
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
