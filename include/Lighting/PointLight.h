#pragma once

#include "Lighting/Light.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>
#include <string>

class Cube;
class Shader;

class PointLight : public Light
{
	public:

		//Constructors
		PointLight(Shader* lightingShader, Shader* lightCubeShader, const glm::vec3&);
		~PointLight() override;

		//Render Functions
		void Draw();
		void updateLightSpaceMatrices();

		//For Light Properties
		void setLightPos(const glm::vec3 lightPos);
		void setConstant(const float constant);
		void setLinear(const float linear);
		void setQuadratic(const float quadratic);
		void setNearPlane(const float& n);
		void setFarPlane(const float& f);
		void setLightViews(const std::array<glm::mat4, 6>& lv);
		void setShadowFOV(const float& fov);
		void setShadowPassUpdate(bool val);
		void setShadowBias(const float& b);

		glm::vec3 getLightPos() const;
		GLuint& getCubeMapTexture();
		float getFarPlane() const;
		float getNearPlane() const;
		const glm::mat4& getShadowProj() const;
		const std::array<glm::mat4, 6>& getLightViews() const;
		float getShadowFOV() const;
		bool getShadowPassUpdate() const;
		float getShadowBias() const;

		//Variables
		static unsigned short m_PointLightCount;
		unsigned short m_LightID;

	private:

		//Shaders (for lighting and for light object itself
		Shader* m_LightSourceShader;
		//Light Object (light is not invisible)
		Cube* m_LightShape;

		//Light properties
		glm::vec3 m_LightPos;
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
		float m_ShadowBias;

		void updateCommonShaderUniforms() override;


};
