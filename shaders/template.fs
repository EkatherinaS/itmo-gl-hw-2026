#version 330 core

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 view_pos;

in vec3 frag_col;
in vec3 frag_norm;
in vec3 frag_pos;

out vec4 out_col;

void main() {
        float ambient_strength = 0.2f;
        float diffuse_strength = 0.3f;
        float specular_strength = 0.5f;

        vec3 normal = normalize(frag_norm);
        vec3 light_dir = normalize(light_pos - frag_pos);
        vec3 view_dir = normalize(view_pos - frag_pos);

        vec3 ambient = ambient_strength * frag_col.rgb;

        float diffuse_cosine = max(dot(normal, light_dir), 0.0);
        vec3 diffuse = diffuse_strength * diffuse_cosine * mix(frag_col.rgb, light_color.rgb, 0.5);

        float specular_power = 32;
        vec3 reflect_dir = reflect(-light_dir, normal);
        float reflection_cosine = max(dot(view_dir, reflect_dir), 0.0);
        float specular_factor = pow(reflection_cosine, specular_power);
        vec3 specular = specular_strength * specular_factor * light_color.rgb;

        vec3 result = ambient + diffuse + specular;
        out_col = vec4(result, 1.0f);
}
