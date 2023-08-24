#include"Object.h"

class Cube  : public Object
{
public:
    // Constructor to initialize the cube's properties
    Cube(std::vector<float> inputVertices, std::vector<float> inputNormals, std::vector<float> inputTexCoords,
        const char* texturePathDiffuse, const char* texturePathSpecular, Shader& shader);


    Cube(std::vector<float> inputVertices, std::vector<float> inputNormals, Shader& shader);

    // Destructor to release any resources
    ~Cube();   

};