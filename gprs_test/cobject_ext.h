#ifndef COBJECT_EXT_H
#define COBJECT_EXT_H

#include "vector.h"
#include "cobject.h"
#include "functions.h"

template <class REQUEST, class RESPONSE, class TIMEOUT, bool REPEAT = false, unsigned char AMOUNT = 2>
class cobject_ext : public cobject
{
private:
    mutable bool            m_request_done;
    mutable bool            m_response_gotten;
    char*                   m_command;
    char*                   m_answer;
    unsigned int            m_timeout;
    mutable unsigned int    m_amount;

public:
    cobject_ext() :
        m_request_done(false),
        m_response_gotten(false),
        m_command(nullptr),
        m_answer(nullptr),
        m_timeout(0),
        m_amount(0)
    {}

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
        typename RESPONSE::type_buffer response_buffer;

        auto size = RESPONSE()(response_buffer, m_answer); // read everything in any case

        if (m_request_done) {
            if (size > 0) {
                auto string_length = get_string_length(m_answer);
                m_response_gotten = find(response_buffer, response_buffer.size(), m_answer, string_length);
            }
        } else {
            response_buffer.erase();
            m_request_done = true;
            REQUEST()(m_command);
        }
    }

    virtual bool is_completed() const override
    {
        if (m_response_gotten) {
            return true;
        } else if (TIMEOUT()(m_timeout)) {
            if (REPEAT) {
                if (m_amount < AMOUNT) {
                    m_request_done = false;
                    ++m_amount;
                    return false;
                } else {
                    return true;
                }
            } else {
                return true;
            }
        } else {
            return false;
        }
    }

    virtual bool is_successful() const override
    {
        return m_response_gotten;
    }
};

#endif
