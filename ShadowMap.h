#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Object.h"

class ShadowMap
{
	private:
		Shader depthShader;
		Shader debugDepthShader;
		std::vector<Object*>* m_SceneObjects;
		GLuint depthMapFBO;
		GLuint depthMap;
		const unsigned int SHADOW_WIDTH = 2048;
		const unsigned int SHADOW_HEIGHT = 2048;
		float m_NearPlane;
		float m_FarPlane;
		glm::vec3 m_LightPos;
		glm::vec3 m_LightDir;
		glm::mat4 m_LightSpaceMatrix;
	public:
		ShadowMap(std::vector<Object*>*, glm::vec3, glm::vec3);
		void ShadowPass();
		GLuint getFBO();
		GLuint getDepthMapID();
		glm::mat4& getLightSpaceMatrx();
		float getNearPlane();
		void setNearPlane(float&);
		float getFarPlane();
		void setFarPlane(float&);
};