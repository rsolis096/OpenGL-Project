#include "Texture.h"

Texture::Texture(const char* filePath, bool flipOnY, std::string type) : ID(0), m_Type(type), m_Path(filePath)
{
    //Creates a Texture
    glGenTextures(1, &ID);

    //Ensure the texture applies
    if (!updateTexture(filePath, flipOnY))
    {
        //Failed to apply texture
        m_Type = "";
        m_Path = "";
        glDeleteTextures(1, &ID);
        ID = GL_INVALID_VALUE;
        //Failure essentially does the same as the default constructor
    }
}

Texture::Texture() : ID(0)
{
    m_Type = "";
    m_Path = "";
    ID = GL_INVALID_VALUE;
}

Texture::~Texture()
{
    glDeleteTextures(1, &ID);
    ID = GL_INVALID_VALUE;
    m_Type = "";
    m_Path = "";
}

//Load Cube map texture
Texture::Texture(std::string path) : ID(0), m_Path(path), m_Type("CubeMap")
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    glCheckError();

    int width, height, nrChannels;

    unsigned char* data;
    std::cout << "Loading Texture: " << path << std::endl;
    data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    //Dimensions for each face
    int faceWidth = width/4;
    int faceHeight = height/3;
    std::cout << width << ", " << height << std::endl;
    std::cout << faceWidth << ", " << faceHeight << std::endl;
    GLenum format = GL_RED;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    else
        std::cout << "Invalid Format" << std::endl;

    std::pair<int, int> positions[] = {
        {0, faceHeight},                //Left
        {2 * faceWidth, faceHeight},    //Right
        {faceWidth, 0},                 //Top
        {faceWidth, 2 * faceHeight},    //Bottom
        {3 * faceWidth, faceHeight},    //Back
        {faceWidth, faceHeight}         //Front
    };

    //Swapping right and left fixing alignment issue along the horizontal
    unsigned int targets[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, //Right
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X, //Left
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,	//Top
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, //Bottom
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, //Back
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, //Front
    };
    glCheckError();

    if (data)
    {
        for (int i = 0; i < 6; ++i)
        {
            int offsetX = positions[i].first;
            int offsetY = positions[i].second;

            glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
            glCheckError();
            glPixelStorei(GL_UNPACK_SKIP_PIXELS, offsetX);
            glCheckError();
            glPixelStorei(GL_UNPACK_SKIP_ROWS, offsetY);

            glCheckError();
            glTexImage2D(
                targets[i], 
                0, 
                format, 
                faceWidth, 
                faceHeight, 
                0, 
                format, 
                GL_UNSIGNED_BYTE, 
                data
            );

            glCheckError();
        }
        // Reset pixel store parameters to their default values
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glCheckError();
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glCheckError();
}

//Load Cube map texture
Texture::Texture(std::vector<std::string> paths) : ID(0), m_Type("CubeMap")
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    int width, height, nrChannels;
    unsigned char* data;
    for (unsigned int i = 0; i < paths.size(); i++)
    {
        std::cout << "Loading Texture: " << paths[i]<< std::endl;
        data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);

        GLenum format = GL_RED;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
            std::cout << "Invalid Format" << std::endl;

        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, 
                format, 
                width, 
                height, 
                0, 
                format, 
                GL_UNSIGNED_BYTE, 
                data
            );

            std::cout << "Successfully loaded texture: " << paths[i] << std::endl;

        }
        else
        {
            std::cout << "Failed to load texture: " << paths[i]<< std::endl;
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

//Allows for texture updates at runtime, also used by constructor
int Texture::updateTexture(const char* filePath, bool flipOnY)
{
    if (flipOnY)
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    
    //TODO: Optimize such that textures already applied are not re applied (filepath = m_Path)
    //Note that constructor uses updateTextures already

    m_Path = filePath;
    int width, height, nrChannels;
    std::cout << "Loading Texture: " << filePath << std::endl;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = GL_RED;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, ID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "Successfully Loaded: " << filePath << std::endl;
        stbi_image_free(data);
        return 1;
    }
    glCheckError();
    std::cout << "Failed to load texture. Please verify filepath: " << filePath << std::endl;
    stbi_image_free(data);
    return 0;
}


//Could be useful for fixing top and bottom of cubemap loaded as t-shaped cube map
//TO CORRECTLY FLIP (IN PHOTOSHOP) FLIP VERTICALLY THEN HORIZONTALLY FOR BOTH THE TOP AND BOTTOM FACES
/*
const char* flipTexture(std::string path)
{
    const char* inputPath = path.c_str();
    const char* outputPath = "output.png";

    int width, height, channels;
    unsigned char* data = stbi_load(inputPath, &width, &height, &channels, 0);
    //4096 x 3072
    //faceHeight = 1365
    //faceWidth = 1024
    if (data) 
    {
        // Define the region to flip (e.g., from (0,0) to (256,256))
        int startX = 1024;
        int startY = 0;
        int endX = 1024*2;
        int endY = 1365;

        // Ensure the region is within bounds
        endX = std::min(endX, width);
        endY = std::min(endY, height);
        // Flip the specified portion vertically
        for (int y = startY; y < endY; ++y) {
            for (int x = startX; x < endX; ++x) {
                int topPixelIndex = (y * width + x) * channels;
                int bottomPixelIndex = ((endY - 1) * width + x) * channels;

                for (int c = 0; c < channels; ++c) {
                    // Swap the pixel values vertically
                    std::swap(data[topPixelIndex + c], data[bottomPixelIndex + c]);
                }
            }
        }

        // Save the modified image
        stbi_write_png(outputPath, width, height, channels, data, width * channels);

        // Free the image data
        stbi_image_free(data);

        return outputPath;
    }

    return "" ;

}
*/