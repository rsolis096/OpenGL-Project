#pragma once
#include "Object.h"

class Cube : public Object
{
public:
	unsigned int m_vao, m_vbo;
	Cube(Shader& shader);
	Cube(const char* texturePathDiffuse, const char* texturePathSpecular, Shader& shader);
	void Draw(Shader& shader) override;
private:
	void buildCube();
};