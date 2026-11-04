#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 norm;
layout(location=2) in vec3 col;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_norm;
out vec3 frag_col;
out vec3 frag_pos;

void main() {
        frag_col = col;
        frag_pos = vec3(model * vec4(pos.xyz, 1.0));
        frag_norm = mat3(transpose(inverse(model))) * norm;

        mat4 mvp = projection * view * model;
        gl_Position = mvp * vec4(pos.xyz, 1.0);
}
