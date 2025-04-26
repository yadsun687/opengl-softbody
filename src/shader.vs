#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 instancePos;
layout (location = 2) in vec4 instanceColor;
layout (location = 3) in vec4 textPos;

uniform vec3 textColor;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
out vec4 oColor;

void main()
{
    
    vec3 camera_right_dir = vec3(normalize(vec3(view[0][0] , view[1][0],view[2][0])));
    vec3 camera_up_dir = vec3(normalize(vec3(view[0][1] , view[1][1],view[2][1])));
    
    float quadSize = 1.0f;
    vec4 world_pos = vec4( (instancePos + (camera_right_dir*aPos.x*quadSize) + (camera_up_dir*aPos.y*quadSize) ) , 1.0f);
    gl_Position = projection * view * world_pos;

    //gl_Position = projection * view * vec4(aPos + instancePos, 1.0f);

    TexCoords = aPos.xy + vec2(0.5, 0.5); // Assuming aPos is from -0.5 to 0.5
    oColor = instanceColor;
}