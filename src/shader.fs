#version 330 core

uniform vec2 screen_size;
in vec4 gl_FragCoord ;
in vec4 oColor;
out vec4 fragColor;
void main()
{     
      fragColor = oColor;
}