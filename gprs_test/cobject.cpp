#include "cobject.h"

cobject::cobject() : m_counter(0), m_length(0), m_queue_of_commands(nullptr)
{
}

void cobject::add_object(cobject* object)
{
    cobject** temp_queue = new cobject*[m_length + 1];
    for (unsigned int i = 0; i < m_length; ++i) {
        temp_queue[i] = m_queue_of_commands[i];
    }
    temp_queue[m_length] = object; 
    m_queue_of_commands = temp_queue;
    ++m_length;
}

void cobject::execute() const
{
    if (m_counter < m_length) {
        auto object = m_queue_of_commands[m_counter];
        object->execute();

        if (object->is_completed()) {
            ++m_counter;
        }
    }
}

bool cobject::is_completed() const
{
    return m_counter == m_length;
}

bool cobject::is_successful() const
{
    return true;
}
