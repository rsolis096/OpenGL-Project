#pragma once

#include "Lighting/Shader.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Camera;
class Texture;

class SkyBox
{
public:
	std::vector<float> m_Vertices;
	unsigned int m_SkyBoxVAO, m_SkyBoxVBO;
	Camera* m_PlayerCamera;
	Shader m_CubeMapShader;
	SkyBox(Shader&, Camera*);
	~SkyBox();
	void draw(glm::mat4&);
	Texture* m_CubeMap;
	int setCubeMapTexture(int);
	static std::vector<std::vector<std::string>> m_CubeMapPaths;
	static std::vector<std::string> m_CubeMapNames;
	int m_Current;
	bool m_InvertedTexture;
};
