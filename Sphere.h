#pragma once
#include "Object.h"


class Sphere : public Object
{
public:
	unsigned int m_vao, m_vbo, m_ebo;
	Sphere(Shader& shader);
	Sphere(const char* texturePathDiffuse, const char* texturePathSpecular, Shader& shader);
	void Draw(Shader& shader) override;
private:
	void buildSphere();
};