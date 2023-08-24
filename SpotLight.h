#pragma once

#include "LightSource.h"

class SpotLight : public LightSource
{
public:
	SpotLight(std::vector<float> inputVertices, std::vector<float> inputNormals, const char* type, Shader& lightingShader, Shader& lightCubeShader);
	void renderLight(glm::vec3 cameraPos, glm::mat4 projection, glm::mat4 view) override;
	void initializeLight(glm::vec3 cameraPos, glm::vec3 cameraFront) override;

	~SpotLight();
};