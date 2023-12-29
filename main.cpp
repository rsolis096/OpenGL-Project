#include <glad/glad.h> //Opengl functions
#include <GLFW/glfw3.h> //Window functions

#include "Vendors/imgui/imgui.h"
#include "Vendors/imgui/imgui_impl_glfw.h"
#include "Vendors/imgui/imgui_impl_opengl3.h"


#include <iostream>
#include "Shader.h"
#include "Texture.h"
#include "GUI.h"

//Objects

#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "SkyBox.h"
#include "Scene.h"
#include "Model.h"

#include "LightController.h"

//Physics
#include "PhysicsWorld.h"


//Matrix Multiplication
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;


float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool cursorLocked = true;
bool isWindowHidden = true;

//For key input (debounce timer)
double lastKeyPressTime = 0.0;
double debounceThreshold = 0.2; // Adjust this value based on your needs

//Save cursor location for unlock/lock
double mouseX = 0, mouseY = 0;
float xpos = 0, ypos = 0;


//Initialize Camera
Camera* myCamera = new Camera();

//Lighting position
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout << "resize window called" << std::endl;
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
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
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
    glEnable(GL_FRAMEBUFFER_SRGB);
    //Enable this to only render front facing polygons (for performance)
    glEnable(GL_CULL_FACE);

    //Defaults, not needed to be specified (just for reference)
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);

    Scene myScene(myCamera);
    //Create Scene Manager and some default objects
    myScene.addObject(new Model("resources/objects/dragon/dragon.obj"));
    myScene.addObject(new Cube("Assets/container2.png", "Assets/container2_specular.png"));
    myScene.addObject(new Sphere("Assets/globe.jpg", "Assets/globe.jpg"));
    myScene.addObject(new Plane("Assets/woodparquet_93_basecolor-2K.png", "Assets/woodparquet_93_basecolor-2K.png"));
    myScene.m_PhysicsWorld->addObject(myScene.m_SceneObjects[1]);
    myScene.m_PhysicsWorld->addObject(myScene.m_SceneObjects[2]);
    myScene.m_PhysicsWorld->addObject(myScene.m_SceneObjects[3]);
    myScene.createLightController();
    myScene.m_LightController->addPointLight();
    myScene.m_LightController->addPointLight();
    myScene.m_LightController->addSpotLight();
    myScene.m_LightController->m_PointLights[0]->setLightPos(glm::vec3(-51.0f, -51.0f, -5.0f));
    myScene.m_LightController->m_PointLights[1]->setLightPos(glm::vec3(1.0f, 5.0f, 0.0f));

    //Create the SkyBox
    SkyBox* mySkyBox = new SkyBox(myScene.cubeMapShader, myCamera);
    mySkyBox->setCubeMapTexture("Assets/skybox/rocky_ridge_puresky2.png");

    //Initialize the GUI
    GUI myGUI(window, myScene);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Enable this line for wireframe display

    //Initialize View and Projection (These are changed in updateCamera() )
    glm::mat4 view;
    glm::mat4 projection;


    //Set window size of ImGUI window
    //ImGui::SetNextWindowSize(ImVec2(100, 75)); // Set the desired width and height

    myScene.mainCamera = (myCamera);

    //Main loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        //Use this to get framerate info
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        myScene.fps = 1 / deltaTime;

        //Rendering Starts Here
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //sets the clear color for the color buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        myScene.lightingShader.use();
        myScene.lightingShader.setVec3("viewPos", myCamera->cameraPos);
        // Update the camera
        updateCamera(myScene.lightingShader, view, projection);
        //Draw Scene
        myScene.drawScene(projection, deltaTime);
        //Draw SkyBox
        mySkyBox->draw(projection);

        //GUI should always be drawn last
        myGUI.displayWindow();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);//swap back frame buffer with front frame buffer.
        glfwPollEvents();
    }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------


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