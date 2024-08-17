#version 330 core
layout (location = 0) in vec3 aPos;


uniform mat4 shadowPassMatrix;
uniform mat4 model;

void main()
{
	
		gl_Position = shadowPassMatrix * model * vec4(aPos, 1.0);
	
}