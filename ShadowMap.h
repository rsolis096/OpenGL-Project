#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Object.h"
#include "LightController.h"
#include <glm/gtx/string_cast.hpp>

class ShadowMap
{
	private:

		std::vector<Object*>* m_SceneObjects;
		std::vector<SpotLight*>* m_SpotLights;
		std::vector<GLuint> depthMapFBO;
		std::vector<GLuint> depthMap;
		const unsigned int SHADOW_WIDTH = 2048;
		const unsigned int SHADOW_HEIGHT = 2048;
		std::vector<glm::mat4> m_LightSpaceMatrices;
		unsigned int m_ShadowCasters;

	public:
		Shader depthShader;
		Shader debugDepthShader;
		ShadowMap(std::vector<Object*>*, std::vector<SpotLight*>*);
		void ShadowPass();
		void addShadowMap();
		glm::mat4& getLightSpaceMatrix(int);
		std::vector<glm::mat4>& getLightSpaceMatrices();
		void drawShadowMap(GLuint&);
		
		GLuint getDepthMapID(int);
		GLuint getDepthMapFBOID(int);
		
};