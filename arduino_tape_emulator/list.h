#ifndef LIST_H
#define LIST_H

template <class T>
class list
{
public:
    class iterator
    {
    private:
        T value;
        item* prev;
        item* next;

    public:
        iterator() : prev(nullptr), next(nullptr) {}
        iterator& operator++()
        {
            value = next->value;
            prev = next->prev;
            next = next->next;
            return *this;
        }
        iterator operator++(int)
        {
            auto ret = *this;
            value = next->value;
            prev = next->prev;
            next = next->next;
            return ret;
        }
        iterator& operator--()
        {
            value = prev->value;
            prev = prev->prev;
            next = prev->next;
            return *this;
        }
        iterator operator--(int)
        {
            auto ret = *this;
            value = prev->value;
            prev = prev->prev;
            next = prev->next;
            return ret;
        }
    };

private:
    iterator* begin;
    iterator* end;

public:
    list() : first(nullptr), last(nullptr) {}

    void push(T value);
    void clean();
};

#endif
