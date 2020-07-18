#ifndef VECTOR_H
#define VECTOR_H

#define BEGIN_STORE_SIZE 8
#define INCREASE_STORE_SIZE 1.4 

template <class TYPE>
class vector
{
private:
    unsigned int    m_size;

    unsigned int    m_store_size;
    TYPE*           m_store;

private:
    inline void allocate()
    {
        m_store_size = BEGIN_STORE_SIZE;
        m_store = new TYPE[BEGIN_STORE_SIZE];
    }

    void reallocate()
    {
        auto store_temp = m_store;
        m_store = new TYPE[m_store_size * INCREASE_STORE_SIZE];
        for (unsigned int i = 0; i < m_size; ++i) {
            m_store[i] = store_temp[i];
        }
        delete[] store_temp;
        m_store_size *= INCREASE_STORE_SIZE;
    }

    vector(const vector&);
    vector& operator=(const vector&);

public:
    vector() : m_size(0)
    {
        m_store_size = BEGIN_STORE_SIZE;
        m_store = new TYPE[BEGIN_STORE_SIZE];
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
        if (m_store_size != BEGIN_STORE_SIZE) {
            delete[] m_store;
            allocate();
        }
    }
};

#endif
