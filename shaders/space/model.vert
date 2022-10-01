#version 460 core

in vec3 model_pos;

out VertexData {
    vec4 model_pos;
} vs_out;


void main() {
    vs_out.model_pos = vec4(model_pos, 1.0);
    gl_Position = vs_out.model_pos;
}
