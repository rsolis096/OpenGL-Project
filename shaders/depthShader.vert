#version 330 core
layout (location = 0) in vec3 aPos;

//0 = Point Light, 1 = Spot Light, 2- Directional Light
uniform int lightType; 
uniform mat4 shadowPassMatrixDir;
uniform mat4 shadowPassMatrixSpot;
uniform mat4 model;

void main()
{
	if(lightType == 0)
	{
		gl_Position = model * vec4(aPos, 1.0);
	}
	else if (lightType == 1)
	{
		gl_Position = shadowPassMatrixSpot  * model * vec4(aPos, 1.0);
	}
	else if (lightType == 2)
	{
		gl_Position = shadowPassMatrixDir  * model * vec4(aPos, 1.0);
	}
}