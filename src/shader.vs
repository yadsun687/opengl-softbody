#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aStep;

out vec2 TexCoord;
out vec3 VertPos;
out float oStep;

uniform mat4 transform;

void main()
{	
	oStep = aStep;
	vec4 pos = transform * vec4(aPos, 1.0) + vec4(aStep,0.0f,0.0f,1.0f);

	VertPos = pos.xyz;
	gl_Position = transform * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}