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
	Texture(const char* filePath, bool flipOnY);
	Texture();
	~Texture();

	//File Properties
	std::string type; //Specifies diffuse, specular, norma, height
	std::string path;
};