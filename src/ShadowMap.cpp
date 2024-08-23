#include "ShadowMap.h"

//Static members initialization
unsigned int ShadowMap::quadVAO = 0;
unsigned int ShadowMap::quadVBO = 0;
bool ShadowMap::quadInitialized = false;

ShadowMap::ShadowMap(std::vector<Object*>* objects, LightController* lightController) :
    m_SceneObjects(objects), m_LightController(lightController),
	shadowPassShader(Shader("shaders/depthShader.vert", "shaders/depthShader.frag", "shaders/depthShader.gs")),
	debugDepthShader(Shader("shaders/debug_quad.vert", "shaders/debug_quad_depth.frag"))
{
    //Everything done in initializer list
}

// Adds a new shadow map for a spotlight in the scene.
void ShadowMap::addShadowMap()
{
    // Expand the vectors to accommodate the new spotlight
    depthMapFBOSpotLights.emplace_back(0);
    depthMapSpotLights.emplace_back(0);

    unsigned short numberOfSpotLights = m_LightController->m_SpotLights.size() - 1;

    // Generate and configure depth map texture
    glGenFramebuffers(1, &depthMapFBOSpotLights[numberOfSpotLights]);
    glGenTextures(1, &depthMapSpotLights[numberOfSpotLights]);

    glBindTexture(GL_TEXTURE_2D, depthMapSpotLights[numberOfSpotLights]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOSpotLights[numberOfSpotLights]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapSpotLights[numberOfSpotLights], 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    //Generates a RGB depth map texture to be used by the GUI
    GenerateGUIShadowMap();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCheckError();
}

// Sets up a framebuffer and cube map texture for point light shadow mapping.
void ShadowMap::addCubeMap()
{
    unsigned int numberOfPointLights = m_LightController->m_PointLights.size() - 1;

    depthMapFBOPointLights.emplace_back(0);
    glGenFramebuffers(1, &depthMapFBOPointLights[numberOfPointLights]);

    depthCubeMaps.emplace_back(0);
    glGenTextures(1, &depthCubeMaps[numberOfPointLights]);

    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMaps[numberOfPointLights]);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOPointLights[numberOfPointLights]);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMaps[numberOfPointLights], 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Generate shadow maps for spotlights and point lights.
void ShadowMap::ShadowPass()
{
    unsigned int numberOfSpotLights = m_LightController->m_SpotLights.size();
    unsigned int numberOfPointLights = m_LightController->m_PointLights.size();

    float near_plane = 1.0f;
    float far_plane = 25.0f;

    if (numberOfPointLights > 0) {
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shadowPassShader.use();
        shadowPassShader.setInt("lightType", 0);
        shadowPassShader.setFloat("far_plane", far_plane);

        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f),
            static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT), near_plane, far_plane);

        for (unsigned int i = 0; i < numberOfPointLights; i++) {
            glm::vec3 lightPos = m_LightController->m_PointLights[i]->getLightPos();
            shadowPassShader.setVec3("lightPos", lightPos);

            glm::mat4 shadowTransforms[6] = {
                shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
                shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
                shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
                shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
                shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
                shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
            };

            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOPointLights[i]);
            glClear(GL_DEPTH_BUFFER_BIT);

            for (unsigned int j = 0; j < 6; j++) {
                shadowPassShader.setMat4("shadowMatrices[" + std::to_string(j) + "]", shadowTransforms[j]);
            }

            for (Object* obj : *m_SceneObjects) {
                obj->ShadowPassDraw(shadowPassShader);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        glCheckError();
    }

    if (numberOfSpotLights > 0) {
        m_LightSpaceMatrices.clear();

        shadowPassShader.use();
        shadowPassShader.setInt("lightType", 1);

        for (int i = 0; i < numberOfSpotLights; i++) {

            glm::mat4 lightProjection = glm::perspective(
                glm::radians(45.0f),
                static_cast<float>(SHADOW_WIDTH / SHADOW_HEIGHT),
                m_LightController->m_SpotLights[i]->getNearPlane(),
                m_LightController->m_SpotLights[i]->getFarPlane()
            );

            glm::vec3 lightPos = m_LightController->m_SpotLights[i]->getLightPos();
            glm::vec3 lightDir = m_LightController->m_SpotLights[i]->getLightDirection();

            // Calculate the target point by adding the direction vector to the position
            glm::vec3 target = lightPos + lightDir;

            glm::mat4 lightView = glm::lookAt(
                lightPos,      // Position of the light
                target,        // The target point (position + direction)
                glm::vec3(0.0, 1.0, 0.0)  // Up vector
            );

            m_LightSpaceMatrices.push_back(lightProjection * lightView);

            shadowPassShader.setMat4("shadowPassMatrix", m_LightSpaceMatrices[i]);

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOSpotLights[i]);
            glClear(GL_DEPTH_BUFFER_BIT);

            for (Object* obj : *m_SceneObjects) {
                obj->ShadowPassDraw(shadowPassShader);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
}

// This method sets up the uniforms for both spotlights and point lights in the main rendering pass.
void ShadowMap::updateShaderUniforms(Shader& shader) const
{
    unsigned int numberOfSpotLights = m_LightController->m_SpotLights.size();
    unsigned int numberOfPointLights = m_LightController->m_PointLights.size();

    // Type 0 - Point Lights
    if (numberOfPointLights > 0)
    {
        // Activate main lighting shader
        shader.use();
        float far_plane = 25.0f;

        //shader.setVec3("lightPos", lightPos);
        shader.setFloat("far_plane", far_plane);
        shader.setInt("lightType", 0);
        shader.setInt("numberOfPointLightsFRAG", numberOfPointLights);


        // Apply Textures for Point Lights
        for (int i = 0; i < depthCubeMaps.size(); i++)
        {
            GLuint textureUnit = TextureManager::getNextUnit();
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMaps[i]);

            shader.setVec3("pointLights[" + std::to_string(i) + "].position", m_LightController->m_PointLights[i]->getLightPos());
            shader.setInt("pointLights[" + std::to_string(i) + "].shadowMap", TextureManager::getCurrentUnit());
        }
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
            GLuint textureUnit = TextureManager::getNextUnit();
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, depthMapSpotLights[i]);

            shader.setInt("spotLights[" + std::to_string(i) + "].shadowMap", textureUnit);
        }
    }

    glCheckError();
}


