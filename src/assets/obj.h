int load_obj(
    char* file_path,
    uint32_t** faces,
    size_t* faces_size,
    float** vertices,
    size_t* vertices_size
) {
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
    size_t max_line_length = 0;
    size_t line_length = 0;
    size_t vertex_lines_length = 0;
    size_t face_lines_length = 0;
    size_t n_vertices = 0;
    size_t n_faces = 0;
    size_t n_face_lines = 0;
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

                size_t n_spaces = 0;
                for (size_t i = 0; i < line_length; ++i) {
                    n_spaces += current_line[i] == ' ';
                }
                face_lines_length = face_lines_length + line_length;
                n_faces += n_spaces - 2;
                n_face_lines += 1;
            }

            max_line_length = max(max_line_length, line_length + 2);
            line_length = 0;
        } else {
            current_line[line_length++] = c;
        }
    }

    vertex_lines[vertex_lines_length++] = '\0';
    face_lines[face_lines_length++] = '\0';

    char c;
    size_t c_idx = 0;
    line_length = 0;
    char line[max_line_length];
    size_t value_length = 0;
    char value[max_line_length];
    size_t n_values_parsed = 0;
    *vertices_size = 3 * n_vertices * sizeof(float);
    *vertices = (float*)malloc(*vertices_size);
    for (size_t i = 0; i < n_vertices; ++i) {
        do {
            c = vertex_lines[c_idx++];
            line[line_length++] = c;
        } while (c != '\n');

        line[line_length] = '\0';

        for (size_t j = 0; j < line_length; ++j) {
            c = line[j];
            if ((c >= '0' && c <= '9') || c == '-' || c == '.') {
                value[value_length++] = c;
            } else if (value_length > 0) {
                value[value_length] = '\0';
                (*vertices)[n_values_parsed++] = atof(value);
                value_length = 0;
            }
        }
        line_length = 0;
    }

    c_idx = 0;
    line_length = 0;
    value_length = 0;
    n_values_parsed = 0;
    size_t attrib_idx = 0;
    size_t value_idx = 0;
    *faces_size = 3 * n_faces * sizeof(uint32_t);
    *faces = (uint32_t*)malloc(*faces_size);
    for (size_t i = 0; i < n_face_lines; ++i) {
        do {
            c = face_lines[c_idx++];
            line[line_length++] = c;
        } while (c != '\n');

        line[line_length] = '\0';
        
        for (size_t j = 0; j < line_length; ++j) {
            c = line[j];
            if (c >= '0' && c <= '9') {
                value[value_length++] = c;
            } else if (value_length > 0) {
                value[value_length] = '\0';
                if (attrib_idx == 0) {
                    if (value_idx >= 3) {
                        (*faces)[n_values_parsed] =
                            (*faces)[n_values_parsed - 2];
                        (*faces)[n_values_parsed + 1]
                            = (*faces)[n_values_parsed - 1];
                        n_values_parsed += 2;
                    }
                    (*faces)[n_values_parsed++] = atoi(value);
                }

                if (c != '/') {
                    attrib_idx = 0;
                    value_idx += 1;
                } else {
                    attrib_idx += 1;
                }

                value_length = 0;
            }
        }

        value_idx = 0;
        line_length = 0;
    }

    free(content);
    free(current_line);
    free(vertex_lines);
    free(face_lines);
}
