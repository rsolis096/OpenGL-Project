#include "ShadowMap.h"

ShadowMap::ShadowMap(std::vector<Object*>* objects, std::vector<SpotLight*>* spotlights):
    m_SceneObjects(objects), m_SpotLights(spotlights), m_UpdateShadowMap(true)
{
    // Initialize shaders
	depthShader = Shader("shaders/shadow_mapping_depth.vert", "shaders/shadow_mapping_depth.frag");
    
    m_ShadowCasters = 0;
}

//Called when adding a new light source (currently only spotlight)
void ShadowMap::addShadowMap()
{
    depthMapFBO.push_back(0);
    depthMap.push_back(0);
    m_UpdateShadowMap = true;
    // Generate framebuffer and depth map texture of the new item
    glGenFramebuffers(1, &depthMapFBO[m_ShadowCasters]);
    glGenTextures(1, &depthMap[m_ShadowCasters]);

    // Bind the depth map texture to the current texture unit
    glBindTexture(GL_TEXTURE_2D, depthMap[m_ShadowCasters]);
    // Bind the depth map texture to the current texture unit
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    // Set the minification filter to nearest neighbor
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Set the magnification filter to nearest neighbor
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Set the horizontal wrapping mode to clamp to border
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // Set the vertical wrapping mode to clamp to border
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    // Define the border color as white
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    // Set the border color for the texture
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[m_ShadowCasters]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[m_ShadowCasters], 0);
    //No color data needs to be rendered
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_ShadowCasters++;
}

void ShadowMap::ShadowPass() 
{
    //Shadow Pass (Generate depth maps)
    
    // 1. render depth of scene to texture (from light's perspective)
    
    //TODO: Efficiency Improvment Opportunity - Shadow Maps
    //Clear lightSpaceMatrices vector (not very efficient for unchanging matrices)
    //Currently, every spotlight calculates its corresponding depth map even if scene is unchanged

    unsigned int numberOfSpotLights = m_SpotLights->size();

    //Currently only working on static scenes, further updates require manual overrides
    //m_UpdateShadowMap is set to true upon += 1 spotlight
    if (m_UpdateShadowMap)
    {
        m_LightSpaceMatrices.clear();
        if (numberOfSpotLights > 0)
            m_LightSpaceMatrices.resize(numberOfSpotLights, glm::mat4(1.0f));

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

        // Render to each depth map (1 for each spotlight)
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

            //Render to depth map
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
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


}

void ShadowMap::drawShadowMap(GLuint& shaderID)
{
    // Loop through each shadow map (
    // this actually draws the shadow map to the screen, does not generate depth maps)
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

glm::mat4& ShadowMap::getLightSpaceMatrix(int i)
{
    return m_LightSpaceMatrices[i];
}

std::vector<glm::mat4>& ShadowMap::getLightSpaceMatrices()
{
    return m_LightSpaceMatrices;
}

GLuint ShadowMap::getDepthMapID(int i)
{
    return depthMap[i];
}

GLuint ShadowMap::getDepthMapFBOID(int i)
{
    return depthMapFBO[i];

}

void ShadowMap::setUpdateShadowMap()
{
    m_UpdateShadowMap != m_UpdateShadowMap;
}
