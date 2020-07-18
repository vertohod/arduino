#ifndef VECTOR_H
#define VECTOR_H

#define FIRST_CAPACITY 4
#define DEFAULT_CAPACITY 0
#define INCREASE_STORE_SIZE 1.5

template <class TYPE, unsigned int CAPACITY = DEFAULT_CAPACITY>
class vector
{
private:
    unsigned int    m_size;

    unsigned int    m_store_size;
    TYPE*           m_store;

private:
    inline void allocate()
    {
        m_store_size = CAPACITY;
        if (m_store_size  > 0) {
            m_store = new TYPE[CAPACITY];
        }
    }

    void reallocate()
    {
        auto new_store_size = m_store_size * INCREASE_STORE_SIZE;
        if (new_store_size == 0) new_store_size = FIRST_CAPACITY;

        auto store_temp = m_store;
        m_store = new TYPE[new_store_size];
        m_store_size = new_store_size;

        for (unsigned int i = 0; i < m_size; ++i) {
            m_store[i] = store_temp[i];
        }
        delete[] store_temp;
    }

    vector(const vector&);
    vector& operator=(const vector&);

public:
    vector() : m_size(0)
    {
        m_store_size = CAPACITY;
        m_store = new TYPE[CAPACITY];
    }

    ~vector()
    {
        delete[] m_store;
    }

    unsigned int size()
    {
        return m_size;
    }

    void push_back(TYPE value)
    {
        if (m_size == m_store_size) {
            reallocate();
        }
        m_store[m_size++] = value;
    }

    TYPE& operator[](unsigned int index)
    {
        return m_store[index];
    }

    const TYPE& operator[](unsigned int index) const
    {
        return m_store[index];
    }

    void erase()
    {
        m_size = 0;
        if (m_store_size != CAPACITY) {
            delete[] m_store;
            allocate();
        }
    }
};

#endif
