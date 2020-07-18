#ifndef FUNCTIONS_H
#define FUNCTIONS_H

template <class TYPE1, class TYPE2>
bool find(const TYPE1& buf1, unsigned int size1, const TYPE2& buf2, unsigned int size2)
{
    for (unsigned int i = 0; i < size1; ++i) {
        for (unsigned int j = 0; j < size2; ++j) {
            if (i + j >= size1) return false;
            if (buf1[i + j] == buf2[j]) {
                continue;
            } else {
                return false;
            }
        }
        return true;
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
