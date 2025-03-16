/*
 * Free 95
 *
 * NAME:
 *		string.h
 *
 * DESCRIPTION:
 *		String operations.
 *
 * Author: Rainy101112
 *
 */

#ifndef INCLUDE_STRING_H_
#define INCLUDE_STRING_H_

#include "stdint.h"
#include "stddef.h"

int memcmp(const void *buffer1, const void *buffer2, size_t count);

void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);

void memset(void *dest, uint8_t val, uint32_t len);

void bzero(void *dest, uint32_t len);

void memclean(char *s, int len);

int strcmp(const char *str1, const char *str2);

char *strcpy(char *dest, const char *src);

char *strncpy(char *dest, const char *src, uint32_t len);

char *strcat(char *dest, const char *src);

char *strchr(char *str, int c);

char *strchrnul(const char *s, int c);

int strlen(const char *src);

void delete_char(char *str, int pos);

void insert_char(char *str, int pos, char ch);

void insert_str(char *str, char *insert_str, int pos);

char *strupr(char *src);

char *strlwr(char *src);

void *strdup(const char *s);

int strncmp(const char *s1, const char *s2, size_t n);

int streq(const char *s1, const char *s2);

#endif // INCLUDE_STRING_H_