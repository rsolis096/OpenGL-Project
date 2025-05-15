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
    m_LookFrom = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_LookAt = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::vec3 dir = glm::normalize(m_LookAt - m_LookFrom);
    m_Pitch = glm::degrees(asin(dir.y));
    m_Yaw =  glm::degrees(atan2(dir.z, dir.x));
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(m_LookFrom, m_LookFrom + m_LookAt, cameraUp);
}
//Handle input that moves camera
void Camera::processKeyboard(float changeValue, unsigned int keyPressed)
{
    if (keyPressed == GLFW_KEY_S || keyPressed == GLFW_KEY_W)
        m_LookFrom += changeValue * m_LookAt;
    else if (keyPressed == GLFW_KEY_D || keyPressed == GLFW_KEY_A)
        m_LookFrom += glm::normalize(glm::cross(m_LookAt, cameraUp)) * changeValue;
    else if (keyPressed == GLFW_KEY_SPACE)
        m_LookFrom.y += 0.05;
    else if (keyPressed == GLFW_KEY_LEFT_CONTROL)
        m_LookFrom.y -= 0.05;
}

void Camera::processMouse(double xoffset, double yoffset, GLboolean constrainPitch)
{
    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    m_LookAt.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_LookAt.y = sin(glm::radians(m_Pitch));
    m_LookAt.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_LookAt = glm::normalize(m_LookAt);
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

