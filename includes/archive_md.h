#ifndef ARCHIVE_MD_H
#define ARCHIVE_MD_H

#define SIGNATURE 0xFFEFFE
#define FILE_EXT  "PA"
#define VER       1.0

typedef struct Archive_md {
    int   signature;
    float version;
    int   files_count;
} Archive_md;

#endif