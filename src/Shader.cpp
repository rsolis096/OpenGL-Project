#include "Shader.h"

Shader::Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path)
{
    //Create Shader Program
    m_ProgramId = glCreateProgram();
    //Compile given shaders
    compileShader(vertex_path, "VERTEX", m_ProgramId);
    if(geometry_path != nullptr)
        compileShader(geometry_path, "GEOMETRY", m_ProgramId);
    compileShader(fragment_path, "FRAGMENT", m_ProgramId);
}

void Shader::compileShader(const char* shader_path, const char* type, unsigned int m_ProgramId)
{
    std::cout << "COMPILING SHADER: " << shader_path << "\n";
    std::string shader_content;
    std::ifstream shader_file(shader_path);
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        std::stringstream shader_stream;
        // read file's buffer contents into streams
        shader_stream << shader_file.rdbuf();
        // close file handlers
        shader_file.close();
        // convert stream into string
        shader_content = shader_stream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << "\n";
    }

    const char* shader_code = shader_content.c_str();

    //initialize shader reference id
    GLuint shader_id = 0;
    if (strcmp(type,"FRAGMENT") == 0) {
        shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    }
    else if (strcmp(type, "VERTEX") == 0) {
        shader_id = glCreateShader(GL_VERTEX_SHADER);
    }
    else if (strcmp(type, "GEOMETRY") == 0) {
        shader_id = glCreateShader(GL_GEOMETRY_SHADER); // Corrected to GL_GEOMETRY_SHADER
    }
    else {
        std::cout << "ERROR::SHADER::INVALID SHADER TYPE\n";
    }

     //Create shader of type GL_VERTEX_SHADER
    glShaderSource(shader_id, 1, &shader_code, nullptr); //Attach shader to shader source code
    glCompileShader(shader_id); //Compile shader
   
    //Links output of vertex shader to input of fragment shader
    glAttachShader(m_ProgramId, shader_id); //Attach compiled shader object to a shader program (shaderProgram)
    glLinkProgram(m_ProgramId); 

    checkCompileErrors(shader_id, type, shader_path);
    checkCompileErrors(m_ProgramId, "PROGRAM", "PROGRAM");

    //Discard shader object
    glDeleteShader(shader_id);
    glCheckError();
}

Shader::Shader() = default;

void Shader::use()
{
    glUseProgram(m_ProgramId);
}

void Shader::checkCompileErrors(const unsigned int id, const std::string& type, const std::string& path)
{
    int  success;
    char infoLog[512];

    //CHeck the compilation of the shaders
    //This essentially checks for syntax errors like missing semi-colons in the fragment and vertex shaders
    if (type != "PROGRAM")
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success); //Check compilation success
        glGetShaderInfoLog(id, 512, NULL, infoLog); //If error message exists, write it to infoLog

        if (!success)
        {
            std::cout << "SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << "\n" << path << "\n";
        }
    }

    //Check for linking success
    //Linking is the process of combining the shaders into the rendering pipeline
    //Linking ensures the output of one shader stages matches the input of the next
    //Just because compilation succeeded does not imply Linking will also succeed
    else if (type == "PROGRAM")
    {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        glGetProgramInfoLog(id, 512, nullptr, infoLog);

        if (!success) {
            std::cout << "Program Linking Failed: " << infoLog << "\n" << path << "\n";
        }
    }
}

void Shader::setBool(const std::string& name, const bool value) const
{
    glUniform1i(glGetUniformLocation(m_ProgramId, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, const int value) const
{
    // Set the value of the uniform
    glUseProgram(m_ProgramId);
    glUniform1i(glGetUniformLocation(m_ProgramId, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, const float value) const
{
    glUniform1f(glGetUniformLocation(m_ProgramId, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_ProgramId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_ProgramId, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const float x, const float y, const float z) const
{
    glUniform3f(glGetUniformLocation(m_ProgramId, name.c_str()), x, y, z);
}