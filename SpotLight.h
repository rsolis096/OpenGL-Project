#pragma once

#include "LightSource.h"

class SpotLight : public LightSource
{
public:
	SpotLight(std::vector<float> inputVertices, std::vector<float> inputNormals, Shader& lightingShader, Shader& lightCubeShader, Camera& cam);
	void renderLight();
	void initializeLight() ;
	~SpotLight();
};