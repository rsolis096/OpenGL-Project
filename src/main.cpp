//#include <glad/glad.h> //Opengl functions
#include "DebugUtils.h"

#include <iostream>
#include <GLFW/glfw3.h> //Window functions

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


#include "PhysicsWorld.h"

#include "Texture.h"
#include "GUI.h"
#include "ShadowMap.h"
#include "Model.h"

#include "Model.h"

//Contains includes for all light types
#include "LightController.h"

//Matrix Multiplication
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// settings
unsigned int SCR_WIDTH = 900;
unsigned int SCR_HEIGHT = 560;
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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //Return mouse movement when tilde is pressed
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

    //Mouse Input (input disabled when imgui menu is opened)
    if (cursorLocked)
    {
        float cameraSpeed = myCamera->getMovementSpeed() * deltaTime;
        //Forward
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            myCamera->processKeyboard(cameraSpeed, GLFW_KEY_W);
        //Backward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            myCamera->processKeyboard(-1.0f * cameraSpeed, GLFW_KEY_S);
        //Left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            myCamera->processKeyboard(-1.0f * cameraSpeed, GLFW_KEY_A);
        //Right
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            myCamera->processKeyboard(cameraSpeed, GLFW_KEY_D);
        //Up
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            myCamera->processKeyboard(cameraSpeed, GLFW_KEY_SPACE);
        //Down
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            myCamera->processKeyboard(cameraSpeed, GLFW_KEY_LEFT_CONTROL);
        //Movement Speed
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            myCamera->setMovementSpeed(50.0f);
        else
            myCamera->setMovementSpeed(2.5f);
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

//GLFW scroll_callback function sends scroll input to camera method
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    myCamera->processScroll(xoffset, yoffset);
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

GLFWwindow* setupWindow()
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
        return nullptr;
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
        return nullptr;
    }
    glEnable(GL_DEPTH_TEST);


    

    //Gamma Correction (looks awful)
    //glEnable(GL_FRAMEBUFFER_SRGB);
    
    //Enable this to only render front facing polygons (for performance)
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);


    return window;
}

void demoScene(Scene& demoScene)
{
    //GENERATE INITIAL SCENE (ALL OF THESE CAN BE CHANGED IN REAL TIME)
    demoScene.addObject(new Model("Assets/models/dragon/dragon.obj"));
    demoScene.addObject(new Cube("Assets/container2.png", "Assets/container2_specular.png"));
    demoScene.addObject(new Sphere("Assets/globe.jpg", "Assets/globe.jpg"));
    //demoScene.addObject(new Cube());
    demoScene.addObject(new Plane());
    demoScene.m_PhysicsWorld->addObject(demoScene.m_SceneObjects[0]);
    demoScene.m_PhysicsWorld->addObject(demoScene.m_SceneObjects[1]);
    demoScene.m_PhysicsWorld->addObject(demoScene.m_SceneObjects[2]);
    demoScene.m_PhysicsWorld->addObject(demoScene.m_SceneObjects[3]);

    glm::vec3 spotLightPos1 = glm::vec3(4.0f, 3.0f, -17.0f);
    glm::vec3 spotLightDir1 = glm::vec3(-20.0f, -5.0f, 45.0f);
    spotLightDir1 = glm::normalize(spotLightDir1 - spotLightPos1);

    glm::vec3 spotLightPos2 = glm::vec3(10.0f, 3.0f, 0.0f);
    glm::vec3 spotLightDir2 = glm::vec3(-4.50f, -3.0f, 14.0f);
    spotLightDir2 = glm::normalize(spotLightDir2 - spotLightPos2);

    glm::vec3 spotLightPos3 = glm::vec3(-14.0f, 3.0f, -13.0f);
    glm::vec3 spotLightDir3 = glm::vec3(0.0f, 0.0f, 1.0f);
    spotLightDir3 = glm::normalize(spotLightDir3 - spotLightPos3);

    demoScene.m_LightController->addSpotLight(spotLightPos1, spotLightDir1);
    demoScene.m_LightController->addSpotLight(spotLightPos2, spotLightDir2);
    demoScene.m_LightController->addSpotLight(spotLightPos3, spotLightDir3);
    demoScene.m_LightController->addSpotLight(spotLightPos3, spotLightDir3);

    demoScene.m_LightController->addPointLight(glm::vec3(0.0f,10.0f,0.0f));

    //Cube
    demoScene.m_SceneObjects[1]->setPosition(glm::vec3(4.0f, 3.0f, 0.0));
    demoScene.m_SceneObjects[1]->setScale(glm::vec3(1.0f, 1.0f, 11.0f));
    //Sphere
    demoScene.m_SceneObjects[2]->setPosition(glm::vec3(-2.0f, 0.5f, -1.0f));
    //Model
    demoScene.m_SceneObjects[0]->setPosition(glm::vec3(-3.0f, 0.0f, 14.0f));
    demoScene.m_SceneObjects[0]->setScale(glm::vec3(0.01f));
}

int main()
{
    //Initialize GLFW and create window
    GLFWwindow* window = setupWindow();

    //Create the scene
    Scene myScene(myCamera);
    //Currently, this must be called
    demoScene(myScene);

    //Initialize the GUI
    GUI myGUI(window, myScene);

    //Initialize View and Projection (These are changed in updateCamera() )
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);;

    glCheckError();

    //Main loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //Update Camera
        updateCamera(*myScene.lightingShader, view, projection);

        //Use this to get framerate info
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        myScene.fps = 1 / deltaTime;

        //Rendering Starts Here
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //sets the clear color for the color buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //SHADOW PASS (Only update upon launch and press of "=" key, see func definition) 
        myScene.m_ShadowMap->ShadowPass();
        
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        myScene.drawScene(deltaTime, projection, view);

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