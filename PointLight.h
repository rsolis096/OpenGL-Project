#pragma once

#include "LightSource.h"

class PointLight : public LightSource
{
	public:
		PointLight(std::vector<float> inputVertices, std::vector<float> inputNormals, const char* type, Shader& lightingShader, Shader& lightCubeShader);
		void renderLight(glm::mat4 projection, glm::mat4 view) override;
		~PointLight();

};