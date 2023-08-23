#include"Object.h"

class Cube  : public Object
{
public:
    // Constructor to initialize the cube's properties
    Cube(std::vector<float> inputVertices, std::vector<float> inputNormals, std::vector<float> inputTexCoords,
        const char* texturePathDiffuse, const char* texturePathSpecular, unsigned int sId);


    Cube(std::vector<float> inputVertices, std::vector<float> inputNormals, unsigned int sId);

    // Destructor to release any resources
    ~Cube();   


};