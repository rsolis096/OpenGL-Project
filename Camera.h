//Matrix Multiplication
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <vector>

//Default Camera Values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 60.0f;

class Camera
{
public:

    glm::vec3 direction;
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 worldUp;

    float m_MovementSpeed;
    float m_MouseSensitivity;
    float m_FOV;
    float m_Yaw;
    float m_Pitch;


    // constructor with vectors
    
    Camera() 
        : direction(glm::vec3(0.0f, 0.0f, -1.0f)), m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_FOV(FOV)
    {
        cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        cameraFront = direction;
        m_Yaw = YAW;
        m_Pitch = PITCH;
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    

    void processKeyboard(float changeValue, unsigned int keyPressed)
    {
        if(keyPressed == GLFW_KEY_S  || keyPressed == GLFW_KEY_W)
            cameraPos += changeValue * cameraFront;
        else
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * changeValue;
    }

    void processMouse(double xpos, double ypos, GLboolean constrainPitch = true)
    {
        const float sensitivity = 0.1f;
        xpos *= sensitivity;
        ypos *= sensitivity;

        //Add the offset values to the globally declared pitch and yaw values:
        m_Yaw += xpos;
        m_Pitch += ypos;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (m_Pitch > 89.0f)
                m_Pitch = 89.0f;
            if (m_Pitch < -89.0f)
                m_Pitch = -89.0f;
        }


        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();

    }

    void processScroll(double xoffset, double yoffset)
    {
        m_FOV -= (float)yoffset;
        if (m_FOV < 1.0f)
            m_FOV = 1.0f;
        if (m_FOV > 45.0f)
            m_FOV = 45.0f;
    }

private:
    void updateCameraVectors()
    {
        direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        direction.y = sin(glm::radians(m_Pitch));
        direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        cameraFront = glm::normalize(direction);
    }

};