// Generates a framebuffer with a texture attachment for rendering a shadow map to be used in a GUI window. 
void ShadowMap::GenerateGUIShadowMap()
{
    // Generate a framebuffer and bind it as the current framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Generate a 2D texture for storing the shadow map and configure it
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Set texture filtering and wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach the texture to the framebuffer as the color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    // Check if the framebuffer is complete and report any errors
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    // Check for any OpenGL errors during the framebuffer setup
    glCheckError();
}

GLuint ShadowMap::RenderToGUIWindow(unsigned short index)
{

    // Basic MPV matrices for the ImGui Window
    glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix for the model
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(54.0f), 1.0f, 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float near_plane = 1.0f, far_plane = 17.5f;

    // Set up the shader and uniform variables
    debugDepthShader.use();
    debugDepthShader.setFloat("near_plane", near_plane);
    debugDepthShader.setFloat("far_plane", far_plane);
    debugDepthShader.setMat4("model", modelMatrix);
    debugDepthShader.setMat4("view", viewMatrix);
    debugDepthShader.setMat4("projection", projectionMatrix);

    //Set up the quad to be rendered to
    if (!quadInitialized)
    {
        InitializeQuadBuffers(quadVAO, quadVBO, true);
        quadInitialized = true;
    }

    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render the depth map texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMapSpotLights[index]);
    debugDepthShader.setInt("depthMap", 0);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clean up resources (need to re-add this)
    //glDeleteFramebuffers(1, &framebuffer);
    //glDeleteTextures(1, &colorTexture);
    //glDeleteVertexArrays(1, &quadVAO);
    //glDeleteBuffers(1, &quadVBO);
    glCheckError();

    return colorTexture;
}

void ShadowMap::debugShadowMap()
{
    // render Depth map to quad for visual debugging
    // ---------------------------------------------
    float near_plane = 1.0f, far_plane = 17.5f; //intensity of quad
    debugDepthShader.use();
    debugDepthShader.setFloat("near_plane", near_plane);
    debugDepthShader.setFloat("far_plane", far_plane);

    //Initialize quad
    if (!quadInitialized)
    {
        InitializeQuadBuffers(quadVAO, quadVBO, false);
        quadInitialized = true;
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

        //Render Quad
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapSpotLights[i]);
        debugDepthShader.setInt("depthMap", 0);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
    glCheckError();
}

void ShadowMap::InitializeQuadBuffers(unsigned int& quadVAO, unsigned int& quadVBO, bool invertQuad)
{
    if (quadVAO == 0)
    {
        //Regular
        std::vector<float> quadVertices = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        //Inverted
        if (invertQuad)
        {
            quadVertices = {
                -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
                    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                    1.0f, -1.0f, 0.0f, 1.0f, 1.0f
            };
        }

        // setup plane VAO and VBO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    glCheckError();

}

