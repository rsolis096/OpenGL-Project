#pragma once

#include "LightSource.h"

class PointLight : public LightSource
{
	public:
		PointLight(std::vector<float> inputVertices, std::vector<float> inputNormals, Shader& lightingShader, Shader& lightCubeShader, Camera& cam);
		void renderLight(glm::mat4 view, glm::mat4 projection);
		void initializeLight();
		~PointLight();
};