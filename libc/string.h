// libc/string.h

#ifndef STRING_H
#define STRING_H

int strlen(const char *s);
void *memset(void *s, int c, unsigned int n);
void clear_screen(void);
int strlen(const char *s);
void *memset(void *s, int c, unsigned int n);
void *memcpy(void *dest, const void *src, unsigned int n);
int memcmp(const void *s1, const void *s2, unsigned int n);
char *itoa(int value, char *str);

#endif

