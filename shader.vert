#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aRGB;
layout (location = 1) in vec2 aTexCoord;

out vec3 aRGBf;
out vec2 TexCoord;

uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
   gl_Position = projection * view * model  * vec4(aPos, 1.0f);
   aRGBf = aRGB;
   TexCoord = vec2(aTexCoord.x, 1.0-aTexCoord.y);
};