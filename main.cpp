#include <glad/glad.h> //Opengl functions
#include <GLFW/glfw3.h> //Window functions
#include <iostream>
#include "Shader.h"
#include "Texture.h"
#include "Vendors/stb_image.h"
#include "Camera.h"
#include "Cube.h"
#include "Sphere.h"

//Matrix Multiplication
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;


float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//Initialize Camera
Camera myCamera;

//Lighting position
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout << "resize window called" << std::endl;
    glViewport(0, 0, width, height);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //Mouse Input
    //const float movementSpeed = 0.05f; // adjust accordingly
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        myCamera.processInput(cameraSpeed, GLFW_KEY_W);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        myCamera.processInput(-1.0f * cameraSpeed, GLFW_KEY_S);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        myCamera.processInput(-1.0f * cameraSpeed, GLFW_KEY_A);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        myCamera.processInput(cameraSpeed, GLFW_KEY_D);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    myCamera.processMouse(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    myCamera.processScroll(xoffset, yoffset);
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //OpenGL Version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Use CORE profile of OpenGL (Modern profile with deprecated features removed)

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //Enable cursor and scroll wheel input
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //Enable mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



    // glad: load all OpenGL function pointers (Initialize GLAD)
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Enable Z-Buffer
    glEnable(GL_DEPTH_TEST);

    //Compile and link shaders
    //Shader myShader("shader.vert", "shader.frag");
    Shader lightingShader("1.colors.vert", "1.colors.frag");
    Shader lightCubeShader("1.light_cube.vert", "1.light_cube.frag");

    float vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f, 
     0.5f, -0.5f,  0.5f, 
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f, 

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f, 
     0.5f,  0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f, 
     0.5f, -0.5f,  0.5f, 
     0.5f,  0.5f,  0.5f,  

    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f, -0.5f,  

    -0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f
    };

    float normals[] = {
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,

        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,

        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,

         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,

         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,

         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f
    };

    Cube *myCube = new Cube(vertices, normals, lightingShader.ID);
    Cube *lightSource = new Cube(vertices, normals, lightCubeShader.ID);
    //Sphere* mySphere = new Sphere(lightCubeShader.ID);

    // render loop 
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Enable this line for wireframe display

    //Initialize MVP
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        //Use this to get framerate info
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Rendering Starts Here
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //sets the clear color for the color buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//The back buffer currently only contains the color buffer, this clears and updates it with the colour specified by glClearColor.

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 0.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", myCamera.cameraPos);




        //View Matrix
        view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(myCamera.cameraPos, myCamera.cameraPos + myCamera.cameraFront, myCamera.cameraUp);
        //Projection Matrix (fov change with scroll wheel)
        projection = glm::perspective(glm::radians(myCamera.fov), 1280.0f / 720.0f, 0.1f, 150.0f);
        //Send View and Projection matrices to shader (for camera)]]
        lightingShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.    
        lightingShader.setMat4("view", view);
       
        //World Transformation
        model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        //Render Cube
        myCube->bind();
        myCube->render();

        //Draw Lamp Object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        //Make the lamp smaller
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);
        lightSource->bind();
        lightSource->render();

        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);//swap back frame buffer with front frame buffer.
        glfwPollEvents();

    }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    delete myCube;
    myCube = nullptr;
    //delete mySphere;
    //mySphere = nullptr;



    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}



