#include "ShadowMap.h"

#include <glm/gtx/string_cast.hpp>

//Static members initialization
unsigned int ShadowMap::quadVAO = 0;
unsigned int ShadowMap::quadVBO = 0;
bool ShadowMap::quadInitialized = false;

ShadowMap::ShadowMap(std::vector<Object*>* objects, LightController* lightController) :
    m_SceneObjects(objects), m_LightController(lightController),
    shadowPassShader(Shader("shaders/depthShader.vert", "shaders/depthShader.frag", "shaders/depthShader.gs")),
    debugDepthShader(Shader("shaders/debug_quad.vert", "shaders/debug_quad_depth.frag"))
{
    //Initialize Dir Light Space Matrix identity 4
    directionalLightSpaceMatrix = glm::mat4(1.0f);

    //Initialize Directional Map / SpotLight FBO
    m_FBO = 0;
    glGenFramebuffers(1, &m_FBO);

    //Initialize Point Light FBO
    m_CubeMapFBO = 0;
    glGenFramebuffers(1, &m_CubeMapFBO);

    m_2DGUI_FBO = 0;
    glGenFramebuffers(1, &m_2DGUI_FBO);

    m_3DGUI_FBO = 0;
    glGenFramebuffers(1, &m_3DGUI_FBO);

    //Create a frame buffer object and color texture to be rendered in ImGUI
    generateGUIShadowMap();

    // Initialize the quad the gui will render depth maps to
    initializeQuadBuffers(quadVAO, quadVBO, true);
}

void ShadowMap::addSpotLightShadowMap(GLuint& depthMapTexture)
{
    // Expand the vector to accommodate the new shadow map
    addShadowMap(depthMapTexture);
}

void ShadowMap::addDirectionalShadowMap(GLuint& depthMapTexture)
{
    //Create a new 2D Depth map for Directional Lights (only supports 1 atm)
    addShadowMap(depthMapTexture);
}

void ShadowMap::addPointLightShadowMap(GLuint& cubeMapTexture)
{
    //Create a new 3D Depth map for Point Lights
    addCubeMap(cubeMapTexture);
}

void ShadowMap::addShadowMap(GLuint& depthMapTexture) const
{

    // Generate and configure depth map texture
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Bind the FBO and attach the texture
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Frame buffer not complete! \n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCheckError();
}

