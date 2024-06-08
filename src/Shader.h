#pragma once

#include "Camera.h" // include glad to get all the required OpenGL headers
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader
{
public:
    // the program ID
    unsigned int m_ProgramId;

    // constructor reads and builds the shader
    Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr);
    Shader();
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, const bool value) const;
    void setInt(const std::string& name, const int value) const;
    void setFloat(const std::string&, const float value) const;
    void setMat4(const std::string& name, glm::mat4& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;

private:
    void checkCompileErrors(const unsigned int id, const std::string& type, const std::string& path);
    void compileShader(const char* shader_path, const char* type, unsigned int program_id);
};