#include <iostream>
#include <vector>
#include <glad/glad.h> //Opengl functions
#include "Texture.h"

class Cube {
public:
    // Constructor to initialize the cube's properties
    Cube(float inputVertices[], float inputNormals[], float inputIndices[], const char* texturePath, unsigned int sId);
    Cube(float inputVertices[], float inputNormals[],  unsigned int sId);

    // Destructor to release any resources
    ~Cube();   

    // Function to render the cube
    void render();
    void bind();
private:
    void buildInterleavedVertices();

    void buildInterleavedVerticesWithTexCoords();
    // Cube properties
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<float> interleavedVertices;
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