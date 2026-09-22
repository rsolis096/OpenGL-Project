#pragma once

#include "Lighting/Light.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader;

class DirectionalLight : public Light
{
public:
	DirectionalLight(Shader* lightingShader, const glm::vec3& dir);
	~DirectionalLight() override;
	bool showShadowArea();

	//Getter Methods
	//glm::vec3 getLightPos() const;
	//glm::vec3 getLightDirection() const;
	//Setter Methods
	void setShadowHeight(int);
	void setShadowWidth(int);
	void setNearPlane(const float i);
	void setFarPlane(const float i);
	void setYaw(const float i);
	void setPitch(const float i);
	void setRadius(const float i);

	glm::vec3 m_LightDirection;
	glm::vec3 m_LightPosition;

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
	GLuint m_DepthMapTexture;

	int m_ShadowHeight;
	int m_ShadowWidth;

	float m_NearPlane;
	float m_FarPlane;
	float m_Yaw;
	float m_Pitch;
	float m_Radius;

	bool m_ShowShadowArea;

	void updateCommonShaderUniforms() override;

};
