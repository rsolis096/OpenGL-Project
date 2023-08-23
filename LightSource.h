#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>

#include "Cube.h"
#include "Sphere.h"
#include "Shader.h"

class LightSource
{
public:
	LightSource();
	LightSource(std::vector<float> inputVertices, std::vector<float> inputNormals, const char* type, Shader& lightingShader, Shader& lightCubeShader);
	virtual void renderLight(glm::mat4 projection, glm::mat4 view) = 0;
	~LightSource();
	void initializeLight(glm::vec3 cameraPos);
	
	//For Light Properties
	void setLightPos(glm::vec3 lightPos);
	void setAmbient(glm::vec3 ambient);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);

	//For Attenuation
	void setconstant(float constant);
	void setLinear(float linear);
	void setQuadratic(float quadratic);

protected:

	//Shaders (for lighting and for light object itself
	Shader m_lightShapeShader;
	Shader m_lightingShader;

	//Light Object (light is not invisible)
	Cube* m_LightShape;

	//World Position
	glm::vec3 m_LightPos;

	//Light properties
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;

	//For Attenuation
	float m_Constant;
	float m_Linear;
	float m_Quadratic;
};