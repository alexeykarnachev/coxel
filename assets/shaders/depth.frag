#version 460 core

in VertexData {
    float depth;
} fs_in;

void main() {
    gl_FragDepth = fs_in.depth;
}

