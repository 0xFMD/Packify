#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/colors.h"
#include "../includes/helpers.h"

#define COMMAND_NO_HELP           2
#define COMMAND_NO_CREATE_ARCHIVE 3
#define COMMAND_NO_LIST_FILE      3
#define COMMAND_NO_COUNT_FILE     3
#define COMMAND_NO_DELETE_FILE    4
#define COMMAND_NO_INSERT_FILE    4
#define COMMAND_NO_EXTRACT_FILE   4
#define COMMAND_NO_REPLACE_FILE   5

void create_archive(char* filename) {
    char arc_name[strlen(filename) + strlen(FILE_EXT) + 2];
    sprintf(arc_name, "%s.%s", filename, FILE_EXT);
    FILE* arc = fopen(arc_name, "wb");
    if (!arc) {
        printf(RED "failed to create %s.%s\n" REST, filename, FILE_EXT);
        return;
    }

    Archive_md arc_header = {.signature = SIGNATURE, .version = VER, .files_count = 0};
    fwrite(&arc_header, sizeof(arc_header), 1, arc);

    fclose(arc);
    printf(GREEN "%s.%s created\n" REST, filename, FILE_EXT);
}

bool insert_file(char* arc_file_name, char* file_name, int index_to_insert) {
    FILE* arc_file_handler      = fopen(arc_file_name, "rb");
    FILE* inserted_file_handler = fopen(file_name, "rb");

    if (!arc_file_handler || !inserted_file_handler) {
        printf(RED "failed to insert %s in %s\n" REST, file_name, arc_file_name);
        return false;
    }

    // read the archive
    Archive_md     arc_header     = read_archive_header(arc_file_handler);
    File_md*       files_metadata = read_file_metadata(arc_file_handler, arc_header.files_count);
    unsigned char* data[arc_header.files_count];

    for (int i = 0; i < arc_header.files_count; i++) {
        data[i] = malloc(files_metadata[i].size);
        fread(data[i], files_metadata[i].size, 1, arc_file_handler);
    }

    fclose(arc_file_handler);

    File_md inserted_file_metadata = {.size = get_file_size(inserted_file_handler), .name_length = strlen(file_name)};
    inserted_file_metadata.name    = malloc(inserted_file_metadata.name_length + 1);
    strcpy(inserted_file_metadata.name, file_name);

    // write to the file
    arc_file_handler   = fopen(arc_file_name, "wb");
    int original_count = arc_header.files_count;
    arc_header.files_count++;

    // if it's out of range add it on the end
    if (index_to_insert > original_count || index_to_insert < 0) {
        index_to_insert = original_count;
    }
    fwrite(&arc_header, sizeof(Archive_md), 1, arc_file_handler);

    for (int i = 0; i < index_to_insert; i++) {
        write_metadata(arc_file_handler, files_metadata[i]);
    }
    write_metadata(arc_file_handler, inserted_file_metadata);

    for (int i = index_to_insert; i < original_count; i++) {
        write_metadata(arc_file_handler, files_metadata[i]);
    }

    unsigned char* new_data = malloc(inserted_file_metadata.size);
    fread(new_data, inserted_file_metadata.size, 1, inserted_file_handler);

    for (int i = 0; i < index_to_insert; i++) {
        fwrite(data[i], 1, files_metadata[i].size, arc_file_handler);
    }
    fwrite(new_data, 1, inserted_file_metadata.size, arc_file_handler);
    for (int i = index_to_insert; i < original_count; i++) {
        fwrite(data[i], 1, files_metadata[i].size, arc_file_handler);
    }

    fclose(arc_file_handler);
    fclose(inserted_file_handler);

    free(new_data);
    for (int i = 0; i < original_count; i++) {
        free(data[i]);
    }
    free(inserted_file_metadata.name);
    free_file_metadata(files_metadata, original_count);
    printf(GREEN "%s inserted in %s\n" REST, file_name, arc_file_name);
    return true;
}

