#version 330 core
layout (location = 0) in vec3 aPos;

uniform int lightType; //0 = Point Light, 1 = Spot Light

uniform mat4 shadowPassMatrix;
uniform mat4 model;

void main()
{
	if(lightType == 0)
	{
		gl_Position = model * vec4(aPos, 1.0);
	}
	else if (lightType == 1)
	{
		gl_Position = shadowPassMatrix  * model * vec4(aPos, 1.0);
	}
}