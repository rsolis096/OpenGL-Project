#pragma once

#include "Cube.h"
#include "Shader.h"


class PointLight
{
	public:

		//Constructors
		PointLight(Shader* lightingShader, Shader* lightCubeShader, const glm::vec3&);
		~PointLight();

		//Render Functions
		void Draw();

		//For Light Properties
		void setLightPos(const glm::vec3 lightPos);
		void setAmbient(const glm::vec3 ambient);
		void setDiffuse(const glm::vec3 diffuse);
		void setSpecular(const glm::vec3 specular);

		glm::vec3 getLightPos();
		glm::vec3 getAmbient();
		glm::vec3 getDiffuse();
		glm::vec3 getSpecular();
		GLuint& getCubeMapTexture();

		//For Attenuation
		void setconstant(const float constant);
		void setLinear(const float linear);
		void setQuadratic(const float quadratic);

		//Variables
		std::string m_DisplayName;
		static unsigned short m_PointLightCount;
		unsigned short m_LightID;

	private:

		//Shaders (for lighting and for light object itself
		Shader* m_LightSourceShader;
		Shader* m_LightingShader;

		//Light Object (light is not invisible)
		Cube* m_LightShape;

		//Light properties
		glm::vec3 m_LightPos;
		glm::vec3 m_Ambient;
		glm::vec3 m_Diffuse;
		glm::vec3 m_Specular;

		//For Attenuation
		float m_Constant;
		float m_Linear;
		float m_Quadratic;

		GLuint m_CubeMapTexture;
};