#version 330 core

// All in object space
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 color;

uniform mat4 model, view, projection;

out vec3 worldSpace_pos;
out vec3 worldSpace_norm;
out vec2 tex_coord;
out vec4 vertColor;

void main() {
    worldSpace_pos = vec3(view*model*vec4(pos, 1.0));
    worldSpace_norm = vec3(transpose(inverse(model))*vec4(norm, 0.0));
    tex_coord = uv;
    vertColor = vec4(color, 1.0);

    gl_Position = projection*view*model*vec4(pos, 1.0);
}
