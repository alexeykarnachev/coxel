vec3 hdr(vec3 color, float gamma, float exposure) {
    vec3 res = vec3(1.0) - exp(-color * exposure);
    res = pow(res, vec3(1.0 / gamma));
    return res;
}

