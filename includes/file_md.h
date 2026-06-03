#ifndef FILE_MD_H
#define FILE_MD_H

#include <stdio.h>

typedef struct File_md {
    size_t size;
    int    name_length;
    char*  name;
} File_md;

#endif