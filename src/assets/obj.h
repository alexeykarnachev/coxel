// TODO: Factor out these arguments into a return structure
int load_obj(
    char* file_path,
    float** vp,
    size_t* vp_size,
    float** vn,
    size_t* vn_size,
    uint32_t** f,
    size_t* f_size
) {
    uint32_t* vp_f;
    uint32_t* vn_f;
    size_t vp_f_size;
    size_t vn_f_size;

    int res = -1;
    size_t n_bytes;
    char* content = read_cstr_file(file_path, "r", &n_bytes);
    if (content == NULL) {
        fprintf(stderr, "ERROR: failed to read the .obj source file `%s`: %s\n", file_path, strerror(errno));
        errno = 0;
        return res;
    }

    // TODO: I can restrict the size of these line buffers.
    // In reality they are definitely smaller than n_bytes.
    char* current_line = (char*)malloc(n_bytes);
    char* vp_lines = (char*)malloc(n_bytes);
    char* vn_lines = (char*)malloc(n_bytes);
    char* f_lines = (char*)malloc(n_bytes); 

    size_t max_line_length = 0;
    size_t line_length = 0;
    size_t vp_lines_length = 0;
    size_t vn_lines_length = 0;
    size_t f_lines_length = 0;
    size_t n_vp = 0;
    size_t n_vn = 0;
    size_t n_f = 0;
    size_t n_f_lines = 0;
    for (size_t i = 0; i < n_bytes; ++i) {
        char c = content[i];
        if (c == '\n') {
            if (current_line[0] == 'v' && current_line[1] == ' ') {
                memcpy(
                    &vp_lines[vp_lines_length], current_line, line_length);
                vp_lines_length += line_length;
                vp_lines[vp_lines_length++] = '\n';
                n_vp += 1;
            } else if (current_line[0] == 'f' && current_line[1] == ' ') {
                memcpy(
                    &f_lines[f_lines_length], current_line, line_length);

                size_t n_spaces = 0;
                for (size_t i = 0; i < line_length; ++i) {
                    n_spaces += current_line[i] == ' ';
                }
                f_lines_length += line_length;
                f_lines[f_lines_length++] = '\n'; 
                n_f += n_spaces - 2;
                n_f_lines += 1;
            } else if (
                current_line[0] == 'v'
                && current_line[1] == 'n'
                && current_line[2] == ' '
            ) {
                memcpy(
                    &vn_lines[vn_lines_length], current_line, line_length);

                vn_lines_length += line_length;
                vn_lines[vn_lines_length++] = '\n';
                n_vn += 1;
            }

            max_line_length = max(max_line_length, line_length + 2);
            line_length = 0;
        } else {
            current_line[line_length++] = c;
        }
    }

    vp_lines[vp_lines_length++] = '\0';
    vn_lines[vn_lines_length++] = '\0';
    f_lines[f_lines_length++] = '\0';

    // TODO: Factor these chunks of code below out
    char c;
    size_t c_idx = 0;
    line_length = 0;
    char line[max_line_length];
    size_t value_length = 0;
    char value[max_line_length];
    size_t n_values_parsed = 0;
    *vp_size = 3 * n_vp * sizeof(float);
    *vp = (float*)malloc(*vp_size);
    for (size_t i = 0; i < n_vp; ++i) {
        do {
            c = vp_lines[c_idx++];
            line[line_length++] = c;
        } while (c != '\n');

        line[line_length] = '\0';

        for (size_t j = 0; j < line_length; ++j) {
            c = line[j];
            if ((c >= '0' && c <= '9') || c == '-' || c == '.') {
                value[value_length++] = c;
            } else if (value_length > 0) {
                value[value_length] = '\0';
                (*vp)[n_values_parsed++] = atof(value);
                value_length = 0;
            }
        }
        line_length = 0;
    }

    c_idx = 0;
    line_length = 0;
    value_length = 0;
    n_values_parsed = 0;
    *vn_size = 3 * n_vn * sizeof(float);
    *vn = (float*)malloc(*vn_size);
    for (size_t i = 0; i < n_vn; ++i) {
        do {
            c = vn_lines[c_idx++];
            line[line_length++] = c;
        } while (c != '\n');

        line[line_length] = '\0';

        for (size_t j = 0; j < line_length; ++j) {
            c = line[j];
            if ((c >= '0' && c <= '9') || c == '-' || c == '.') {
                value[value_length++] = c;
            } else if (value_length > 0) {
                value[value_length] = '\0';
                (*vn)[n_values_parsed++] = atof(value);
                value_length = 0;
            }
        }
        line_length = 0;
    }

    c_idx = 0;
    line_length = 0;
    value_length = 0;
    size_t n_vp_values_parsed = 0;
    size_t n_vn_values_parsed = 0;
    size_t a_idx = 0;
    size_t v_idx = 0;

    vp_f_size = 3 * n_f * sizeof(uint32_t);
    vn_f_size = 3 * n_f * sizeof(uint32_t);
    vn_f = (uint32_t*)malloc(vn_f_size);
    vp_f = (uint32_t*)malloc(vp_f_size);
    for (size_t i = 0; i < n_f_lines; ++i) {
        do {
            c = f_lines[c_idx++];
            line[line_length++] = c;
        } while (c != '\n');

        line[line_length] = '\0';
        
        for (size_t j = 0; j < line_length; ++j) {
            c = line[j];
            if (c >= '0' && c <= '9') {
                value[value_length++] = c;
            } else if (value_length > 0) {
                value[value_length] = '\0';
                if (a_idx == 0) {
                    if (v_idx >= 3) {
                        vp_f[n_vp_values_parsed] =
                            vp_f[n_vp_values_parsed - v_idx];
                        vp_f[n_vp_values_parsed + 1]
                            = vp_f[n_vp_values_parsed - 1];
                        n_vp_values_parsed += 2;
                    }
                    vp_f[n_vp_values_parsed++] = atoi(value) - 1;
                } else if (a_idx == 2) {
                    if (v_idx >= 3) {
                        vn_f[n_vn_values_parsed] =
                            vn_f[n_vn_values_parsed - v_idx];
                        vn_f[n_vn_values_parsed + 1]
                            = vn_f[n_vn_values_parsed - 1];
                        n_vn_values_parsed += 2;
                    }
                    vn_f[n_vn_values_parsed++] = atoi(value) - 1;
                }

                if (c == ' ') {
                    v_idx += 1;
                    a_idx = 0;
                } else if (c == '/') {
                    a_idx += 1;
                } else if (c == '\n') {
                    v_idx = 0;
                    a_idx = 0;
                }

                value_length = 0;
            } else if (c == '/') {
                a_idx += 1;
            }
        }

        v_idx = 0;
        line_length = 0;
    }

    float* vp_flat = malloc(sizeof(float) * 3 * 3 * n_f);
    float* vn_flat = malloc(sizeof(float) * 3 * 3 * n_f);
    *f = malloc(sizeof(size_t) * 3 * n_f);

    for (size_t i = 0; i < n_f * 3; ++i) {
        (*f)[i] = i;

        memcpy(
            &vp_flat[i * 3],
            &(*vp)[vp_f[i] * 3], 3 * sizeof(float));
        memcpy(
            &vn_flat[i * 3],
            &(*vn)[vn_f[i] * 3], 3 * sizeof(float));
    }

    free(*vp);
    free(*vn);
    *vp = vp_flat;
    *vn = vn_flat;
    *f_size = n_f * 3 * sizeof(uint32_t);
    *vp_size = n_f * 3 * 3 * sizeof(float);
    *vn_size = n_f * 3 * 3 * sizeof(float);

    res = 1;
free:
    free(content);
    free(current_line);
    free(vp_lines);
    free(vn_lines);
    free(f_lines);

    return res;
}