void extract_file(char* arc_file_name, char* extract_file_name) {
    FILE* arc_file_handler = fopen(arc_file_name, "rb");
    if (!arc_file_handler) {
        printf(RED "can't open %s\n" REST, arc_file_name);
        exit(1);
    }

    Archive_md arc_header     = read_archive_header(arc_file_handler);
    File_md*   files_metadata = read_file_metadata(arc_file_handler, arc_header.files_count);
    int        the_one_index  = get_file_metadata_index(files_metadata, arc_header.files_count, extract_file_name);

    // skip not needed files
    for (int i = 0; i < the_one_index; i++) {
        fseek(arc_file_handler, files_metadata[i].size, SEEK_CUR);
    }

    unsigned char* extracted = malloc(files_metadata[the_one_index].size);
    fread(extracted, files_metadata[the_one_index].size, 1, arc_file_handler);

    FILE* out_fh = fopen(files_metadata[the_one_index].name, "wb");
    fwrite(extracted, files_metadata[the_one_index].size, 1, out_fh);

    fclose(out_fh);
    fclose(arc_file_handler);

    free(extracted);
    free_file_metadata(files_metadata, arc_header.files_count);
    printf(GREEN "%s extracted\n" REST, extract_file_name);
}

void list_files(char* arc_file_name) {
    FILE* fh = fopen(arc_file_name, "rb");
    if (!fh) {
        printf(RED "can't open %s\n" REST, arc_file_name);
        exit(1);
    }
    Archive_md arc_header = read_archive_header(fh);
    File_md*   file_md    = read_file_metadata(fh, arc_header.files_count);

    if (arc_header.files_count == 0) {
        printf(RED "archive is empty\n");
    }

    for (int i = 0; i < arc_header.files_count; i++) {
        printf("\n%s===== File %d =====\n name: %s\n size: %zu Bytes%s\n", BLUE, i + 1, file_md[i].name, file_md[i].size, REST);
    }

    free_file_metadata(file_md, arc_header.files_count);
    fclose(fh);
}

void count_files(char* arc_file_name) {
    FILE*      fh         = fopen(arc_file_name, "rb");
    Archive_md arc_header = read_archive_header(fh);

    printf("%sFile count: %s%s%d%s\n", BLUE, REST, RED, arc_header.files_count, REST);

    fclose(fh);
}

int delete_file(char* arc_file_name, char* delete_file_name) {
    FILE* arc_file_handler = fopen(arc_file_name, "rb");
    if (!arc_file_handler) {
        exit(1);
    }

    Archive_md     arc_header     = read_archive_header(arc_file_handler);
    File_md*       files_metadata = read_file_metadata(arc_file_handler, arc_header.files_count);
    unsigned char* data[arc_header.files_count];

    for (int i = 0; i < arc_header.files_count; i++) {
        data[i] = malloc(files_metadata[i].size);
        fread(data[i], files_metadata[i].size, 1, arc_file_handler);
    }

    fclose(arc_file_handler);

    int target_index   = get_file_metadata_index(files_metadata, arc_header.files_count, delete_file_name);
    int original_count = arc_header.files_count;
    arc_header.files_count--;

    arc_file_handler = fopen(arc_file_name, "wb");
    fwrite(&arc_header, sizeof(Archive_md), 1, arc_file_handler);

    for (int i = 0; i < original_count; i++) {
        if (i == target_index) {
            continue;
        }
        write_metadata(arc_file_handler, files_metadata[i]);
    }

    for (int i = 0; i < original_count; i++) {
        if (i == target_index) {
            continue;
        }
        fwrite(data[i], files_metadata[i].size, 1, arc_file_handler);
    }

    fclose(arc_file_handler);
    for (int i = 0; i < original_count; i++) {
        free(data[i]);
    }

    free_file_metadata(files_metadata, original_count);

    printf(GREEN "%s deleted\n" REST, delete_file_name);
    return target_index;
}

void replace_file(char* arc_file_name, char* target_file_name, char* new_file_name) {
    int deleted_index = delete_file(arc_file_name, target_file_name);
    insert_file(arc_file_name, new_file_name, deleted_index);
    printf(GREEN "%s replaced with %s\n" REST, target_file_name, new_file_name);
}

