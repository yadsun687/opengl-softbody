#version 330 core

uniform vec2 screen_size;
in vec4 gl_FragCoord ;
in vec2 TexCoords ;
in vec4 oColor;

out vec4 fragColor;
void main()
{     

      vec2 center = vec2(0.5, 0.5);
      float dist = length(TexCoords - center) * 2.0; // *2 to normalize to 0-1 range

      // Discard fragments outside the circle
      if (dist > 1.0) {
          discard;
      }

      // Output the color
      fragColor = oColor;
}