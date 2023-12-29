#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile(vertexPath);
    std::ifstream fShaderFile(fragmentPath);

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

    //Create vertex shader object
    unsigned int vertexShader; //initialize vertex shader reference id
    vertexShader = glCreateShader(GL_VERTEX_SHADER); //Create shader of type GL_VERTEX_SHADER
    glShaderSource(vertexShader, 1, &vShaderCode, NULL); //Attach shader to shader source code
    glCompileShader(vertexShader); //Compile vertexShader

    //Create fragmentShader object
    unsigned int fragmentShader; //initialize fragment shader reference id
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Create shader of type GL_FRAGMENT_SHADER
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL); //Attach shader to shader source code
    glCompileShader(fragmentShader); //compile fragmentShader

    //To use the recently compiled shaders we have to link them to a shader program object and then activate this shader program
    //Links output of vertex shader to input of fragment shader
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader); //Attach compiled shader object (vertexShader) to a shader program (shaderProgram)
    glAttachShader(ID, fragmentShader); //Attach compiled shader object (fragmentShader) to a shader program (shaderProgram)
    glLinkProgram(ID); //Link the two shaders, order matters. Always attach vertex shader before fragment shader to match rendering pipeline

    checkCompileErrors(vertexShader, "VERTEX");
    checkCompileErrors(fragmentShader, "FRAGMENT");
    checkCompileErrors(ID, "PROGRAM");

    //The vertexShader and fragmentShader shader objects are already linked to the program object. Its ok to delete them.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader()
{
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Check compilation success
    glGetShaderInfoLog(shader, 512, NULL, infoLog); //If error message exists, write it to infoLog

    if (type != "PROGRAM")
    {
        if (!success)
        {
            std::cout << "SHADER::"<<type<<"::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }
    //Check for linking success
    else if(type == "PROGRAM")
    {
        if (!success) {
            std::cout << "Shader Linking Failed: " << infoLog << std::endl;
        }
    }

}



void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setMat4(const std::string& name, glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}