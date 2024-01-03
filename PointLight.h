#pragma once

#include <iostream>
#include <vector>
#include "Model.h"
#include "Cube.h"
#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class PointLight
{
	public:
		PointLight(Shader* lightingShader, Shader* lightCubeShader, Camera& cam);
		void Draw();
		~PointLight();
		static int pointLightCount;
		int lightID;

		//For Light Properties
		void setLightPos(glm::vec3 lightPos);
		void setAmbient(glm::vec3 ambient);
		void setDiffuse(glm::vec3 diffuse);
		void setSpecular(glm::vec3 specular);

		//For Attenuation
		void setconstant(float constant);
		void setLinear(float linear);
		void setQuadratic(float quadratic);

	private:
		//Shaders (for lighting and for light object itself
		Shader* m_ObjectShader;
		Shader* m_lightingShader;

		//Light Object (light is not invisible)
		Cube* m_LightShape;
		//Model m_LightModel;

		//Reference to the camera
		Camera& playerCamera;

		//World Position
		glm::vec3 m_LightPos;

		//Light properties
		glm::vec3 m_Ambient;
		glm::vec3 m_Diffuse;
		glm::vec3 m_Specular;

		//For Attenuation
		float m_Constant;
		float m_Linear;
		float m_Quadratic;
};

