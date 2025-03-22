#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 instancePos;
layout (location = 2) in vec4 instanceColor;
layout (location = 3) in vec4 textPos;

uniform vec3 textColor;
uniform mat4 view;
uniform mat4 projection;

out vec4 oColor;

void main()
{
    gl_Position = projection * view * vec4(aPos + instancePos, 1.0f);
    oColor = instanceColor;
}