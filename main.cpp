//#include <glad/glad.h> //Opengl functions
#include "DebugUtils.h"
#include <GLFW/glfw3.h> //Window functions

#include "Vendors/imgui/imgui.h"
#include "Vendors/imgui/imgui_impl_glfw.h"
#include "Vendors/imgui/imgui_impl_opengl3.h"

#include <iostream>


#include "PhysicsWorld.h"

#include "Texture.h"
#include "GUI.h"
#include "ShadowMap.h"
#include "Model.h"

#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "Model.h"

#include "LightController.h"




//Matrix Multiplication
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// settings
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool cursorLocked = true;
bool isWindowHidden = true;
double lastKeyPressTime = 0.0;
double debounceThreshold = 0.2; // Adjust this value based on your needs
double mouseX = 0, mouseY = 0; //Save cursor position when gui is opened
float xpos = 0, ypos = 0;


//Initialize Camera
Camera* myCamera = new Camera();

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout << "resize window called" << std::endl;
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void updateCamera(Shader& lightingShader, glm::mat4& view, glm::mat4& projection)
{
    lightingShader.use();

    //View Matrix (Do after camera)
    view = myCamera->GetViewMatrix();

    //Projection Matrix (fov change with scroll wheel) last value changes render distance
    projection = glm::perspective(glm::radians(myCamera->getFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1500.0f);

    //Send View and Projection matrices to shader (for camera)]]
    
    lightingShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.    

    lightingShader.setMat4("view", view);
    glCheckError();

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        myCamera->setMovementSpeed(50.0f);
    }
    else
    {
        myCamera->setMovementSpeed(2.5f);
    }


    //Return mouse movement when tab is pressed
    if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS)
    {
        double currentTime = glfwGetTime();
        if (currentTime - lastKeyPressTime > debounceThreshold)
        {
            GUI::isWindowHidden = !GUI::isWindowHidden;
            if (cursorLocked)
            {
                //Save previous mouse location
                mouseX = xpos;
                mouseY = ypos;
                //Unlock mouse
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                cursorLocked = false;
                //Set mouse to center
                glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);
            }
            else
            {
                //Place mouse on middle of screen
                glfwSetCursorPos(window, mouseX, mouseY);
                //Lock mouse
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursorLocked = true;
                //Reset last position for mouse
                mouseX = 0;
                mouseY = 0;

            }
            lastKeyPressTime = currentTime; // Update debounce timer
        }
    }

    //Mouse Input
    if (cursorLocked)
    {
        const float cameraSpeed = myCamera->getMovementSpeed() * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            myCamera->processKeyboard(cameraSpeed, GLFW_KEY_W);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            myCamera->processKeyboard(-1.0f * cameraSpeed, GLFW_KEY_S);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            myCamera->processKeyboard(-1.0f * cameraSpeed, GLFW_KEY_A);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            myCamera->processKeyboard(cameraSpeed, GLFW_KEY_D);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            myCamera->processKeyboard(cameraSpeed, GLFW_KEY_SPACE);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            myCamera->processKeyboard(cameraSpeed, GLFW_KEY_LEFT_CONTROL);
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    //Only change camera if cursor is locked
    if (cursorLocked)
    {
        //Cursor is locked, save current mouse positon
        xpos = xposIn;
        ypos = yposIn;
        //If this call is immedietly after unlock/lock, set mouse to previous position before unlock (mouseX and mouseY)
        if (mouseX != 0 && mouseY != 0)
        {
            xpos = static_cast<float>(mouseX);
            ypos = static_cast<float>(mouseY);
            mouseX = 0;
            mouseY = 0;
        }
        //if this call is done during cursor lock, set mouse position to new xposIn, yposIn
        else
        {
            xpos = static_cast<float>(xposIn);
            ypos = static_cast<float>(yposIn);
        }

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        myCamera->processMouse(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    myCamera->processScroll(xoffset, yoffset);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
unsigned int quadVAO = 0;
unsigned int quadVBO;

void RenderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

int main()
{
    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //OpenGL Version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Use CORE profile of OpenGL (Modern profile with deprecated features removed)
    // Set desired depth buffer size
    glfwWindowHint(GLFW_DEPTH_BITS, 32);

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

    //Enable mouse input (This disables the mouse and locks it to screen)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers (Initialize GLAD)
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    //Gamma Correction
    //glEnable(GL_FRAMEBUFFER_SRGB);
    
    //Enable this to only render front facing polygons (for performance)
    glEnable(GL_CULL_FACE);

    Scene myScene(myCamera);

    //GENERATE INITIAL SCENE (ALL OF THESE CAN BE CHANGED IN REAL TIME)
    myScene.addObject(new Model("resources/objects/dragon/dragon.obj"));
    myScene.addObject(new Cube("Assets/container2.png", "Assets/container2_specular.png"));
    myScene.addObject(new Sphere("Assets/globe.jpg", "Assets/globe.jpg"));
    myScene.addObject(new Plane());
    myScene.m_PhysicsWorld->addObject(myScene.m_SceneObjects[0]);
    myScene.m_PhysicsWorld->addObject(myScene.m_SceneObjects[1]);
    myScene.m_PhysicsWorld->addObject(myScene.m_SceneObjects[2]);    
    myScene.m_PhysicsWorld->addObject(myScene.m_SceneObjects[3]);
    
    glm::vec3 spotLightPos1 = glm::vec3(4.0f, 3.0f, -17.0f);
    glm::vec3 spotLightDir1 = glm::vec3(-20.0f, -5.0f, 45.0f);
    spotLightDir1 = glm::normalize(spotLightDir1 - spotLightPos1);

    glm::vec3 spotLightPos2 = glm::vec3(10.0f, 3.0f, 0.0f);
    glm::vec3 spotLightDir2 = glm::vec3(-4.50f, -3.0f, 14.0f);
    spotLightDir2 = glm::normalize(spotLightDir2 - spotLightPos2);

    glm::vec3 spotLightPos3 = glm::vec3(-14.0f, 3.0f, -13.0f);
    glm::vec3 spotLightDir3 = glm::vec3(0.0f, 0.0f, 1.0f);
    spotLightDir3 = glm::normalize(spotLightDir3 - spotLightPos3);

    myScene.m_LightController->addSpotLight(spotLightPos1, spotLightDir1);
    myScene.m_LightController->addSpotLight(spotLightPos2, spotLightDir2);
    myScene.m_LightController->addSpotLight(spotLightPos3, spotLightDir3);

    //Cube
    myScene.m_SceneObjects[1]->setPosition(glm::vec3(2.0f, 1.0f, 1.0));
    myScene.m_SceneObjects[1]->setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    //Sphere
    myScene.m_SceneObjects[2]->setPosition(glm::vec3(-2.0f, 0.5f, -1.0f));
    //Model
    myScene.m_SceneObjects[0]->setPosition(glm::vec3(-3.0f, 0.0f, 14.0f));
    myScene.m_SceneObjects[0]->setScale(glm::vec3(0.01f));

    //Initialize the GUI
    GUI myGUI(window, myScene);

    //Initialize View and Projection (These are changed in updateCamera() )
    glm::mat4 view;
    glm::mat4 projection;

    myScene.mainCamera = myCamera;

    glCheckError();

    //Main loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        view = myCamera->GetViewMatrix();
        updateCamera(*myScene.lightingShader, view, projection);
        //Use this to get framerate info
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        myScene.fps = 1 / deltaTime;
        //Rendering Starts Here
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //sets the clear color for the color buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //SHADOW PASS
        myScene.m_ShadowMap->ShadowPass();
        
        /**/
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //Point Light Lamp (light spheres)
        myScene.pointLightShader->use();
        myScene.pointLightShader->setMat4("projection", projection);
        myScene.pointLightShader->setMat4("view", view);

        //Main Shader
        myScene.lightingShader->use();
        myScene.lightingShader->setMat4("projection", projection);
        myScene.lightingShader->setMat4("view", view);
        myScene.lightingShader->setVec3("viewPos", myCamera->cameraPos);

        glUseProgram(myScene.lightingShader->ID);
        glUniformMatrix4fv(
            glGetUniformLocation(myScene.lightingShader->ID, "lightSpaceMatrices"),
            myScene.m_ShadowMap->getLightSpaceMatrices().size(),
            GL_FALSE, 
            glm::value_ptr(myScene.m_ShadowMap->getLightSpaceMatrices()[0])
        );

        myScene.drawScene(deltaTime, projection);
        // END OF LIGHTING PASS
        

        // render Depth map to quad for visual debugging
        // ---------------------------------------------

        /*
        myScene.m_ShadowMap->debugDepthShader.use();
        myScene.m_ShadowMap->debugDepthShader.setFloat("near_plane", myScene.m_LightController->m_SpotLights[1]->getNearPlane());
        myScene.m_ShadowMap->debugDepthShader.setFloat("far_plane", myScene.m_LightController->m_SpotLights[1]->getFarPlane());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, myScene.m_ShadowMap->getDepthMapID(1));
        updateCamera(myScene.m_ShadowMap->debugDepthShader, view, projection);
        RenderQuad();
        */

        glCheckError();

        //Ready the GUI in the background
        myGUI.displayWindow();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);//swap back frame buffer with front frame buffer.
        glfwPollEvents();
    }

    //Delete everything
    myScene.m_PhysicsWorld->removeAllObjects();
    myScene.removeAllObjects();


    //imgui: terminate
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    return 0;
}




/*
* Short Term Goals:
*   - Refactor the code to work more fluidly with eachother, less back and forth
*   - Remove as many hard coded situations as possible and give the user freedom
*       - Adding / removing light sources
*       - Object identities and names for more convenient and personalized editing
*       - Enable a flash light option (only 1 flashlight)
*   - Make some tests
*/