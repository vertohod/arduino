#ifndef FUNCTIONS_H
#define FUNCTIONS_H

template <class TYPE1, class TYPE2>
bool find(const TYPE1& buf1, unsigned int size1, const TYPE2& buf2, unsigned int size2)
{
    for (unsigned int i = 0; i < size1; ++i) {
        unsigned int counter_of_coincidence = 0;
        for (; counter_of_coincidence < size2; ++counter_of_coincidence) {
            if (i + counter_of_coincidence >= size1) return false;
            if (buf1[i + counter_of_coincidence] != buf2[counter_of_coincidence]) {
                break;
            } else {
                continue;
            }
        }
        if (counter_of_coincidence == size2) {
            return true;
        } else {
            continue;
        }
    }
    return false;
}

template <class TYPE>
unsigned int get_string_length(TYPE str)
{
    unsigned int counter = 0;
    while (str[counter] != 0) {
        ++counter;
    }
    return counter;
}

#endif
