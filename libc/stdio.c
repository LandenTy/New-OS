#include "stdio.h"
#include "../libc/string.h"
#include <stdarg.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
static volatile char *video = (volatile char *)0xb8000;
static int cursor = 0;

void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x07;
    }
    cursor = 0;
}

void putchar(char c) {
    if (c == '\n') {
        cursor = (cursor / VGA_WIDTH + 1) * VGA_WIDTH;
        return;
    }

    video[cursor * 2] = c;
    video[cursor * 2 + 1] = 0x07; // White on black
    cursor++;

    if (cursor >= VGA_WIDTH * VGA_HEIGHT) {
        cursor = 0; // Simple overflow behavior (wrap)
    }
}

void puts(const char *s) {
    while (*s) {
        putchar(*s++);
    }
}

// Helper: convert unsigned int to hex string (lowercase)
void utoa_hex(unsigned int value, char* buffer, int min_width) {
    const char* digits = "0123456789abcdef";
    char temp[9]; // max 8 hex digits + null
    int pos = 0;

    if (value == 0) {
        temp[pos++] = '0';
    } else {
        while (value > 0 && pos < 8) {
            temp[pos++] = digits[value & 0xF];
            value >>= 4;
        }
    }

    // Pad with zeros if needed
    while (pos < min_width) {
        temp[pos++] = '0';
    }

    // Reverse string into buffer
    int i;
    for (i = 0; i < pos; i++) {
        buffer[i] = temp[pos - i - 1];
    }
    buffer[pos] = '\0';
}

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    const char *s;
    int d;
    char buf[32];

    for (int i = 0; format[i]; i++) {
        if (format[i] == '%') {
            i++;

            // Check for zero padding like %02x
            int zero_pad = 0;
            if (format[i] == '0' && format[i+1] == '2') {
                zero_pad = 2;
                i += 2;
            }

            if (format[i] == 's') {
                s = va_arg(args, const char *);
                puts(s);
            } else if (format[i] == 'd') {
                d = va_arg(args, int);
                itoa(d, buf);
                puts(buf);
            } else if (format[i] == 'c') {
                putchar(va_arg(args, int));
            } else if (format[i] == 'x') {
                unsigned int x = va_arg(args, unsigned int);
                utoa_hex(x, buf, zero_pad);
                puts(buf);
            } else {
                putchar('%');
                putchar(format[i]); // fallback for unknown format
            }
        } else {
            putchar(format[i]);
        }
    }

    va_end(args);
}

