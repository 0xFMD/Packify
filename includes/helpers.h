#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>

#include "archive_md.h"
#include "file_md.h"

size_t     get_file_size(FILE* fh);
char*      get_file_basename(char* path, int length);
Archive_md read_archive_header(FILE* arc_file_handler);
File_md*   read_file_metadata(FILE* arc_file_handler, int files_count);
void       free_file_metadata(File_md* files_metadata, int files_count);
void       write_metadata(FILE* file_handler, File_md f);
int        get_file_metadata_index(File_md files_metadata[], int files_count, char* name);

#endif
