#pragma once

#include "LightSource.h"

class SpotLight : public LightSource
{
public:
	SpotLight(std::vector<float> inputVertices, std::vector<float> inputNormals, Shader& lightingShader, Shader& lightCubeShader, Camera& cam);
	void renderLight();
	~SpotLight();

	//For Light Properties
	void setLightPos(glm::vec3 lightPos);
	void setAmbient(glm::vec3 ambient);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);

	//For Attenuation
	void setconstant(float constant);
	void setLinear(float linear);
	void setQuadratic(float quadratic);
};