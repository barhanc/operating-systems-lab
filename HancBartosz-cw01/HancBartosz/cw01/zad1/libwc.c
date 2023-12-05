#include "libwc.h"

/*
Utworzenie i zwrócenie struktury zawierającej:
-> Zainicializowaną zerami (calloc()) tablicę wskaźników w której będą przechowywane wskaźniki na
    bloki pamięci.
-> Rozmiar tablicy, tj. maksymalna ilość bloków jakie można zapisać.
-> Aktualny rozmiar, tj. ilość zapisanych bloków.
*/
libwc_mem libwc_create(size_t size)
{
    libwc_mem new_libwc_mem;

    new_libwc_mem.data = calloc(size, sizeof(char *));
    new_libwc_mem.size = size;
    new_libwc_mem.curr_size = 0;
    new_libwc_mem.occupied = calloc(size, sizeof(bool));

    return new_libwc_mem;
}

// Zwolnienie pamięci tablicy wskaźników.
void libwc_destroy(libwc_mem *libwc_mem)
{
    for (size_t i = 0; i < libwc_mem->size; i++)
    {
        if (libwc_mem->occupied[i])
        {
            free(libwc_mem->data[i]);
            libwc_mem->occupied[i] = false;
        }
    }
    libwc_mem->curr_size = 0;
    free(libwc_mem->data);
    free(libwc_mem->occupied);
}

// Usunięcie z pamięci bloku o zadanym indeksie.
void libwc_delete_at(libwc_mem *libwc_mem, int i)
{
    if (i >= libwc_mem->size)
    {
        printf("Index %d out of range\n", i);
        return;
    }

    if (!libwc_mem->occupied[i])
    {
        printf("Block at index %d is not occupied\n", i);
        return;
    }

    free(libwc_mem->data[i]);
    libwc_mem->occupied[i] = false;
    libwc_mem->curr_size -= 1;
}

// Zwrócenie zawartości bloku o zadanym indeksie.
char *libwc_get_at(libwc_mem *libwc_mem, int i)
{
    if (i >= libwc_mem->size)
    {
        printf("Index %d out of range\n", i);
        return "";
    }

    if (!libwc_mem->occupied[i])
    {
        printf("Block at index %d is not occupied\n", i);
        return "";
    }

    return libwc_mem->data[i];
}

// Przeprowadzenie procedury zliczania ilości linii i słów dla podanego pliku.
void libwc_wc(libwc_mem *libwc_mem, char *file_name)
{
    if (libwc_mem->curr_size == libwc_mem->size)
    {
        puts("No available space!");
        return;
    }

    if (fopen(file_name, "r") == NULL)
    {
        printf("File %s not found!\n", file_name);
        return;
    }

    int i_free = 0;
    for (size_t i = 0; i < libwc_mem->size; i++)
    {
        if (!libwc_mem->occupied[i])
        {
            i_free = i;
            break;
        }
    }

    char tmp_filename[] = "/tmp/wclib_XXXXXX";
    int tmp_file = mkstemp(tmp_filename);
    if (tmp_file == -1)
    {
        printf("Error occured while creating a tmp file!");
        return;
    }

    char syscommand1[1000] = "";
    strcat(syscommand1, "wc ");
    strcat(syscommand1, file_name);
    strcat(syscommand1, " > ");
    strcat(syscommand1, tmp_filename);

    int err = system(syscommand1);
    if (err == -1)
    {
        printf("Error occured while executing system command!");
        return;
    }

    char *buffer = 0;
    long length;
    FILE *f = fopen(tmp_filename, "r");
    fseek(f, 0L, SEEK_END);
    length = ftell(f);
    fseek(f, 0L, SEEK_SET);
    buffer = (char *)calloc(length, sizeof(char));
    int read_size = fread(buffer, sizeof(char), length, f);
    if (read_size < length)
    {
        printf("Error occured while reading from file %s", tmp_filename);
        return;
    }

    fclose(f);

    char syscommand2[LIBWC_COMMAND_BUFF_SIZE] = "";
    snprintf(syscommand2, LIBWC_COMMAND_BUFF_SIZE, "rm -f '%s' 2>/dev/null", tmp_filename);

    err = system(syscommand2);
    if (err == -1)
    {
        printf("Error occured while executing system command!");
        return;
    }

    libwc_mem->data[i_free] = buffer;
    libwc_mem->occupied[i_free] = true;
    libwc_mem->curr_size += 1;
}