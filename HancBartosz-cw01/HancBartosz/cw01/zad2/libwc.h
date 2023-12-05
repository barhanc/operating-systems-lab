/*
Struktura pliku nagłówkowego oparta na
https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_C_libraries.html
*/

#ifndef _LIBWC_H_
#define _LIBWC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define LIBWC_COMMAND_BUFF_SIZE 1000

typedef struct libwc_mem
{
    char **data;
    size_t size;
    size_t curr_size;
    bool *occupied;
} libwc_mem;

extern libwc_mem libwc_create(size_t);

extern void libwc_destroy(libwc_mem *);

extern void libwc_delete_at(libwc_mem *, int);

extern char *libwc_get_at(libwc_mem *, int);

extern void libwc_wc(libwc_mem *, char *);

#endif