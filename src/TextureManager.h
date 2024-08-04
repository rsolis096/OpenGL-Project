#pragma once

#include <iostream>
struct TextureManager
{
	//Variables
	static const int m_MaxUnits = 192;
	static unsigned int m_HighestUnit;
	static unsigned int m_CurrentUnit;

	//Functions
	static unsigned int getNextUnit();
	static unsigned int getCurrentUnit();
	static void reset();

};

/*
int maxTextureSize;
glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
std::cout << "Max Texture Size: " << maxTextureSize << std::endl;
*/