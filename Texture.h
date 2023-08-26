#pragma once

#include "Vendors/stb_image.h"
#include <glad/glad.h>
#include <vector>
#include <iostream>
#include <string>

class Texture
{
public:
	unsigned int ID;
	int m_Width, m_Height, m_nrChannels;
	Texture(const char* filePath, bool flipOnY, unsigned int format);
	Texture();
	~Texture();

	//File Properties
	std::string type; //Specifies diffuse, specular, norma, height
	std::string path;
};