void print_help(char* program_name) {
    printf("Usage: " GREEN "%s <flag> <archive> [file] [new_file]\n\n" REST, program_name);

    printf(BLUE "\t\t\t\t\t\t\t" RED "Packify@ThreadX Copyright (c) 2026\n" REST);
    printf(BLUE "\t\t\t\t\t\t\t" RED "═════════════\n" REST);

    printf(BLUE "╔═════════════════════════════════════════════╗\t\t" REST);
    printf(BLUE "flags:\n" REST);

    printf(BLUE "║" REST "                                             " BLUE "║\t\t" REST);
    printf("  " RED "-c" REST " <archive>\t\t\t\t\t\tCreate a new archive\n");

    printf(BLUE "║" REST BOLD WHITE "    _____ _                        _" REST BOLD RED "__  __" REST "   " BLUE "║\t\t" REST);
    printf("  " RED "-i" REST " <archive> <file>\t\t\t\t\tInsert a file into the archive\n");

    printf(BLUE "║" REST BOLD WHITE "   |_   _| |__  _ __ ___  __ _  __| " REST BOLD RED "\\ \\/ /" REST "   " BLUE "║\t\t" REST);
    printf("  " RED "-x" REST " <archive> <file>\t\t\t\t\tExtract a file from the archive\n");

    printf(BLUE "║" REST BOLD WHITE "     | | | '_ \\| '__/ _ \\/ _` |/ _` |" REST BOLD RED "\\  /" REST "    " BLUE "║\t\t" REST);
    printf("  " RED "-l" REST " <archive>\t\t\t\t\t\tList all files in the archive\n");

    printf(BLUE "║" REST BOLD WHITE "     | | | | | | | |  __/ (_| | (_| |" REST BOLD RED "/  \\" REST "    " BLUE "║\t\t" REST);
    printf("  " RED "-n" REST " <archive>\t\t\t\t\t\tNumber of files in the archive\n");

    printf(BLUE "║" REST BOLD WHITE "     |_| |_| |_|_|  \\___| \\__,_|\\__,_" REST BOLD RED "/_/\\_\\" REST "  " BLUE "║\t\t" REST);

    printf("  " RED "-d" REST " <archive> <file>\t\t\t\t\tDelete a file from the archive\n");

    printf(BLUE "║" REST "                                             " BLUE "║\t\t" REST);
    printf("  " RED "-r" REST " <archive> <file to replace> <file to insert>\tReplace a file in the archive\n");

    printf(BLUE "╚═════════════════════════════════════════════╝" REST);
    printf("\n\t\t\t\t\t\t\t  " BG_RED "   " BG_BLUE "   " BG_GREEN "   " BG_WHITE "   " REST "\n\n");
}

int main(int argc, char** argv) {

    if (argc < 2) {
        printf(RED "missing arguments — run %s -h for help\n" REST, argv[0]);
        return 1;
    }

    if (argc == COMMAND_NO_HELP && argv[1][1] == 'h') {
        print_help(argv[0]);
        return 0;
    }

    char* arc_file_name = argv[2];

    if (argc == COMMAND_NO_CREATE_ARCHIVE && argv[1][1] == 'c') {
        create_archive(arc_file_name);

    } else if (argc == COMMAND_NO_INSERT_FILE && argv[1][1] == 'i') {
        char* inserted_file_name = get_file_basename(argv[3], strlen(argv[3]));
        insert_file(arc_file_name, inserted_file_name, -1);

    } else if (argc == COMMAND_NO_EXTRACT_FILE && argv[1][1] == 'x') {
        char* extract_file_name = get_file_basename(argv[3], strlen(argv[3]));
        extract_file(arc_file_name, extract_file_name);

    } else if (argc == COMMAND_NO_LIST_FILE && argv[1][1] == 'l') {
        list_files(arc_file_name);
    } else if (argc == COMMAND_NO_COUNT_FILE && argv[1][1] == 'n') {
        count_files(arc_file_name);

    } else if (argc == COMMAND_NO_DELETE_FILE && argv[1][1] == 'd') {
        char* delete_file_name = get_file_basename(argv[3], strlen(argv[3]));
        delete_file(arc_file_name, delete_file_name);

    } else if (argc == COMMAND_NO_REPLACE_FILE && argv[1][1] == 'r') {
        char* target_file_name = get_file_basename(argv[3], strlen(argv[3]));
        char* new_file_name    = get_file_basename(argv[4], strlen(argv[4]));
        replace_file(arc_file_name, target_file_name, new_file_name);

    } else {
        printf(RED "missing arguments — run %s -h for help\n" REST, argv[0]);
        return 1;
    }

    return 0;
}
