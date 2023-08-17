#include "stb_image.h"
#include <glad/glad.h>
#include <vector>

class Texture
{
public:
	static int textureCount;
	unsigned int ID;
	int m_Width, m_Height, m_nrChannels;
	Texture(const char* filePath, bool flipOnY, unsigned int format);
	//~Texture();
	//void Bind() const;
};

