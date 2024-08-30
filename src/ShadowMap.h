#pragma once

// Include necessary headers
#include "Shader.h"
#include "Object.h"
#include "LightController.h"
#include <vector>

class ShadowMap
{

public:

    // Shaders
    Shader shadowPassShader;
    Shader debugDepthShader;

    // Constructor
    ShadowMap(std::vector<Object*>* objects, LightController* lightController);

    // Public methods
    void addSpotLightShadowMap(GLuint& depthMapTexture) const;
    void addDirectionalShadowMap(GLuint& depthMapTexture) const;
    void addPointLightShadowMap(GLuint& cubeMapTexture) const;
    void updateShadowResolution(DirectionalLight* light) const; 
    void updateShadowResolution(SpotLight* light) const;

    //Main Loop Methods (Used during the rendering loop every frame)
    void shadowPass();
    void updateShaderUniforms(Shader& shader) const;
    GLuint renderDepthMapToGUI(GLuint texture, int height, int width);
    //GLuint renderCubeMapToGUI(PointLight* lightSource, unsigned int i);

private:

    // Frame buffer Objects

    //Shadow Frame buffer objects
    GLuint m_FBO;
    GLuint m_CubeMapFBO;

    //GUI frame buffer objects
    GLuint m_2DGUI_FBO;
    GLuint m_3DGUI_FBO;

    //Depth Map Textures

    //Color Texture for rendering GUI depth map
    GLuint m_GUIColorTexture;
    GLuint m_GUIColorCubeMap;

    // Light Space Matrices
    std::vector<glm::mat4> m_LightSpaceMatrices;
    glm::mat4 directionalLightSpaceMatrix;

    // Scene Pointers
    std::vector<Object*>* m_SceneObjects;
    LightController* m_LightController;


    // Static variables for quad VAO and VBO (Used in GUI)
    static unsigned int quadVAO;
    static unsigned int quadVBO;
    static bool quadInitialized;

    // Generic Size
    static const unsigned short SHADOW_WIDTH = 1024;
    static const unsigned short SHADOW_HEIGHT = 1024;
    
    void addShadowMap(GLuint& depthMapTexture) const;
    void addCubeMap(GLuint& cubeMapTexture) const;

    // Private helper methods
    void generateGUIShadowMap();
    //void generateGUICubeMap();

    static void initializeQuadBuffers(unsigned int& quadVAO, unsigned int& quadVBO, bool invertQuad);
};
