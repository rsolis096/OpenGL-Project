#include "Camera.h"

//Default Camera Values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 80.0f;

Camera::Camera() : m_Yaw(YAW), m_Pitch(PITCH),
    m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_FOV(FOV)
{
    cameraPos = glm::vec3(0.0f, 3.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::updateCameraVectors()
{
    cameraFront.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    cameraFront.y = sin(glm::radians(m_Pitch));
    cameraFront.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    cameraFront = glm::normalize(cameraFront);
}

//Handle input that moves camera
void Camera::processKeyboard(float changeValue, unsigned int keyPressed)
{
    if (keyPressed == GLFW_KEY_S || keyPressed == GLFW_KEY_W)
        cameraPos += changeValue * cameraFront;
    else if (keyPressed == GLFW_KEY_D || keyPressed == GLFW_KEY_A)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * changeValue;
    else if (keyPressed == GLFW_KEY_SPACE)
        cameraPos.y += 0.05;
    else if (keyPressed == GLFW_KEY_LEFT_CONTROL)
        cameraPos.y -= 0.05;
}

void Camera::processMouse(double xpos, double ypos, GLboolean constrainPitch)
{
    xpos *= (double)m_MouseSensitivity;
    ypos *= (double)m_MouseSensitivity;

    //Add the offset values to the globally declared pitch and yaw values:
    m_Yaw += (float)xpos;
    m_Pitch += (float)ypos;

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

void Camera::processScroll(double xoffset, double yoffset)
{
    m_FOV -= (float)yoffset;
    //Max narrowness
    if (m_FOV < 1.0f)
        m_FOV = 1.0f;
    //max wideness
    if (m_FOV > 120.0f)
        m_FOV = 120.0f;
}

void Camera::setMovementSpeed(float newSpeed)
{
    m_MovementSpeed = newSpeed;
}

float Camera::getMovementSpeed()
{
    return m_MovementSpeed;
}

float Camera::getFOV()
{
    return m_FOV;
}

