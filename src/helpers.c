#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../includes/colors.h"
#include "../includes/helpers.h"

size_t get_file_size(FILE* fh) {
    struct stat st;
    fstat(fileno(fh), &st);
    return st.st_size;
}

int get_file_metadata_index(File_md files_metadata[], int files_count, char* name) {
    for (int i = 0; i < files_count; i++) {
        if (strcmp(files_metadata[i].name, name) == 0) {
            return i;
        }
    }
    printf(RED "file not found\n" REST);
    exit(1);
}

Archive_md read_archive_header(FILE* arc_file_handler) {
    Archive_md header;
    fread(&header, sizeof(Archive_md), 1, arc_file_handler);

    if (header.signature != SIGNATURE) {
        printf(RED "Signature doesn't match!\n" REST);
        exit(1);
    }

    return header;
}

File_md* read_file_metadata(FILE* arc_file_handler, int files_count) {
    File_md* files_metadata = malloc(sizeof(File_md) * files_count);

    for (int i = 0; i < files_count; i++) {
        fread(&files_metadata[i].size, sizeof(files_metadata[i].size), 1, arc_file_handler);
        fread(&files_metadata[i].name_length, sizeof(files_metadata[i].name_length), 1, arc_file_handler);
        files_metadata[i].name = malloc(files_metadata[i].name_length + 1);
        fread(files_metadata[i].name, files_metadata[i].name_length, 1, arc_file_handler);
    }

    return files_metadata;
}

void free_file_metadata(File_md* files_metadata, int files_count) {
    if (!files_metadata) {
        return;
    }
    for (int i = 0; i < files_count; i++) {
        free(files_metadata[i].name);
    }
    free(files_metadata);
}

void write_metadata(FILE* file_handler, File_md f) {
    fwrite(&f.size, sizeof(f.size), 1, file_handler);
    fwrite(&f.name_length, sizeof(f.name_length), 1, file_handler);
    fwrite(f.name, 1, f.name_length, file_handler);
}

char* get_file_basename(char* path, int length) {
    if (!path || !length) {
        return NULL;
    }

    if (!strchr(path, '/')) {
        return path;
    }

    char* last_slash = path + length - 1;

    while (*last_slash != '/') {
        last_slash--;
    }

    return last_slash + 1;
}
