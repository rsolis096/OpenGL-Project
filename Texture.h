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
	Texture();
	Texture(const char* filePath, bool flipOnY);
	~Texture();

	//Cube Map
	Texture(std::vector<std::string> paths);


	//File Properties
	std::string type; //Specifies diffuse, specular, norma, height
	std::vector<std::string> path;

	void loadTexture(const char* filePath, bool flipOnY);
};