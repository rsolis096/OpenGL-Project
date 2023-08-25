#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>

#include "Cube.h"
#include "Sphere.h"
#include "Shader.h"
#include "Camera.h"

class LightSource
{
public:
	//LightSource();
	LightSource(std::vector<float> inputVertices, std::vector<float> inputNormals, Shader& lightingShader, Shader& lightCubeShader, Camera& cam);
	~LightSource();

	//virtual void renderLight() = 0;
	//virtual void initializeLight();
	
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

	//Reference to the camera
	Camera& playerCamera;

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