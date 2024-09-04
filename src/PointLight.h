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
		void setConstant(const float constant);
		void setLinear(const float linear);
		void setQuadratic(const float quadratic);
		void setNearPlane(const float& n);
		void setFarPlane(const float& f);
		void setLightViews(const std::array<glm::mat4, 6>& lv);
		void setShadowFOV(const float& fov);
		void setIntensity(const float& i);
		void setShadowPassUpdate(bool val);
		void setBias(const float& b);

		glm::vec3 getLightPos() const;
		glm::vec3 getAmbient() const;
		glm::vec3 getDiffuse() const;
		glm::vec3 getSpecular() const;
		GLuint& getCubeMapTexture();
		float getFarPlane() const;
		float getNearPlane() const;
		const glm::mat4& getShadowProj() const;
		const std::array<glm::mat4, 6>& getLightViews() const;
		float getShadowFOV() const;
		float getIntensity() const;
		bool getShadowPassUpdate() const;
		float getBias() const;

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
		float m_Intensity;

		//For Attenuation
		float m_Constant;
		float m_Linear;
		float m_Quadratic;

		//For Shadows
		float m_NearPlane;
		float m_FarPlane;
		float m_ShadowFOV;
		GLuint m_CubeMapTexture;
		glm::mat4 m_ShadowProj;
		std::array<glm::mat4, 6> m_LightViews;
		bool m_ShadowPassUpdate;
		float m_Bias;


};