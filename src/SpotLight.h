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

	//Methods
	void Draw() const;
	void updateLightSpaceMatrix();

	//Setters for light properties
	void setLightPos(const glm::vec3&);
	void setAmbient(const glm::vec3& ambient);
	void setDiffuse(const glm::vec3& diffuse);
	void setSpecular(const glm::vec3& specular);
	void setConstant(const float constant);
	void setLinear(const float linear);
	void setQuadratic(const float quadratic);
	void setIntensity(const float i);
	void setNearPlane(const float&);
	void setFarPlane(const float&);
	void setShadowHeight(int);
	void setShadowWidth(int);
	void setLightSpaceMatrix(const glm::mat4&);
	void setShadowBias(const float&);
	void setShadowPassUpdate(bool val);
	void setShadowFOV(const float& fov);
	void setYaw(float i);
	void setPitch(float i);

	//Getters for light properties
	glm::vec3 getLightPos() const;
	glm::vec3 getLightDirection() const;
	glm::vec3 getAmbient() const;
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpecular() const;
	float getIntensity() const;
	float getNearPlane() const;
	float getFarPlane() const;
	GLuint& getDepthMapTexture();
	int getShadowHeight() const;
	int getShadowWidth() const;
	glm::mat4& getLightSpaceMatrix();
	float getShadowBias() const;
	float getShadowFOV() const;
	float getYaw() const;
	float getPitch() const;
	bool getShadowPassUpdate() const;


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

	//For Attenuation
	float m_Constant;
	float m_Linear;
	float m_Quadratic;
	float m_Intensity;
	float m_Yaw;
	float m_Pitch;

	//For Shadow Maps
	int m_ShadowHeight;
	int m_ShadowWidth;
	GLuint m_DepthMapTexture;
	float m_NearPlane;
	float m_FarPlane;
	float m_ShadowBias;
	glm::mat4 m_LightSpaceMatrix;
	glm::mat4 m_LightProjectionMatrix;
	glm::mat4 m_LightViewMatrix;
	bool m_ShadowPassUpdate;
	float m_ShadowFOV;
};