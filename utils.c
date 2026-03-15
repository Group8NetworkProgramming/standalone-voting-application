// utils.c
#include <stdio.h>
#include <string.h>
#include "voter.h"

int read_record(const char *filename, long index, void *out, size_t record_size) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) return 0;

    fseek(f, index * record_size, SEEK_SET);
    int result = fread(out, record_size, 1, f) == 1;
    fclose(f);
    return result;
}

int append_record(const char *filename, void *record, size_t record_size) {
    FILE *f = fopen(filename, "ab+");
    if (f == NULL) return -1;

    fseek(f, 0, SEEK_END);
    int new_id = (ftell(f) / record_size) + 1;

    fwrite(record, record_size, 1, f);
    fclose(f);
    return new_id;
}