#version 330 core

layout(location=0) in vec3 norm;
layout(location=1) in vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 trs;

out vec3 frag_norm;
out vec3 frag_pos;

void main() {
        mat4 final_model = model * trs;
        frag_pos = vec3(final_model * vec4(pos.xyz, 1.0));
        frag_norm = mat3(transpose(inverse(final_model))) * norm;

        mat4 mvp = projection * view * model;
        gl_Position = mvp * trs * vec4(pos.xyz, 1.0);
}
