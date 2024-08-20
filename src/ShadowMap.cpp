#include "ShadowMap.h"

ShadowMap::ShadowMap(std::vector<Object*>* objects, LightController* lightController, std::vector<SpotLight*>* spotlights) :
    m_SceneObjects(objects), m_LightController(lightController), m_UpdateShadowMap(true), m_SpotLights(spotlights)
{
    // Initialize shaders

    //Drawn to to generate depth map
	depthShader = Shader("shaders/depthShader.vert", "shaders/depthShader.frag", "shaders/depthShader.gs");

    //Drawn to display a debug image of the depth map
    debugDepthShader = Shader("shaders/debug_quad.vert", "shaders/debug_quad_depth.frag");

}

// This function adds a new shadow map for a spotlight in the scene.
// It generates a frame buffer object (FBO) and a depth map texture for the new spotlight.
//For Spot Lights
void ShadowMap::addShadowMap()
{
    std::cout << "add Shadow Map \n";
    depthMapFBOSpotLights.push_back(0);
    depthMapSpotLights.push_back(0);
    m_UpdateShadowMap = true;

    unsigned short numberOfSpotLights = m_SpotLights->size() - 1;

    // Generate frame buffer and depth map texture of the new item
    glGenFramebuffers(1, &depthMapFBOSpotLights[numberOfSpotLights]);
    glGenTextures(1, &depthMapSpotLights[numberOfSpotLights]);

    // Bind the depth map texture to the current texture unit
    glBindTexture(GL_TEXTURE_2D, depthMapSpotLights[numberOfSpotLights]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    // Define the border color as white
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    // Set the border color for the texture
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOSpotLights[numberOfSpotLights]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapSpotLights[numberOfSpotLights], 0);

    //No color data needs to be rendered
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete: " << status << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCheckError();
}


void ShadowMap::addCubeMap()
{
    unsigned int numberOfPointLights = m_LightController->m_PointLights.size() - 1;

    depthMapFBOPointLights.push_back(0);
    glGenFramebuffers(1, &depthMapFBOPointLights[0]);

    depthCubeMaps.push_back(0);
    glGenTextures(1, &depthCubeMaps[0]);


    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMaps[0]);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOPointLights[0]);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMaps[0], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

// This function performs the shadow pass to generate depth maps for shadow mapping.
// It iterates over all spotlights in the scene, calculates their light-space matrices,
// and renders the scene from the perspective of each light to create depth maps.
// These depth maps are used later to determine shadowed areas in the final rendering pass.
void ShadowMap::ShadowPass()
{
    unsigned int numberOfSpotLights = m_SpotLights->size();
    unsigned int numberOfPointLights = m_LightController->m_PointLights.size();


    //Type 0 - Point Lights
    if (numberOfPointLights > 0)
    {

        // 0. create depth cubemap transformation matrices
        // -----------------------------------------------
        float near_plane = 1.0f;
        float far_plane = 25.0f;
        glm::vec3 lightPos = m_LightController->m_PointLights[0]->getLightPos();
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT), near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));


        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOPointLights[0]);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader.use();
        depthShader.setInt("lightType", 0);
        for (unsigned int i = 0; i < 6; ++i)
            depthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        depthShader.setFloat("far_plane", far_plane);
        depthShader.setVec3("lightPos", lightPos);


        for (Object* obj : *m_SceneObjects)
            obj->ShadowPassDraw(depthShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCheckError();
    }

    //Type 1 - SpotLights
    if (numberOfSpotLights > 0) // (m_UpdateShadowMap) // m_UpdateShadowMap is set to true upon += 1 spotlight
    {
        // Re-draw the depth maps, clear the light space matrices
        m_LightSpaceMatrices.clear();
        if (numberOfSpotLights > 0)
            m_LightSpaceMatrices.resize(numberOfSpotLights, glm::mat4(1.0f));
        // Generate the light space matrices
        for (int i = 0; i < numberOfSpotLights; i++)
        {
            // Calculate light projection matrix
            glm::mat4 lightProjection = glm::perspective<float>(
                glm::radians(45.0f),
                static_cast<float>(SHADOW_WIDTH / SHADOW_HEIGHT),
                (*m_SpotLights)[i]->getNearPlane(),
                (*m_SpotLights)[i]->getFarPlane()
            );

            // Calculate light view matrix
            glm::mat4 lightView = glm::lookAt(
                (*m_SpotLights)[i]->getLightPos(),
					(*m_SpotLights)[i]->getLightDirection(), // Adjusted for direction
                glm::vec3(0.0, 1.0, 0.0)
            );

            // Combine projection and view matrices to get the light-space matrix
            m_LightSpaceMatrices[i] = lightProjection * lightView;
            glCheckError();
        }

        // Set the viewport and use the depth shader once
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        depthShader.use();
        depthShader.setInt("lightType", 1);

        //Perform Shadow Pass
        for (int i = 0; i < numberOfSpotLights; i++)
        {
            depthShader.setMat4("shadowPassMatrix", m_LightSpaceMatrices[i]);
            //depthShader.setFloat("far_plane", (*m_SpotLights)[i]->getFarPlane());
            //depthShader.setVec3("lightPos", (*m_SpotLights)[i]->getLightPos());
            // Render to depth map
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOSpotLights[i]);
            glClear(GL_DEPTH_BUFFER_BIT);

            for (Object* obj : *m_SceneObjects)
                obj->ShadowPassDraw(depthShader);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glCheckError();
        }
    }

    m_UpdateShadowMap = false;
}


