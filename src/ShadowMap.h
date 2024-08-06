#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Object.h"
#include "Model.h"
#include "LightController.h"

#include <glm/gtx/string_cast.hpp>

class ShadowMap
{
	private:

		std::vector<Object*>* m_SceneObjects;
		std::vector<SpotLight*>* m_SpotLights;
		std::vector<GLuint> depthMapFBO;
		std::vector<GLuint> depthMap;
		const unsigned short SHADOW_WIDTH = 2048;
		const unsigned short SHADOW_HEIGHT = 2048;
		std::vector<glm::mat4> m_LightSpaceMatrices;
		unsigned short m_ShadowCasters;
		bool m_UpdateShadowMap;

	public:
		Shader depthShader;
		Shader debugDepthShader;
		ShadowMap(std::vector<Object*>*, std::vector<SpotLight*>*);
		void ShadowPass();
		void addShadowMap();
		glm::mat4& getLightSpaceMatrix(int);
		std::vector<glm::mat4>& getLightSpaceMatrices();
		void drawShadowMap(GLuint) const;
		
		GLuint getDepthMapID(int);
		GLuint getDepthMapFBOID(int);
		void setUpdateShadowMap();
		void debugShadowMap();
		
};