#pragma once



#include "Shader.h"
#include "Sphere.h"

//Flashlight should inherit from spotlight or find another way to continuously update camera position
//Flashlights arent typically at eye sight anyways so i may be better to lower is position relative to cam

class SpotLight
{
public:

	SpotLight(Shader*, Shader* , glm::vec3 , glm::vec3 );
	~SpotLight();

	void Draw();
	
	//Setters for light properties
	void setLightPos(const glm::vec3);
	void setLightDirection(const glm::vec3);
	void setAmbient(const glm::vec3 ambient);
	void setDiffuse(const glm::vec3 diffuse);
	void setSpecular(const glm::vec3 specular);
	void setconstant(const float constant);
	void setLinear(const float linear);
	void setQuadratic(const float quadratic);
	void setIntensity(const float i);
	void setNearPlane(const float&);
	void setFarPlane(const float&);

	//Getters for light properties
	glm::vec3 getLightPos();
	glm::vec3 getLightDirection();
	glm::vec3 getAmbient();
	glm::vec3 getDiffuse();
	glm::vec3 getSpecular();
	float getIntensity();
	float getNearPlane();
	float getFarPlane();

	//Private Variables
	std::string m_DisplayName;
	unsigned short m_SpotLightID;
	static unsigned short m_SpotLightCount;

private:

	//Shaders (for lighting and for light source itself)
	Shader* m_LightingShader;
	Shader* m_LightSourceShader;

	//Light Object (light is not invisible)
	Sphere* m_LightShape;

	//Light properties
	glm::vec3 m_LightPos;
	glm::vec3 m_LightDirection; //Unique to spotlight
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;

	//For shadows
	float m_NearPlane;
	float m_FarPlane;

	//For Attenuation
	float m_Constant;
	float m_Linear;
	float m_Quadratic;
	float m_Intensity;

};