#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 VertPos;
in float oStep;

in vec3 oNormal;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	vec3 LIGHT_COLOR = vec3(1.0);
	vec3 LIGHT_POSITION = vec3(0.0f,-1.0f,-1.0f);


	// vec4 textureCol = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
	vec4 textureCol = texture(texture1 , TexCoord);
	FragColor = textureCol;
}