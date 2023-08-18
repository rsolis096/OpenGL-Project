#include <GLFW/glfw3.h> //Window functions
//Matrix Multiplication
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

//Constants


class Camera
{
public:

    glm::vec3 direction;
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float fov;
    float lastX;
    float lastY;
    bool firstMouse;
    float yaw;
    float pitch;

    Camera()
    {
        //Initialize camera vectors
        cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        //Initialize constants
        fov = 45.0f;
        lastX = 640.0f;
        lastY = 360.0f;
        firstMouse = true;
        yaw = -90.0f;
        pitch = 0.0f;
    }

    void processInput(float changeValue, unsigned int keyPressed)
    {
        if(keyPressed == GLFW_KEY_S  || keyPressed == GLFW_KEY_W)
            cameraPos += changeValue * cameraFront;
        else
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * changeValue;
    }

    void processMouse(double xpos, double ypos)
    {

        if (firstMouse) // initially set to true
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
        lastX = xpos;
        lastY = ypos;

        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        //Add the offset values to the globally declared pitch and yaw values:
        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

    }

    void processScroll(double xoffset, double yoffset)
    {
        fov -= (float)yoffset;
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 45.0f)
            fov = 45.0f;
    }

};