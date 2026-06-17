#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "../DebugUtils.h"
#include <glad/glad.h>

class Shader;

class Framebuffer
{
public:
    Framebuffer() = default;
    Framebuffer(int width, int height);

    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    // Do Not Implement
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    void Bind() const;
    static void Unbind();
    void Destroy();
    void BindGenericTexture(GLuint texture, const std::string& name, Shader& shader);

    virtual ~Framebuffer();
    virtual void BindTextures(Shader& shader) const = 0;
    virtual void Resize(int width, int height);


    GLuint GetFBO() const { return m_fbo; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

protected:
    void CreateFramebuffer();
    void CheckComplete() const;
    virtual void DestroyAttachments() {};

protected:
    GLuint m_fbo = 0;

    int m_width = 0;
    int m_height = 0;
};