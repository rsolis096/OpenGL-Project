#include "ShadowMap.h"

ShadowMap::ShadowMap(std::vector<Object*>* objects, std::vector<SpotLight*>* spotlights):
    m_SceneObjects(objects), m_SpotLights(spotlights), m_UpdateShadowMap(true)
{
    // Initialize shaders

    //Drawn to to generate depth map
	depthShader = Shader("shaders/shadow_mapping_depth.vert", "shaders/shadow_mapping_depth.frag");

    //Drawn to to display a debug image of the depth map
    debugDepthShader = Shader("shaders/debug_quad.vert", "shaders/debug_quad_depth.frag");


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
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[m_ShadowCasters]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[m_ShadowCasters], 0);

    //No color data needs to be rendered
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete: " << status << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_ShadowCasters++;
    glCheckError();
}

void ShadowMap::ShadowPass() 
{
    //Shadow Pass (Generate depth maps)
    ////Currently only working on static scenes, further updates require manual overrides

    unsigned int numberOfSpotLights = m_SpotLights->size();

    //Spotlights

    //NEED TO FIND A WAY TO LIMIT CHANGE UNLESS SCENE MOVES
    if (true)//(m_UpdateShadowMap)     //m_UpdateShadowMap is set to true upon += 1 spotlight
    {
        //Re draw the depth maps, clear the light space matrices
        m_LightSpaceMatrices.clear();
        if (numberOfSpotLights > 0)
            m_LightSpaceMatrices.resize(numberOfSpotLights, glm::mat4(1.0f));

        //Generate the light space matrices
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
                (*m_SpotLights)[i]->getLightPos() + (*m_SpotLights)[i]->getLightDirection(), // Adjusted for direction
                glm::vec3(0.0, 1.0, 0.0)
            );

            // Combine projection and view matrices to get the light-space matrix
            m_LightSpaceMatrices[i] = lightProjection * lightView;

            // Set the viewport and bind the frame buffer
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

            //Render to depth map
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
            glClear(GL_DEPTH_BUFFER_BIT);

            depthShader.use();
            // Pass the light-space matrices to the shader
            glUniformMatrix4fv(
                glGetUniformLocation(depthShader.m_ProgramId, "lightSpaceMatrix"),
                1, // Pass 1 matrix
                GL_FALSE,
                glm::value_ptr(getLightSpaceMatrices()[i])
            );

        	for (Object* obj : *m_SceneObjects)
                obj->Draw(depthShader);

            //Render to depth map
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glCheckError();
        }
    }

    m_UpdateShadowMap = false;
}

void ShadowMap::drawShadowMap(GLuint shaderID) const
{
    glUseProgram(shaderID);

    for (size_t i = 0; i < depthMap.size(); ++i)
    {
        GLuint textureUnit = TextureManager::getNextUnit();
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, depthMap[i]);

        std::string uniformName = "shadowMap[" + std::to_string(i) + "]";
        GLint shadowMapUniformLocation = glGetUniformLocation(shaderID, uniformName.c_str());
        if (shadowMapUniformLocation == -1) {
            std::cerr << "ERROR: Uniform location for " << uniformName << " not found.\n";
            return;
        }

        glUniform1i(shadowMapUniformLocation, textureUnit);
    }

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error in drawShadowMap: " << error << std::endl;
    }
}

glm::mat4& ShadowMap::getLightSpaceMatrix(int i){
    return m_LightSpaceMatrices[i];
}

std::vector<glm::mat4>& ShadowMap::getLightSpaceMatrices(){
    return m_LightSpaceMatrices;
}

GLuint ShadowMap::getDepthMapID(int i){
    return depthMap[i];
}

GLuint ShadowMap::getDepthMapFBOID(int i){
    return depthMapFBO[i];
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
    int numRows = (m_SpotLights->size() + numCols - 1) / numCols; // Calculate rows needed

    float quadSize = 2.0f / numCols; // Size of each quad, assuming [-1, 1] range for positions
    float spacing = 0.05f; // Space between quads
    float totalWidth = numCols * (quadSize + spacing) - spacing; // Total width considering spacing


    for (size_t i = 0; i < m_SpotLights->size(); ++i)
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
        glBindTexture(GL_TEXTURE_2D, depthMap[i]); // Bind the depth map of the current spotlight
        debugDepthShader.setInt("depthMap", 0); // Set the sampler to texture unit 0

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

}
