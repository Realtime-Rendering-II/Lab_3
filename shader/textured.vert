#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec3 position;
out vec3 normal;
out vec2 tex_coord;

void main(){
    gl_Position = projection * view * model *  vec4(in_position, 1.0);

    position = (model *  vec4(in_position, 1.0)).xyz;
    normal = (model* vec4(in_normal, 1.0)).xyz;
    tex_coord = in_tex_coord;
}