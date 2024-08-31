#include "PointLight.h"

#include <glm/gtx/string_cast.hpp>

unsigned short PointLight::m_PointLightCount = 0;
bool firstFrame = true;

//Create Point Light with positon (default = 0,10,0)
PointLight::PointLight(Shader* lightingShader, Shader* objectShader, const glm::vec3& pos) :
	m_LightingShader(lightingShader), m_LightSourceShader(objectShader)
{
	m_LightShape = new Cube(); //Create the physical light object
	setLightPos(pos); //Need to update light object too
	m_LightShape->setScale(glm::vec3(0.1f, 0.1f, 0.1f));

	m_CubeMapTexture = 0;
	m_LightID = m_PointLightCount;
	m_DisplayName = "PointLight" + std::to_string(m_LightID);
	m_PointLightCount++;
	m_LightShape->m_DisplayName = "PointLight" + std::to_string(m_LightID);

	//Light Color Properties (How it casts light on objects)
	m_Ambient = glm::vec3(0.05f, 0.05f, 0.05f); //Dark ambient
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f); //Grey light color
	m_Specular = glm::vec3(0.7f, 0.7f, 0.7f); //"Shine" color
	m_Intensity = 1.0f;

	//Set the color for the light object
	m_LightShape->setDiffuse(m_Diffuse);
	m_LightShape->setAmbient(m_Ambient);
	m_LightShape->setSpecular(m_Specular);

	//For attenuation
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;

	//For Shadows
	m_NearPlane = 5.4f;
	m_FarPlane = 100.0f;
	m_ShadowFOV = 85.0f;
	m_ShadowPassUpdate = true;

	m_ShadowProj = glm::perspective(
		glm::radians(m_ShadowFOV),
		1.0f, // static_cast<float>(SHADOW_WIDTH / SHADOW_HEIGHT) 
		m_NearPlane,
		m_FarPlane);

	m_LightViews = {
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};

	//Set these properties in the lightingShader.frag for the corresponding light source at index m_LightID
	m_LightingShader->use();
	m_LightingShader->setInt("numberOfPointLightsFRAG", m_PointLightCount);
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].ambient", m_Ambient);
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].diffuse", m_Diffuse);
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].specular", m_Specular);
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].constant", m_Constant);
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].linear", m_Linear);
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].quadratic", m_Quadratic);
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].position", m_LightPos);
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].far_plane", m_FarPlane);

}

PointLight::~PointLight()
{
	//TODO: Implement Rule of 3 and 5 if you want
	m_LightingShader = nullptr;
	m_LightSourceShader = nullptr;
	delete m_LightShape;
	m_LightShape = nullptr;
}

void PointLight::Draw()
{
	//Render the light Object
	m_LightingShader->use();
	m_LightingShader->setInt("lightType", 0);
	m_LightShape->Draw(*m_LightSourceShader);
}

/*###################################
* ###     SETTER FUNCTIONS        ###
###################################*/

void PointLight::setLightPos(const glm::vec3 lightPos)
{
	m_LightPos = lightPos;
	m_LightShape->setPosition(lightPos);
	m_LightingShader->use();
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].position", m_LightPos);

	m_LightViews = {
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};

	glCheckError();
}

void PointLight::setAmbient(const glm::vec3 ambient)
{
	m_Ambient = ambient * m_Intensity;
	m_LightShape->setAmbient(ambient);
	m_LightingShader->use();
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].ambient", m_Ambient);
}

void PointLight::setDiffuse(const glm::vec3 diffuse)
{
	m_Diffuse = diffuse * m_Intensity;
	m_LightShape->setDiffuse(diffuse);
	m_LightingShader->use();
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].diffuse", m_Diffuse);
}

void PointLight::setSpecular(const glm::vec3 specular)
{
	m_Specular = specular * m_Intensity;
	m_LightShape->setSpecular(specular);
	m_LightingShader->use();
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].specular", m_Specular);
}

void PointLight::setNearPlane(const float& n)
{
	m_NearPlane = n;

	m_ShadowProj = glm::perspective(
		glm::radians(m_ShadowFOV),
		1.0f, // static_cast<float>(SHADOW_WIDTH / SHADOW_HEIGHT) 
		m_NearPlane,
		m_FarPlane);

	m_LightViews = {
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};
}

void PointLight::setFarPlane(const float& f)
{
	m_FarPlane = f;

	m_ShadowProj = glm::perspective(
		glm::radians(m_ShadowFOV),
		1.0f, // static_cast<float>(SHADOW_WIDTH / SHADOW_HEIGHT) 
		m_NearPlane,
		m_FarPlane);

	m_LightViews = {
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};

	m_LightingShader->use();
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].far_plane", m_FarPlane);

}

void PointLight::setLightViews(const std::array<glm::mat4, 6>& lv)
{
	m_LightViews = lv;
}

void PointLight::setShadowFOV(const float& fov)
{
	m_ShadowFOV = fov;

	m_ShadowProj = glm::perspective(
		glm::radians(m_ShadowFOV),
		1.0f, // static_cast<float>(SHADOW_WIDTH / SHADOW_HEIGHT) 
		m_NearPlane,
		m_FarPlane);

	m_LightViews = {
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};
}

void PointLight::setIntensity(const float& i)
{
	m_Intensity = i;
}

void PointLight::setShadowPassUpdate(bool val)
{
	m_ShadowPassUpdate = val;

	m_ShadowProj = glm::perspective(
		glm::radians(m_ShadowFOV),
		1.0f, // static_cast<float>(SHADOW_WIDTH / SHADOW_HEIGHT) 
		m_NearPlane,
		m_FarPlane);

	m_LightViews = {
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		m_ShadowProj * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};
}

void PointLight::setConstant(const float constant)
{
	m_Constant = constant;
	m_LightingShader->use();
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].constant", m_Constant);
}

void PointLight::setLinear(const float linear)
{
	m_Linear = linear;
	m_LightingShader->use();
	m_LightingShader->setFloat("pointLight[" + std::to_string(m_LightID) + "].linear", m_Linear);
}

void PointLight::setQuadratic(const float quadratic)
{
	m_Quadratic = quadratic;
	m_LightingShader->use();
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].quadratic", m_Quadratic);
}

/*###################################
* ###     GETTER FUNCTIONS        ###
###################################*/

glm::vec3 PointLight::getLightPos() const
{
	return m_LightPos;
}

glm::vec3 PointLight::getAmbient() const
{
	return m_Ambient;
}

glm::vec3 PointLight::getDiffuse() const
{
	return m_Diffuse;
}

glm::vec3 PointLight::getSpecular() const
{
	return m_Specular;
}

GLuint& PointLight::getCubeMapTexture() 
{
	return m_CubeMapTexture;
}

float PointLight::getFarPlane() const
{
	return m_FarPlane;
}

float PointLight::getNearPlane() const
{
	return m_NearPlane;
}

const glm::mat4& PointLight::getShadowProj() const
{
	return m_ShadowProj;
}

const std::array<glm::mat4, 6>& PointLight::getLightViews() const
{
	return m_LightViews;
}

float PointLight::getShadowFOV() const
{
	return m_ShadowFOV;
}

float PointLight::getIntensity() const
{
	return m_Intensity;
}

bool PointLight::getShadowPassUpdate() const
{
	return m_ShadowPassUpdate;
}
