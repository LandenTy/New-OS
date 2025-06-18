// libc/string.c

#include "string.h"

int strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

void *memset(void *s, int c, unsigned int n) {
    unsigned char *p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

char *itoa(int value, char *str) {
    char *p = str;
    char *p1, *p2;
    unsigned int num = value;
    int negative = 0;

    if (value < 0) {
        negative = 1;
        num = -value;
    }

    do {
        *p++ = '0' + (num % 10);
        num /= 10;
    } while (num);

    if (negative) *p++ = '-';
    *p = '\0';

    // Reverse
    p1 = str;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1++ = *p2;
        *p2-- = tmp;
    }

    return str;
}

