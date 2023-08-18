#include <iostream>
#include <vector>
#include <glad/glad.h> //Opengl functions

class Cube {
public:
    // Constructor to initialize the cube's properties
    Cube(float inputVertices[], int size);

    // Destructor to release any resources
    ~Cube();   

    // Function to render the cube
    void bind();

private:
    // Cube properties
    std::vector<float> vertices;

    //std::vector<float> uvCoords;
    //std::vector<float> colour;


    // Vertex array object (VAO) and vertex buffer object (VBO)
    GLuint m_vao;
    GLuint m_vbo;

    // Other private functions for setting up VAO, VBO, etc.
};