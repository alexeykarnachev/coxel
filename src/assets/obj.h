int load_obj(char* file_path) {
    size_t n_bytes;
    char* content = read_cstr_file(file_path, "r", &n_bytes);
    if (content == NULL) {
        fprintf(stderr, "ERROR: failed to read the .obj source file `%s`: %s\n", file_path, strerror(errno));
        errno = 0;
        return -1;
    }

    // TODO: I can restrict the size of these line buffers.
    // In reality they are definitely smaller than n_bytes.
    char* current_line = (char*)malloc(n_bytes);
    char* vertex_lines = (char*)malloc(n_bytes);
    char* face_lines = (char*)malloc(n_bytes);
    size_t line_length = 0;
    size_t vertex_lines_length = 0;
    size_t face_lines_length = 0;
    size_t n_vertices = 0;
    size_t n_faces = 0;
    for (size_t i = 0; i < n_bytes; ++i) {
        char c = content[i];
        if (c == '\n') {
            if (current_line[0] == 'v' && current_line[1] == ' ') {
                vertex_lines[vertex_lines_length++] = '\n';
                memcpy(
                    &vertex_lines[vertex_lines_length],
                    current_line,
                    line_length
                );

                vertex_lines_length = vertex_lines_length + line_length;
                n_vertices += 1;
            } else if (current_line[0] == 'f' && current_line[1] == ' ') {
                face_lines[face_lines_length++] = '\n'; 
                memcpy(
                    &face_lines[face_lines_length],
                    current_line,
                    line_length
                );

                face_lines_length = face_lines_length + line_length;
                n_faces += 1;
            }

            line_length = 0;
        } else {
            current_line[line_length++] = c;
        }
    }

    vertex_lines[vertex_lines_length++] = '\0';
    face_lines[face_lines_length++] = '\0';

    printf("%s\n", face_lines);
    printf("%ld\n", n_faces);

    free(content);
    free(current_line);
    free(vertex_lines);
    free(face_lines);
}
