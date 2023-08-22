#include <iostream>
#include <vector>
#include <glad/glad.h> //Opengl functions
#include "Texture.h"
#include <string>

class Cube {
public:
    // Constructor to initialize the cube's properties
    Cube(std::vector<float> inputVertices, std::vector<float> inputNormals, std::vector<float> inputTexCoords,
        const char* texturePathDiffuse, const char* texturePathSpecular, unsigned int sId);


    Cube(std::vector<float> inputVertices, std::vector<float> inputNormals, unsigned int sId);

    // Destructor to release any resources
    ~Cube();   

    // Function to render the cube
    void render();
private:

    //Helper Functions
    void buildInterleavedVertices();
    void buildInterleavedVerticesWithTexCoords();

    // Cube properties
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<float> interleavedVertices;
    Texture* diffuseMap;
    Texture* specularMap;
    unsigned int shaderID;
    bool hasTexture;

    // Vertex array object (VAO) and vertex buffer object (VBO)
    GLuint m_vao;
    GLuint m_vbo;


};