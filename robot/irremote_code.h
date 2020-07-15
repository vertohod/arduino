#ifndef IRREMOTE_CODE_H
#define IRREMOTE_CODE_H

char translate_code(unsigned long code)
{
    if (code == 0xff38c7) return 'o';
    else
        if (code == 0xffffffff) return 'c';
    else
        if (code == 0xff10ef) return 'l';
    else
        if (code == 0xff5aa5) return 'r';
    else
        if (code == 0xff18e7) return 't';
    else
        if (code == 0xff4ab5) return 'd';
    else
        if (code == 0xff9867) return '0';
    else 
        if (code == 0xffa25d) return '1';
    else 
        if (code == 0xff629d) return '2';
    else 
        if (code == 0xffe21d) return '3';
    else 
        if (code == 0xff22dd) return '4';
    else 
        if (code == 0xff02fd) return '5';
    else 
        if (code == 0xffc23d) return '6';
    else 
        if (code == 0xffe01f) return '7';
    else 
        if (code == 0xffa857) return '8';
    else 
        if (code == 0xff906f) return '9';
    else 
        if (code == 0xff6897) return '*';
    else 
        if (code == 0xffb04f) return '#';
    else
        return ' ';
}

#endif
