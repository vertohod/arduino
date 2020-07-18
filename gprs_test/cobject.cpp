#include "cobject.h"

cobject::cobject() : m_counter(0)
{
}

cobject::~cobject()
{
}

void cobject::add_object(cobject* object)
{
    m_queue_of_commands.push_back(object);
}

void cobject::execute() const
{
    if (m_counter < m_queue_of_commands.size()) {
        auto object = m_queue_of_commands[m_counter];
        object->execute();

        if (object->is_completed()) {
            if (object->is_successful()) {
                ++m_counter;
            } else {
                m_counter = m_queue_of_commands.size();
            }
        }
    }
}

bool cobject::is_completed() const
{
    return m_counter == m_queue_of_commands.size();
}

bool cobject::is_successful() const
{
    return m_counter == m_queue_of_commands.size();
}

void cobject::clean()
{
    auto size = m_queue_of_commands.size();
    for (unsigned int i = 0; i < size; ++i) {
        m_queue_of_commands[i]->clean();
        delete m_queue_of_commands[i];
    }
    m_queue_of_commands.erase();
    m_counter = 0;
}
