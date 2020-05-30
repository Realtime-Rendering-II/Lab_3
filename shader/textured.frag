#version 330 core

in vec3 position;
in vec3 normal;
in vec2 tex_coord;

uniform sampler2D custom_texture_1;

uniform vec3 light_diffuse_color;
uniform vec3 light_position;
uniform vec3 camera_position;

out vec4 out_color;

void main(){
    vec3 diffuse_object_color = texture(custom_texture_1, tex_coord).rgb;

    vec3 normal = normalize(normal);
    vec3 light_dir = normalize(light_position - position);

    float attenuation_factor = 1.0f / (1 + pow(length(light_dir), 2));
    vec3 diffuse = attenuation_factor * light_diffuse_color * diffuse_object_color * max(dot(light_dir, normal), 0.0);

    out_color = vec4(diffuse, 1.0);
}