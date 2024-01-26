#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile(vertexPath);
    std::ifstream fShaderFile(fragmentPath);
    glCheckError();
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    glCheckError();
    //Create vertex shader object
    unsigned int vertexShader; //initialize vertex shader reference id
    vertexShader = glCreateShader(GL_VERTEX_SHADER); //Create shader of type GL_VERTEX_SHADER
    glShaderSource(vertexShader, 1, &vShaderCode, NULL); //Attach shader to shader source code
    glCompileShader(vertexShader); //Compile vertexShader
    glCheckError();
    //Create fragmentShader object
    unsigned int fragmentShader; //initialize fragment shader reference id
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Create shader of type GL_FRAGMENT_SHADER
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL); //Attach shader to shader source code
    glCompileShader(fragmentShader); //compile fragmentShader
    glCheckError();
    //To use the recently compiled shaders we have to link them to a shader program object and then activate this shader program
    //Links output of vertex shader to input of fragment shader
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader); //Attach compiled shader object (vertexShader) to a shader program (shaderProgram)
    glCheckError();

    glAttachShader(ID, fragmentShader); //Attach compiled shader object (fragmentShader) to a shader program (shaderProgram)
    glCheckError();

    glLinkProgram(ID); //Link the two shaders, order matters. Always attach vertex shader before fragment shader to match rendering pipeline
    glCheckError();
    checkCompileErrors(vertexShader, "VERTEX", vertexPath);
    glCheckError();
    checkCompileErrors(fragmentShader, "FRAGMENT", fragmentPath);
    glCheckError();
    checkCompileErrors(ID, "PROGRAM", "PROGRAM");
    glCheckError();
    //The vertexShader and fragmentShader shader objects are already linked to the program object. Its ok to delete them.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glCheckError();
}

Shader::Shader()
{
    
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type, std::string path)
{
    int  success;
    char infoLog[512];

    //CHeck the compilation of the shaders
    //This essentially checks for syntax errors like missing semi-colons in the fragment and vertex shaders
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Check compilation success
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //If error message exists, write it to infoLog

        if (!success)
        {
            std::cout << "SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
            std::cout << path << std::endl;
        }
    }

    //Check for linking success
    //Linking is the process of combining the two shaders into the rendering pipeline
    //Linking ensures the output of one shader stages matches the input of the next
    //Just because compilation succeeded does not imply Linking will also succeed
    else if (type == "PROGRAM")
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        glGetProgramInfoLog(shader, 512, NULL, infoLog);

        if (!success) {
            std::cout << "Program Linking Failed: " << infoLog << std::endl;
            std::cout << path << std::endl;

        }
    }
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    // Set the value of the uniform
    glUseProgram(ID);
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setMat4(const std::string& name, glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}