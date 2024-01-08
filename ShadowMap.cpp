#include "ShadowMap.h"

ShadowMap::ShadowMap(std::vector<Object*>* objects, std::vector<SpotLight*>* spotlights):
    m_SceneObjects(objects), m_SpotLights(spotlights)  
{
    // Initialize shaders
	depthShader = Shader("shadow_mapping_depth.vert", "shadow_mapping_depth.frag");
    debugDepthShader = Shader("debug_quad.vert", "debug_quad_depth.frag");
    m_ShadowCasters = 0;
}

void ShadowMap::addShadowMap()
{
    depthMapFBO.push_back(0);
    depthMap.push_back(0);

    // Generate framebuffer and depth map texture
    glGenFramebuffers(1, &depthMapFBO[m_ShadowCasters]);
    glGenTextures(1, &depthMap[m_ShadowCasters]);

    // Configure depth map texture
    glBindTexture(GL_TEXTURE_2D, depthMap[m_ShadowCasters]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[m_ShadowCasters]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[m_ShadowCasters], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_ShadowCasters++;
}



void ShadowMap::ShadowPass() 
{
    //Shadow Pass
    // 1. render depth of scene to texture (from light's perspective)
    m_LightSpaceMatrices.clear();
    m_LightSpaceMatrices.resize(m_SpotLights->size(), glm::mat4(1.0f));

    //Generate the light space matrices
    for (int i = 0; i < (*m_SpotLights).size(); i++)
    {
        // Calculate light projection matrix
        glm::mat4 lightProjection = glm::perspective<float>(
            glm::radians(45.0f), 
            (float)SHADOW_WIDTH / SHADOW_HEIGHT, 
            (*m_SpotLights)[i]->getNearPlane(), 
            (*m_SpotLights)[i]->getFarPlane()
        );

        // Calculate light view matrix
        glm::mat4 lightView = glm::lookAt(
            (*m_SpotLights)[i]->getLightPos(),
            (*m_SpotLights)[i]->getLightDirection(),
            glm::vec3(0.0, 1.0, 0.0)
        );

        // Combine projection and view matrices to get the light-space matrix
        m_LightSpaceMatrices[i] = lightProjection * lightView;
        glCheckError();
    }  


    // Loop through each depth/shadow map FBO
    for (int i = 0; i < (*m_SpotLights).size(); i++)
    {

        // Use the depth shader for rendering
        depthShader.use();
        // Pass the light-space matrices to the shader
        glUniformMatrix4fv(
            glGetUniformLocation(depthShader.ID, "lightSpaceMatrix"),
            1, // Pass 1 matrix
            GL_FALSE,
            glm::value_ptr(getLightSpaceMatrices()[i])
        );

        // Set the viewport and bind the framebuffer
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);

        // Clear the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw each object in the scene using the depth shader
        for (Object* obj : *m_SceneObjects)
            obj->Draw(depthShader);

        // Unbind the framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Check for OpenGL errors
        glCheckError();
    }
}

glm::mat4& ShadowMap::getLightSpaceMatrix(int i)
{
    return m_LightSpaceMatrices[i];
}

std::vector<glm::mat4>& ShadowMap::getLightSpaceMatrices()
{
    return m_LightSpaceMatrices;
}

void ShadowMap::drawShadowMap(GLuint& shaderID)
{
    // Loop through each shadow map
    for (int i = 0; i < (*m_SpotLights).size(); i++)
    {
        // Activate texture unit
        glActiveTexture(GL_TEXTURE0 + TextureManager::getNextUnit());

        // Bind the shadow map texture
        glBindTexture(GL_TEXTURE_2D, depthMap[i]);

        // Construct the uniform location string for the current shadow map
        string location = "shadowMap[" + std::to_string(i) + "]";

        // Get the uniform location for the shadow map in the shader
        GLint shadowMapLocation = glGetUniformLocation(shaderID, location.c_str());

        // Set the uniform value to the corresponding texture unit index
        glUniform1i(shadowMapLocation, TextureManager::getCurrentUnit());
    }
}

GLuint ShadowMap::getDepthMapID(int i)
{
    return depthMap[i];
}

GLuint ShadowMap::getDepthMapFBOID(int i)
{
    return depthMapFBO[i];

}