// This method sets up the shadow maps for both spotlights and point lights in the main rendering pass.
// It binds the appropriate shadow map textures and sets the necessary uniforms in the shader.
void ShadowMap::updateShaderUniforms(Shader& shader) const
{
    unsigned int numberOfSpotLights = m_SpotLights->size();
    unsigned int numberOfPointLights = m_LightController->m_PointLights.size();

    // Type 0 - Point Lights
    if (numberOfPointLights > 0)
    {
        // Activate main lighting shader
        shader.use();
        float far_plane = 25.0f;
        glm::vec3 lightPos = m_LightController->m_PointLights[0]->getLightPos();

        //shader.setVec3("lightPos", lightPos);
        shader.setFloat("far_plane", far_plane);
        shader.setInt("lightType", 1);
        shader.setInt("numberOfPointLightsFRAG", 1);

        // Apply Textures for Point Lights
        for (int i = 0; i < depthCubeMaps.size(); i++)
        {
            GLuint textureUnit = TextureManager::getNextUnit();
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMaps[i]);
        }
        shader.setInt("depthMap", 1);

    }

    //Type 1 - SpotLights
    if (numberOfSpotLights > 0)
    {
        //Activate main lighting shader
        shader.use();

        //Send over the light space matrices generated during the shadow pass
        shader.setMat4Array("lightSpaceMatrices", m_LightSpaceMatrices);
        shader.setInt("lightType", 1);
        shader.setInt("numberOfSpotLightsVERT", m_LightSpaceMatrices.size());
        shader.setInt("numberOfSpotLightsFRAG", m_LightSpaceMatrices.size());

        //Apply Textures
        for (int i = 0; i < depthMapSpotLights.size(); i++)
        {
            std::cout << "Applying Textures  " << i << "\n";
            GLuint textureUnit = TextureManager::getNextUnit();
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, depthMapSpotLights[i]);

            std::string shadowMapLocation = "shadowMap[" + std::to_string(i) + "]";
            shader.setInt(shadowMapLocation, textureUnit);
        }
    }

	glCheckError();
}

glm::mat4& ShadowMap::getLightSpaceMatrix(int i){
    return m_LightSpaceMatrices[i];
}

std::vector<glm::mat4>& ShadowMap::getLightSpaceMatrices(){
    return m_LightSpaceMatrices;
}

GLuint ShadowMap::getDepthMapID(int i){
    return depthMapSpotLights[i];
}

GLuint ShadowMap::getDepthMapFBOID(int i){
    return depthMapFBOSpotLights[i];
}

void ShadowMap::setUpdateShadowMap()
{
    m_UpdateShadowMap = !m_UpdateShadowMap;
}

void ShadowMap::debugShadowMap()
{
    // render Depth map to quad for visual debugging
	// ---------------------------------------------
    float near_plane = 1.0f, far_plane = 17.5f; //intensity of quad
    debugDepthShader.use();
    debugDepthShader.setFloat("near_plane", near_plane);
    debugDepthShader.setFloat("far_plane", far_plane);

    //Render Quad
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    int numCols = 4; // Number of columns
    int numRows = (m_LightController->m_SpotLights.size() + numCols - 1) / numCols; // Calculate rows needed
	float quadSize = 2.0f / numCols; // Size of each quad, assuming [-1, 1] range for positions
    float spacing = 0.05f; // Space between quads
    float totalWidth = numCols * (quadSize + spacing) - spacing; // Total width considering spacing

    for (int i = 0; i < m_LightController->m_SpotLights.size(); ++i)
    {

        int row = i / numCols;
        int col = i % numCols;

        // Calculate position offsets for each quad
        float xOffset = (col * (quadSize + spacing)) - (totalWidth / 2.0f); // Center quads horizontally
        float yOffset = (row * (quadSize + spacing)) - (quadSize / 2.0f); // Center quads vertically

        // Update quad vertices with offset (if you choose to modify the vertices directly)
        float quadVertices[] = {
            // positions        // texture Coords
            xOffset,  yOffset + quadSize, 0.0f, 0.0f, 1.0f,
            xOffset,  yOffset,            0.0f, 0.0f, 0.0f,
            xOffset + quadSize, yOffset + quadSize, 0.0f, 1.0f, 1.0f,
            xOffset + quadSize, yOffset,            0.0f, 1.0f, 0.0f,
        };

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapSpotLights[i]);
        debugDepthShader.setInt("depthMap", 0); 

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

}
