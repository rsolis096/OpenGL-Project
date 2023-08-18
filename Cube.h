#include <iostream>
#include <vector>
#include <glad/glad.h> //Opengl functions
#include "Texture.h"

class Cube {
public:
    // Constructor to initialize the cube's properties
    Cube(float inputVertices[], int size, const char* texturePath, unsigned int sId);
    Cube(float inputVertices[], int size, unsigned int sId);

    // Destructor to release any resources
    ~Cube();   

    // Function to render the cube
    void render();
    void bind();
private:
    void initializeCube();
    // Cube properties
    std::vector<float> vertices;
    Texture* texture;

    //std::vector<float> uvCoords;
    //std::vector<float> colour;


    // Vertex array object (VAO) and vertex buffer object (VBO)
    GLuint m_vao;
    GLuint m_vbo;
    unsigned int shaderID;
    bool hasTexture;

    // Other private functions for setting up VAO, VBO, etc.
};