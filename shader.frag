#version 330 core

out vec4 FragColor;

in vec3 aRGBf;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float opacity;

void main()
{
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), opacity);
	//FragColor = texture(texture1, TexCoord) * vec4(aRGBf, 1.0);
   //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
   //FragColor = vec4(aRGBf, 1.0f);
};