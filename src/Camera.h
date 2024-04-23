#pragma once

#include <glad/glad.h>
#include "DebugUtils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <vector>

class Camera
{
public:

    //glm::vec3 direction;
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 worldUp;

    Camera();

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();
    void processKeyboard(float changeValue, unsigned int keyPressed);
    void processMouse(double xpos, double ypos, GLboolean constrainPitch = true); 
    void processScroll(double xoffset, double yoffset);
    void setMovementSpeed(float newSpeed);
    float getMovementSpeed();
    float getFOV();

private:

    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_FOV;
    float m_Yaw;
    float m_Pitch;

    void updateCameraVectors();

};
