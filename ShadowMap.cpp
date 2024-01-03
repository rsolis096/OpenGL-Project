#include "ShadowMap.h"

ShadowMap::ShadowMap(std::vector<Object*>* objects, glm::vec3 lP, glm::vec3 lD):
    m_SceneObjects(objects), m_LightPos(lP), m_LightDir(lD),
    m_NearPlane(0.5f), m_FarPlane(70.0f), m_LightSpaceMatrix(glm::mat4(1.0f))   
{
	depthShader = Shader("shadow_mapping_depth.vert", "shadow_mapping_depth.frag");
    debugDepthShader = Shader("debug_quad.vert", "debug_quad_depth.frag");

    glGenFramebuffers(1, &depthMapFBO);

    // create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Debug Quad
    debugDepthShader.use();
    debugDepthShader.setInt("depthMap", 0);
}


void ShadowMap::ShadowPass() 
{
    //Shadow Pass
    // 1. render depth of scene to texture (from light's perspective)
    //glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane); //For Directional Lighting (like the sun)
    glm::mat4 lightProjection = glm::perspective<float>(glm::radians(45.0f), (float)SHADOW_WIDTH / SHADOW_HEIGHT, m_NearPlane, m_FarPlane);
    glm::mat4 lightView = glm::lookAt(m_LightPos, glm::normalize(m_LightDir), glm::vec3(0.0, 1.0, 0.0));
    m_LightSpaceMatrix = lightProjection * lightView;
    depthShader.use();
    depthShader.setMat4("lightSpaceMatrix", m_LightSpaceMatrix);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    (*m_SceneObjects)[0]->Draw(depthShader);
    (*m_SceneObjects)[1]->Draw(depthShader);
    (*m_SceneObjects)[2]->Draw(depthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCheckError();
}

GLuint ShadowMap::getFBO()
{
    return depthMapFBO;
}

GLuint ShadowMap::getDepthMapID()
{
    return depthMap;
}

glm::mat4& ShadowMap::getLightSpaceMatrx()
{
    return m_LightSpaceMatrix;
}

float ShadowMap::getNearPlane()
{
    return m_NearPlane;
}

void ShadowMap::setNearPlane(float& val)
{
    m_NearPlane = val;
}

float ShadowMap::getFarPlane()
{
    return m_FarPlane;
}

void ShadowMap::setFarPlane(float& val)
{
    m_FarPlane = val;
}
