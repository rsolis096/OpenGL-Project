#include "SpotLight.h"

#include <glm/gtx/string_cast.hpp>

bool firstFrame2 = true;
unsigned short SpotLight::m_SpotLightCount = 0;

SpotLight::SpotLight(Shader* lightingShader, Shader* lightSourceShader, glm::vec3 pos, glm::vec3 dir) :
	m_LightingShader(lightingShader), m_LightSourceShader(lightSourceShader),
	m_LightPos(pos), m_NearPlane(0.5f), m_FarPlane(70.0f)
{
	m_SpotLightID = m_SpotLightCount;
	m_DisplayName = "SpotLight" + std::to_string(m_SpotLightID);
	m_SpotLightCount+= 1;


	m_Yaw = 0.f;
	m_Pitch = 30.f;
	float yaw_rads = glm::radians(m_Yaw);
	float pitch_rads = glm::radians(m_Pitch);
	m_LightDirection.x = cos(pitch_rads) * cos(yaw_rads);
	m_LightDirection.y = sin(pitch_rads);
	m_LightDirection.z = cos(pitch_rads) * sin(yaw_rads);
	m_LightDirection = glm::normalize(m_LightDirection);


	//m_SpotLight takes the shape of a sphere
	m_LightShape = new Sphere();
	m_LightShape->setPosition(pos);
	m_LightShape->setScale(glm::vec3(0.2f));

	//Light color properties
	m_Intensity = 1.0f;
	m_Ambient = glm::vec3(1.0f, 1.0f, 1.0f) ;
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f) ;
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f) ;

	//Shadow Properties
	m_ShadowFOV = 45.0f;
	m_ShadowHeight = 1024;
	m_ShadowWidth = 1024;
	m_ShadowBias = 0.005f;
	m_ShadowPassUpdate = true;
	updateLightSpaceMatrix();

	//For Attenuation
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;
	m_DepthMapTexture = 0;

	//TODO: Remove dependence on SpotLight Indexing to allow for dynamic removal/addition to spotlights
	m_LightingShader->use();
	m_LightingShader->setInt("numberOfSpotLightsFRAG", m_SpotLightCount);
	m_LightingShader->setInt("numberOfSpotLightsVERT", m_SpotLightCount);
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].position", m_LightPos);
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].direction", m_LightDirection);
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].ambient", m_Ambient);
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].diffuse", m_Diffuse);
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].specular", m_Specular);
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].far_plane", m_FarPlane);
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].constant", m_Constant);
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].linear", m_Linear);
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].quadratic", m_Quadratic);
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].cutOff", glm::cos(glm::radians(12.5f)));
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].outerCutOff", glm::cos(glm::radians(15.0f)));
	m_LightingShader->setMat4("spotLights[" + std::to_string(m_SpotLightID) + "].lightSpaceMatrix", m_LightSpaceMatrix);
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].bias", m_ShadowBias);  

}

SpotLight::~SpotLight()
{
	//TODO: Implement Rule of 3 and 5 if you want
	m_LightingShader = nullptr;
	m_LightSourceShader = nullptr;
	delete m_LightShape;
	m_LightShape = nullptr;
}

void SpotLight::Draw() const
{
	m_LightingShader->use();
	m_LightingShader->setInt("lightType", 1);
	if(m_LightShape != nullptr)
		m_LightShape->Draw(*m_LightSourceShader);
}

void SpotLight::updateLightSpaceMatrix()
{

	m_LightProjectionMatrix = glm::perspective(
		glm::radians(m_ShadowFOV),
		1.0f, //static_cast<float>(SHADOW_WIDTH / SHADOW_HEIGHT)
		m_NearPlane,
		m_FarPlane
	);
	m_LightViewMatrix = glm::lookAt(
		m_LightPos,      // Position of the light
		m_LightPos + m_LightDirection,        // The target point (position + direction)
		glm::vec3(0.0, 1.0, 0.0)  // Up vector
	);
	m_LightSpaceMatrix = m_LightProjectionMatrix * m_LightViewMatrix;

	//Shader uniform must be updated separately. (used in both shadow pass shader and lighting shader)
}

/*###################################
* ###     SETTER FUNCTIONS        ###
###################################*/

void SpotLight::setAmbient(const glm::vec3& ambient)
{
	m_Ambient = ambient * m_Intensity;
	m_LightShape->setAmbient(m_Ambient);
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].ambient", m_Ambient);

}

void SpotLight::setDiffuse(const glm::vec3& diffuse)
{
	m_Diffuse = diffuse * m_Intensity;
	m_LightShape->setDiffuse(m_Diffuse);
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].diffuse", m_Diffuse);
}

