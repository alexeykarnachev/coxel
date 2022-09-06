#include <stdio.h>
#include <assert.h>


char* read_text_file(const char* restrict file_path) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        perror(file_path);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(size + 1);
    if (fread(content, 1, size, file) != size) {
        perror(file_path);
        exit(1);
    }

    fclose(file);
    content[size] = '\0';

    return content;
}







