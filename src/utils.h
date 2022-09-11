char* read_cstr_file(const char* restrict file_path) {
    FILE* file = NULL;
    char* content = NULL;

    file = fopen(file_path, "r");
    if (file == NULL) goto fail;
    if (fseek(file, 0, SEEK_END) < 0) goto fail;

    long size = ftell(file);
    if (size < 0) goto fail;
    if (fseek(file, 0, SEEK_SET) < 0) goto fail;

    content = malloc(size + 1);
    if (content == NULL) goto fail;

    fread(content, 1, size, file);
    if (ferror(file)) goto fail;

    content[size] = '\0';
    if (file) {
        fclose(file);
        errno = 0;
    }
    
    return content;

fail:
    if (file) {
        int e = errno;
        fclose(file);
        errno = e;
    }
    if (content) {
        free(content);
    }
    return NULL;
}

