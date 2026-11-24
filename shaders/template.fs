#version 330 core

in vec3 frag_norm;
in vec3 frag_pos;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 view_pos;

out vec4 out_col;

void main() {
        float ambient_strength = 0.4f;
        float diffuse_strength = 0.3f;
        float specular_strength = 0.3f;
        vec3 frag_color = vec3(1.0, 1.0, 1.0);

        vec3 ambient = ambient_strength * frag_color.rgb;

        vec3 normal = normalize(frag_norm);
        vec3 light_dir = normalize(light_pos - frag_pos);
        vec3 view_dir = normalize(view_pos - frag_pos);

        float diffuse_cosine = max(dot(normal, light_dir), 0.0);
        vec3 diffuse = diffuse_strength * diffuse_cosine * mix(frag_color.rgb, light_color.rgb, 0.5);

        float specular_power = 32;
        vec3 reflect_dir = reflect(-light_dir, normal);
        float reflection_cosine = max(dot(view_dir, reflect_dir), 0.0);
        float specular_factor = pow(reflection_cosine, specular_power);
        vec3 specular = specular_strength * specular_factor * light_color.rgb;

        vec3 result = ambient + diffuse + specular;
        out_col = vec4(result, 1.0f);
}
