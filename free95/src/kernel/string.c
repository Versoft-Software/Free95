/*
 * Free 95
 *
 * NAME:
 *		string.c
 *
 * DESCRIPTION:
 *		String operations.
 *
 * Author: Rainy101112
 *
 */

#include "string.h"
#include "pmm.h"
#include <stdint.h>
#include <stddef.h>

inline int memcmp(const void *buffer1, const void *buffer2, size_t count)
{
    const char *a = buffer1;
    const char *b = buffer2;
    while (count-- > 0)
    {
        if (*a != *b)
            return *a > *b ? 1 : -1;
        a++, b++;
    }
    return 0;
}

inline void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len)
{
    uint8_t *sr = (uint8_t *)src;
    uint8_t *dst = dest;

    while (len != 0)
    {
        *dst++ = *sr++;
        len--;
    }
}

inline void memset(void *dest, uint8_t val, uint32_t len)
{
    for (uint8_t *dst = (uint8_t *)dest; len != 0; len--)
    {
        *dst++ = val;
    }
}

inline void bzero(void *dest, uint32_t len)
{
    memset(dest, 0, len);
}

void memclean(char *s, int len)
{
    int i;
    for (i = 0; i != len; i++)
    {
        s[i] = 0;
    }
    return;
}

inline int strcmp(const char *dest, const char *src)
{
    int ret = 0;

    while (!(ret = *(unsigned char *)src - *(unsigned char *)dest) && *dest)
    {
        ++src;
        ++dest;
    }
    if (ret < 0)
    {
        ret = -1;
    }
    else if (ret > 0)
    {
        ret = 1;
    }
    return ret;
}

inline char *strcpy(char *dest, const char *src)
{
    char *tmp = dest;

    while (*src)
    {
        *dest++ = *src++;
    }
    *dest = '\0';
    return tmp;
}

char *strncpy(char *dest, const char *src, uint32_t len)
{
    char *dst = dest;

    while (len > 0)
    {
        while (*src)
        {
            *dest++ = *src++;
        }
        len--;
    }
    *dest = '\0';
    return dst;
}

inline char *strcat(char *dest, const char *src)
{
    char *cp = dest;

    while (*cp)
    {
        cp++;
    }
    while ((*cp++ = *src++))
        ;
    return dest;
}

char *strchr(char *str, int c)
{
    for (; *str != 0; ++str)
    {
        if (*str == c)
        {
            return str;
        }
    }
    return 0;
}

char *strchrnul(const char *s, int c)
{
    char *p = (char *)s;
    while (*p && *p != (char)c)
    {
        p++;
    }
    return p;
}

inline int strlen(const char *src)
{
    const char *eos = src;

    while (*eos++)
        ;
    return (eos - src - 1);
}

void delete_char(char *str, int pos)
{
    int i;
    for (i = pos; i < strlen(str); i++)
    {
        str[i] = str[i + 1];
    }
}

void insert_char(char *str, int pos, char ch)
{
    int i;
    for (i = strlen(str); i >= pos; i--)
    {
        str[i + 1] = str[i];
    }
    str[pos] = ch;
}

void insert_str(char *str, char *insert_str, int pos)
{
    for (int i = 0; i < strlen(insert_str); i++)
    {
        insert_char(str, pos + i, insert_str[i]);
    }
}

char *strupr(char *src)
{
    while (*src != '\0')
    {
        if (*src >= 'a' && *src <= 'z')
            *src -= 32;
        src++;
    }
    return src;
}

char *strlwr(char *src)
{
    while (*src != '\0')
    {
        if (*src > 'A' && *src <= 'Z')
        {
            //*src += 0x20;
            *src += 32;
        }
        src++;
    }
    return src;
}

void *strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    void *p = (void *)malloc(len);
    if (p != 0)
        memcpy(p, (uint8_t *)s, len);
    return p;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    const unsigned char *p1 = (const unsigned char *)s1,
                        *p2 = (const unsigned char *)s2;
    while (n-- > 0)
    {
        if (*p1 != *p2)
            return *p1 - *p2;
        if (*p1 == '\0')
            return 0;
        p1++, p2++;
    }
    return 0;
}

int streq(const char *s1, const char *s2)
{
    return strcmp(s1, s2) == 0;
}