void ShadowMap::addCubeMap(GLuint& cubeMapTexture) const
{
    glGenTextures(1, &cubeMapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_CubeMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Frame buffer not complete!\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Generate shadow maps for spotlights and point lights.
void ShadowMap::shadowPass()
{
    const int numberOfSpotLights = static_cast<int>(m_LightController->m_SpotLights.size());
    const int numberOfPointLights = static_cast<int>(m_LightController->m_PointLights.size());

    shadowPassShader.use();

    // Process point lights
    if (numberOfPointLights > 0) {

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);


        shadowPassShader.setInt("lightType", LightController::LightType::POINT_LIGHT);  // Set light type to point light


        for (int i = 0; i < static_cast<int>(numberOfPointLights); i++)
        {
            if(m_LightController->m_PointLights[i]->getShadowPassUpdate())
            {
                glm::vec3 lightPos = m_LightController->m_PointLights[i]->getLightPos();
                shadowPassShader.setVec3("pointLightPos", lightPos);
                shadowPassShader.setFloat("far_plane", m_LightController->m_PointLights[i]->getFarPlane());  // Set far plane distance

                // See point light constructor for projection matrices computations
                const std::array<glm::mat4, 6> lightViews = m_LightController->m_PointLights[i]->getLightViews();

                // Set viewport to match shadow map resolution
                glBindFramebuffer(GL_FRAMEBUFFER, m_CubeMapFBO);
                glClear(GL_DEPTH_BUFFER_BIT);

                // Set shadow matrices for each face (used in geometry shader)
                for (int j = 0; j < 6; j++) {
                    shadowPassShader.setMat4("shadowMatricesPoint[" + std::to_string(j) + "]", lightViews[j]);

                    // Attach the appropriate face of the cube map texture as the depth attachment
                    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, m_LightController->m_PointLights[i]->getCubeMapTexture(), 0);
                    //glClear(GL_DEPTH_BUFFER_BIT);

                    // Render scene objects to shadow map (6 times for each light source)
                    for (Object* obj : *m_SceneObjects) {
                        obj->ShadowPassDraw(shadowPassShader);
                    }
                }
            }

        }

        // Unbind the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glCheckError();
    }

    //Process SpotLights
    if (numberOfSpotLights > 0) {

        shadowPassShader.setInt("lightType", LightController::LightType::SPOT_LIGHT);

        for (int i = 0; i < numberOfSpotLights; i++)
        {
            //LightSpace Matrix initialized in spotlight constructor 
            shadowPassShader.setMat4("shadowPassMatrixSpot", m_LightController->m_SpotLights[i]->getLightSpaceMatrix());

            const int width = m_LightController->m_SpotLights[i]->getShadowWidth();
            const int height = m_LightController->m_SpotLights[i]->getShadowHeight();

            // Set up the frame buffer and viewport for shadow map rendering, and clear the depth buffer.
            glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_LightController->m_SpotLights[i]->getDepthMapTexture(), 0);
            glViewport(0, 0, width, height);
            glClear(GL_DEPTH_BUFFER_BIT);

            //Draw Scene (limited for shadow pass)
            for (Object* obj : *m_SceneObjects) {
                obj->ShadowPassDraw(shadowPassShader);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    // Process directional light
    if (m_LightController->m_DirectionalLight != nullptr)
    {

        shadowPassShader.setInt("lightType", LightController::LightType::DIRECTIONAL_LIGHT);  // Set light type to directional light

        float near_plane = m_LightController->m_DirectionalLight->getNearPlane();
        float far_plane = m_LightController->m_DirectionalLight->getFarPlane();
        float scene_radius = m_LightController->m_DirectionalLight->getRadius();

        const glm::mat4 lightProjection = 
            glm::ortho(
                -scene_radius, scene_radius, 
                -scene_radius, scene_radius, 
                near_plane, far_plane
            );

        const glm::vec3 lightDir = -m_LightController->m_DirectionalLight->m_LightDirection;
        const glm::vec3 lightPos = m_LightController->m_DirectionalLight->m_LightPosition;

        const glm::mat4 lightView = glm::lookAt(
            lightPos,      
            lightPos + lightDir,        
            glm::vec3(0.0, 1.0, 0.0)  // Up vector
        );

        directionalLightSpaceMatrix = lightProjection * lightView;

        // Set shadow pass matrix for directional light
        shadowPassShader.setMat4("shadowPassMatrixDir", directionalLightSpaceMatrix);

        // Set up the frame buffer and viewport for shadow map rendering, and clear the depth buffer.
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_LightController->m_DirectionalLight->getDepthMapTexture(), 0);
        glViewport(0, 0, m_LightController->m_DirectionalLight->getShadowWidth(), m_LightController->m_DirectionalLight->getShadowHeight());
        glClear(GL_DEPTH_BUFFER_BIT);

        // Render scene objects to shadow map
        for (Object* obj : *m_SceneObjects) {
            obj->ShadowPassDraw(shadowPassShader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

}

// Updates Shadow Map shader uniforms for each light source
void ShadowMap::updateShaderUniforms(Shader& shader) const
{
    const int numberOfSpotLights = static_cast<int>(m_LightController->m_SpotLights.size());
    const int numberOfPointLights = static_cast<int>(m_LightController->m_PointLights.size());

    //Main lighting shader used for all light sources
    shader.use();

    shader.setInt("numberOfSpotLights", numberOfSpotLights);
    shader.setInt("numberOfPointLights", numberOfPointLights);

    // Type 0 - Point Lights
    if (numberOfPointLights > 0)
    {
        // Apply shadow map textures for Point Lights
        for (int i = 0; i < numberOfPointLights; i++)
        {
            const int textureUnit = static_cast<int>(TextureManager::getNextUnit());
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_LightController->m_PointLights[i]->getCubeMapTexture());
            shader.setVec3("pointLights[" + std::to_string(i) + "].position", m_LightController->m_PointLights[i]->getLightPos());
            shader.setInt("pointLights[" + std::to_string(i) + "].shadowMap", textureUnit);
        }
    }

    //Type 1 - SpotLights
    if (numberOfSpotLights > 0)
    {
        //Apply shadow map textures and light space matrix
        for (int i = 0; i < numberOfSpotLights; i++)
        {
            const int textureUnit = static_cast<int>(TextureManager::getNextUnit());
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, m_LightController->m_SpotLights[i]->getDepthMapTexture());

            //TODO: Remove dependence on SpotLight Indexing to allow for dynamic removal/addition to spotlights
            shader.setInt("spotLights[" + std::to_string(i) + "].shadowMap", textureUnit);
            shader.setMat4("spotLights[" + std::to_string(i) + "].lightSpaceMatrix", m_LightController->m_SpotLights[i]->getLightSpaceMatrix());
        }
    }

    //Type 2 - Directional Lights
    if (m_LightController->m_DirectionalLight)
    {
        shader.setMat4("directionalLightSpaceMatrix", directionalLightSpaceMatrix);
        const int textureUnit = static_cast<int>(TextureManager::getNextUnit());
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, m_LightController->m_DirectionalLight->getDepthMapTexture());
        shader.setInt("dirLight.shadowMap", textureUnit);
    }

    glCheckError();
}

// Updates shadow map resolution for provided directional light
void ShadowMap::updateShadowResolution(DirectionalLight* light) const
{
	// Delete the old texture
	glDeleteTextures(1, &light->getDepthMapTexture());

	// Create a new texture with the new resolution
	int newWidth = light->getShadowWidth(); 
	int newHeight = light->getShadowHeight(); 
	glGenTextures(1, &light->getDepthMapTexture());
	glBindTexture(GL_TEXTURE_2D, light->getDepthMapTexture());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, newWidth, newHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Bind the new texture to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light->getDepthMapTexture(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

// Updates shadow map resolution for provided spot light
void ShadowMap::updateShadowResolution(SpotLight* light) const
{
    // Delete the old texture
    glDeleteTextures(1, &light->getDepthMapTexture());

    // Create a new texture with the updated resolution
    int newWidth = light->getShadowWidth();
    int newHeight = light->getShadowHeight();
    glGenTextures(1, &light->getDepthMapTexture());
    glBindTexture(GL_TEXTURE_2D, light->getDepthMapTexture());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, newWidth, newHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Bind the new texture to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light->getDepthMapTexture(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

// Generates a frame buffer with a texture attachment for rendering a shadow map to be used in a GUI window. 
void ShadowMap::generateGUIShadowMap()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_2DGUI_FBO);

    // Generate a 2D texture for storing the shadow map and configure it
    glGenTextures(1, &m_GUIColorTexture);
    glBindTexture(GL_TEXTURE_2D, m_GUIColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Set texture filtering and wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach the texture to the frame buffer as the color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GUIColorTexture, 0);

    // Cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Check for any OpenGL errors during the frame buffer setup
    glCheckError();
}

// Renders the shadow map of a given light source to a small quad in the gui
GLuint ShadowMap::renderDepthMapToGUI(GLuint texture, int height, int width)
{
    // Basic MPV matrices for the ImGui Window
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // The following values are manually tuned and cannot be changed in real-time
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

    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_2DGUI_FBO);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set texture unit with depth map texture
    glActiveTexture(GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, texture);
    debugDepthShader.setInt("depthMap", 0);

    // Draw to GUI quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Clean up
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCheckError();

    return m_GUIColorTexture;
}

void ShadowMap::initializeQuadBuffers(unsigned int& qVAO, unsigned int& qVBO, bool invertQuad)
{
    if (qVAO == 0)
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
        glGenVertexArrays(1, &qVAO);
        glGenBuffers(1, &qVBO);
        glBindVertexArray(qVAO);

        glBindBuffer(GL_ARRAY_BUFFER, qVBO);
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