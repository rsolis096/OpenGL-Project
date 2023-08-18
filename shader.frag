#version 330 core

out vec4 FragColor;

in vec3 aRGBf;
in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
	vec2 myVec2 = vec2(0.5, 0.7);
	//FragColor = vec4(myVec2, 0.0f, 1.0f);
	//FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 1.0f);
	FragColor = texture(texture1, TexCoord);
};