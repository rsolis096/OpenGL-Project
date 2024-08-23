#pragma once

// Include necessary headers
#include "Shader.h"
#include "Object.h"
#include "LightController.h"
#include <vector>

class ShadowMap
{
private:

    // Private member variables
    GLuint framebuffer;
    GLuint colorTexture;

    // Shaders
    Shader shadowPassShader;
    Shader debugDepthShader;

    // Scene objects and light controller
    std::vector<Object*>* m_SceneObjects;
    LightController* m_LightController;

    // Point Lights
    std::vector<GLuint> depthMapFBOPointLights;
    std::vector<GLuint> depthCubeMaps;

    // Light space matrices for spotlights
    std::vector<glm::mat4> m_LightSpaceMatrices;

    // Static variables for quad VAO and VBO
    static unsigned int quadVAO;
    static unsigned int quadVBO;
    static bool quadInitialized;

    // Generic Size
    static const unsigned short SHADOW_WIDTH = 1024;
    static const unsigned short SHADOW_HEIGHT = 1024;

    // Private helper methods
    void InitializeQuadBuffers(unsigned int& quadVAO, unsigned int& quadVBO, bool invertQuad);
    void GenerateGUIShadowMap();


public:
    // Constructor
    ShadowMap(std::vector<Object*>* objects, LightController* lightController);

    // Spot Lights
    std::vector<GLuint> depthMapFBOSpotLights;
    std::vector<GLuint> depthMapSpotLights;

    // Public methods
    void ShadowPass();
    void addShadowMap();
    void addCubeMap();
    void updateShaderUniforms(Shader& shader) const;
    GLuint RenderToGUIWindow(unsigned short);
    void debugShadowMap();
};