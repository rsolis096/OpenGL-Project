#pragma once

#include <iostream>

#include "Shader.h"

class DirectionalLight
{
public:
	DirectionalLight(Shader* lightingShader, const glm::vec3& dir);
	~DirectionalLight();
	bool showShadowArea();

	//Getter Methods
	//glm::vec3 getLightPos() const;
	//glm::vec3 getLightDirection() const;
	glm::vec3 getAmbient() const;
	glm::vec3 getDiffuse() const;
	glm::vec3 getSpecular() const;
	float getIntensity() const;

	//Setter Methods
	void setAmbient(glm::vec3 ambient);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);
	void setShadowHeight(int);
	void setShadowWidth(int);
	void setIntensity(const float i);
	void setNearPlane(const float i);
	void setFarPlane(const float i);
	void setYaw(const float i);
	void setPitch(const float i);
	void setRadius(const float i);

	glm::vec3 m_LightDirection;
	glm::vec3 m_LightPosition;
	std::string m_DisplayName;

	//Shadow Map Methods
	GLuint& getDepthMapTexture();
	int getShadowHeight() const;
	int getShadowWidth() const;
	float getFarPlane() const;
	float getNearPlane() const;
	float getPitch() const;
	float getYaw() const;
	float getRadius() const;
	bool getShadowArea() const;

private:
	Shader* m_LightingShader;
	GLuint m_DepthMapTexture;

	int m_ShadowHeight;
	int m_ShadowWidth;

	float m_NearPlane;
	float m_FarPlane;
	float m_Yaw;
	float m_Pitch;
	float m_Radius;

	float m_Intensity;

	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;

	bool m_ShowShadowArea;

};