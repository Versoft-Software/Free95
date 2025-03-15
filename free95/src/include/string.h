#ifndef STRING_H
#define STRING_H
#include <stdint.h>
#include <stddef.h>

void outb(uint16_t port, uint8_t value);

void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);

void memset(void *dest, uint8_t val, uint32_t len);

int strcmp(char *str1, char *str2);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);

char upper(char c);

int isalpha(char c);

char *strncpy(char *s1, const char *s2, size_t n);

#endif