void SpotLight::setSpecular(const glm::vec3& specular)
{
	m_Specular = specular * m_Intensity;
	m_LightShape->setSpecular(m_Specular);
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].specular", m_Specular);
}

void SpotLight::setLightPos(const glm::vec3& lightPos)
{
	m_LightPos = lightPos;
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].position", m_LightPos);
	if (m_LightShape != nullptr)
		m_LightShape->setPosition(lightPos);
	updateLightSpaceMatrix();
}

void SpotLight::setFarPlane(const float& val)
{
	m_FarPlane = val;
	updateLightSpaceMatrix();
}

void SpotLight::setNearPlane(const float& val)
{
	m_NearPlane = val;
	updateLightSpaceMatrix();
}

void SpotLight::setConstant(const float constant)
{
	m_Constant = constant;
	m_LightingShader->use();
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].constant", m_Constant);
}

void SpotLight::setLinear(const float linear)
{
	m_Linear = linear;
	m_LightingShader->use();
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].linear", m_Linear);
}

void SpotLight::setQuadratic(const float quadratic)
{
	m_Quadratic = quadratic;
	m_LightingShader->use();
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].quadratic", m_Quadratic);
}

void SpotLight::setIntensity(const float i)
{
	m_Intensity = i;
}

void SpotLight::setShadowHeight(int h)
{
	m_ShadowHeight = h;
}

void SpotLight::setShadowWidth(int w)
{
	m_ShadowWidth = w;
}

void SpotLight::setLightSpaceMatrix(const glm::mat4& ls)
{
	m_LightSpaceMatrix = ls;
}

void SpotLight::setShadowBias(const float& b)
{
	m_ShadowBias = b;
	m_LightingShader->use();
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].bias", m_ShadowBias);
}

void SpotLight::setShadowPassUpdate(bool val)
{
	m_ShadowPassUpdate = val;
	updateLightSpaceMatrix();
}

void SpotLight::setShadowFOV(const float& fov)
{
	m_ShadowFOV = fov;
	updateLightSpaceMatrix();
}

void SpotLight::setYaw(float i)
{
	m_Yaw = i;
	float yaw_rads = glm::radians(m_Yaw);
	float pitch_rads = glm::radians(m_Pitch);
	m_LightDirection.x = cos(pitch_rads) * cos(yaw_rads);
	m_LightDirection.y = sin(pitch_rads);
	m_LightDirection.z = cos(pitch_rads) * sin(yaw_rads);
	m_LightDirection = glm::normalize(m_LightDirection);
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].direction", m_LightDirection);
	updateLightSpaceMatrix();
}

void SpotLight::setPitch(float i)
{
	m_Pitch = i;
	float yaw_rads = glm::radians(m_Yaw);
	float pitch_rads = glm::radians(m_Pitch);
	m_LightDirection.x = cos(pitch_rads) * cos(yaw_rads);
	m_LightDirection.y = sin(pitch_rads);
	m_LightDirection.z = cos(pitch_rads) * sin(yaw_rads);
	m_LightDirection = glm::normalize(m_LightDirection);
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].direction", m_LightDirection);
	updateLightSpaceMatrix();
}


/*###################################
* ###     GETTER FUNCTIONS        ###
###################################*/

glm::vec3 SpotLight::getLightPos() const
{
	return m_LightPos;
}

glm::vec3 SpotLight::getLightDirection() const
{
	return 	m_LightDirection;
}

glm::vec3 SpotLight::getAmbient() const
{
	return m_Ambient;
}

glm::vec3 SpotLight::getDiffuse() const
{
	return m_Diffuse;
}

glm::vec3 SpotLight::getSpecular() const
{
	return m_Specular;
}

float SpotLight::getIntensity() const
{
	return m_Intensity;
}

float SpotLight::getNearPlane() const
{
	return m_NearPlane;
}

float SpotLight::getFarPlane() const
{
	return m_FarPlane;
}

GLuint& SpotLight::getDepthMapTexture()
{
	return m_DepthMapTexture;
}

int SpotLight::getShadowHeight() const
{
	return m_ShadowHeight;
}

int SpotLight::getShadowWidth() const
{
	return m_ShadowWidth;
}

glm::mat4& SpotLight::getLightSpaceMatrix()
{
	return m_LightSpaceMatrix;
}

float SpotLight::getShadowBias() const
{
	return m_ShadowBias;
}

float SpotLight::getShadowFOV() const
{
	return m_ShadowFOV;
}

float SpotLight::getYaw() const
{
	return m_Yaw;
}

float SpotLight::getPitch() const
{
	return m_Pitch;
}


bool SpotLight::getShadowPassUpdate() const
{
	return m_ShadowPassUpdate;
}
