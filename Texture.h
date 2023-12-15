#pragma once

#include "Vendors/stb_image.h"
#include <glad/glad.h>
#include <vector>
#include <iostream>
#include <string>

//Reads texture from file

class Texture
{
public:

	//Texture ID (Each texture object has a different ID set by glGenTextures(1, &ID); Max of 32 texture units
	unsigned int ID;
	//Specifies diffuse, specular, norma, height, texture
	std::string m_Type;
	//Path used by GUI for readability, otherwise it isn't really needed
	std::string m_Path;

	Texture();
	Texture(const char* filePath, bool flipOnY, std::string type);
	~Texture();

	//Cube Map
	Texture(std::vector<std::string> paths);

	void updateTexture(const char* filePath, bool flipOnY);
};