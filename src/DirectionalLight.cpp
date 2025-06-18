#include "DirectionalLight.h"

bool firstFrame1 = true;

DirectionalLight::DirectionalLight(Shader* lightingShader, const glm::vec3& dir)
: m_LightingShader(lightingShader)
{
	//Used for rendering but not explicitly needed, just a GLM technicality
	m_LightPosition = glm::vec3( 20.0f, 20.0f, 0.0f);

	// yaw and pitch for light direction
	m_Yaw = 0.f;
	m_Pitch = 30.f;
	float yaw_rads = glm::radians(m_Yaw);
	float pitch_rads = glm::radians(m_Pitch);
	m_LightDirection.x = cos(pitch_rads) * cos(yaw_rads);
	m_LightDirection.y = sin(pitch_rads);
	m_LightDirection.z = cos(pitch_rads) * sin(yaw_rads);
	m_LightDirection = glm::normalize(m_LightDirection);

	m_DepthMapTexture = 0;
	m_DisplayName = "Directional Light";

	//Light color properties
	m_Ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	m_Diffuse = glm::vec3(0.9f, 0.9f, 0.9f);
	m_Specular = glm::vec3(0.7f, 0.7f, 0.7f);
	m_Intensity = 1.0f;
	m_NearPlane = 0.1f;
	m_FarPlane = 50.0;
	m_Radius = 10.0f;

	// Resolution high to better encapsulate the scene
	m_ShadowHeight = 1024;
	m_ShadowWidth = 1024;
	m_ShowShadowArea = true; // Darken or light the areas outside shadow frustrum

	m_LightingShader->use();
	m_LightingShader->setVec3("dirLight.position", m_LightPosition);
	m_LightingShader->setVec3("dirLight.direction", m_LightDirection);
	m_LightingShader->setVec3("dirLight.ambient", m_Ambient);
	m_LightingShader->setVec3("dirLight.diffuse", m_Diffuse);
	m_LightingShader->setVec3("dirLight.specular", m_Specular);
	m_LightingShader->setBool("hasDirLight", true);
	m_LightingShader->setBool("dirLight.showShadowArea", m_ShowShadowArea);

	glCheckError();
}

DirectionalLight::~DirectionalLight()
{
	m_LightingShader->use();
	m_LightingShader->setBool("hasDirLight", true);

	m_LightingShader = nullptr;

	// Delete the depth map texture
	if (m_DepthMapTexture != 0) {
		glDeleteTextures(1, &m_DepthMapTexture);
	}
}

/*###################################
* ###     SETTER FUNCTIONS        ###
###################################*/

void DirectionalLight::setAmbient(glm::vec3 ambient)
{
	m_Ambient = ambient;
	m_LightingShader->use();
	m_LightingShader->setVec3("dirLight.ambient", m_Ambient);

}

void DirectionalLight::setDiffuse(glm::vec3 diffuse)
{
	m_Diffuse = diffuse;
	m_LightingShader->use();
	m_LightingShader->setVec3("dirLight.diffuse", m_Diffuse);
}

void DirectionalLight::setSpecular(glm::vec3 specular)
{
	m_Specular = specular;
	m_LightingShader->use();
	m_LightingShader->setVec3("dirLight.specular", m_Specular);
}

void DirectionalLight::setShadowHeight(int h) 
{
	m_ShadowHeight = h;
}

void DirectionalLight::setShadowWidth(int w) 
{
	m_ShadowWidth = w;
}

void DirectionalLight::setIntensity(const float i)
{
	m_Intensity = i;
}

void DirectionalLight::setNearPlane(const float i)
{
	m_NearPlane = i;
}

void DirectionalLight::setFarPlane(const float i)
{
	m_FarPlane = i;
}

void DirectionalLight::setRadius(const float i)
{
	m_Radius = i;
}

void DirectionalLight::setYaw(const float i)
{
	m_Yaw = i;
	float yaw_rads = glm::radians(m_Yaw);
	float pitch_rads = glm::radians(m_Pitch);
	m_LightDirection.x = cos(pitch_rads) * cos(yaw_rads);
	m_LightDirection.y = sin(pitch_rads);
	m_LightDirection.z = cos(pitch_rads) * sin(yaw_rads);
	m_LightDirection = glm::normalize(m_LightDirection);

}

void DirectionalLight::setPitch(const float i)
{
	m_Pitch = i;
	float yaw_rads = glm::radians(m_Yaw);
	float pitch_rads = glm::radians(m_Pitch);
	m_LightDirection.x = cos(pitch_rads) * cos(yaw_rads);
	m_LightDirection.y = sin(pitch_rads);
	m_LightDirection.z = cos(pitch_rads) * sin(yaw_rads);
	m_LightDirection = glm::normalize(m_LightDirection);
}


/*###################################
* ###     GETTER FUNCTIONS        ###
###################################*/

int DirectionalLight::getShadowHeight() const
{
	return m_ShadowHeight;
}

int DirectionalLight::getShadowWidth() const
{
	return m_ShadowWidth;
}

bool DirectionalLight::getShadowArea() const
{
	return m_ShowShadowArea;
}

bool DirectionalLight::showShadowArea()
{
	m_ShowShadowArea = !m_ShowShadowArea;

	m_LightingShader->use();
	m_LightingShader->setBool("dirLight.showShadowArea", m_ShowShadowArea);

	return m_ShowShadowArea;
}

GLuint& DirectionalLight::getDepthMapTexture()
{
	return m_DepthMapTexture;
}

glm::vec3 DirectionalLight::getAmbient() const
{
	return m_Ambient;
}

glm::vec3 DirectionalLight::getDiffuse() const
{
	return m_Diffuse;
}

glm::vec3 DirectionalLight::getSpecular() const
{
	return m_Specular;
}

float DirectionalLight::getIntensity() const
{
	return m_Intensity;
}

float DirectionalLight::getNearPlane() const
{
	return m_NearPlane;
}

float DirectionalLight::getPitch() const
{
	return m_Pitch;
}

float DirectionalLight::getYaw() const
{
	return m_Yaw;
}

float DirectionalLight::getFarPlane() const
{
	return m_FarPlane;
}

float DirectionalLight::getRadius() const
{
	return m_Radius;
}