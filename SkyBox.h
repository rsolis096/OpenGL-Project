#pragma once

#include "Object.h"
#include "Camera.h"
#include "Texture.h"

class SkyBox
{
public:
	std::vector<std::string> m_Faces;
	std::vector<float> m_Vertices;
	unsigned int m_SkyBoxVAO, m_SkyBoxVBO;
	Camera* m_PlayerCamera;
	Shader m_CubeMapShader;
	SkyBox(Shader&, Camera*);
	~SkyBox();
	void draw(glm::mat4&);
	Texture* m_CubeMap;
	int setCubeMapTexture(std::vector<std::string>);
	int setCubeMapTexture(std::string);

};