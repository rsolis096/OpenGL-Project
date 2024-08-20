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



		LightController* m_LightController;

		//Spot Lights
		std::vector<GLuint> depthMapFBOSpotLights;
		std::vector<GLuint> depthMapSpotLights;





		const unsigned short SHADOW_WIDTH = 2048;
		const unsigned short SHADOW_HEIGHT = 2048;
		std::vector<glm::mat4> m_LightSpaceMatrices;
		bool m_UpdateShadowMap;

	public:

		//Point Lights
		std::vector<GLuint> depthMapFBOPointLights;
		std::vector<GLuint> depthCubeMaps;

		Shader depthShader;
		Shader debugDepthShader;
		ShadowMap(std::vector<Object*>*, LightController*, std::vector<SpotLight*>*);
		void ShadowPass();
		void addShadowMap();
		void addCubeMap();
		glm::mat4& getLightSpaceMatrix(int);
		std::vector<glm::mat4>& getLightSpaceMatrices();
		void updateShaderUniforms(Shader&) const;
		
		GLuint getDepthMapID(int);
		GLuint getDepthMapFBOID(int);
		void setUpdateShadowMap();
		void debugShadowMap();
		
};