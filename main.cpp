#include "Vendors/imgui/imgui.h"
#include "Vendors/imgui/imgui_impl_glfw.h"
#include "Vendors/imgui/imgui_impl_opengl3.h"

#include <glad/glad.h> //Opengl functions
#include <GLFW/glfw3.h> //Window functions
#include <iostream>
#include "Shader.h"
#include "Texture.h"
#include "Vendors/stb_image.h"
#include "Camera.h"

//Objects
#include "PointLight.h"

//Matrix Multiplication
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;


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

void updateCamera(Shader& lightingShader, glm::mat4& view, glm::mat4& projection)
{
    //View Matrix (Do after camera)
    view = myCamera.GetViewMatrix();
    //Projection Matrix (fov change with scroll wheel)
    projection = glm::perspective(glm::radians(myCamera.m_FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 150.0f);
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

    //Return mouse movement when tab is pressed
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        double currentTime = glfwGetTime();
        if (currentTime - lastKeyPressTime > debounceThreshold)
        {
            isWindowHidden = !isWindowHidden;
            if (cursorLocked)
            {
                //Save previous mouse location
                mouseX = xpos;
                mouseY = ypos;
                //Unlock mouse
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                cursorLocked = false;
                //Set mouse to center
                glfwSetCursorPos(window, SCR_WIDTH/2, SCR_HEIGHT/2);
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
    //const float movementSpeed = 0.05f; // adjust accordingly
    if (cursorLocked)
    {
        const float cameraSpeed = 2.5 * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            myCamera.processKeyboard(cameraSpeed, GLFW_KEY_W);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            myCamera.processKeyboard(-1.0f * cameraSpeed, GLFW_KEY_S);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            myCamera.processKeyboard(-1.0f * cameraSpeed, GLFW_KEY_A);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            myCamera.processKeyboard(cameraSpeed, GLFW_KEY_D);
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

        myCamera.processMouse(xoffset, yoffset);
    }
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    //Enable Z-Buffer
    glEnable(GL_DEPTH_TEST);

    //Compile and link shaders
    //Shader myShader("shader.vert", "shader.frag");
    Shader lightingShader("1.colors.vert", "1.colors.frag");
    Shader lightCubeShader("1.light_cube.vert", "1.light_cube.frag");

    std::vector<float> vertices = {
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

    std::vector<float> normals = {
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

    std::vector<float> texCoords = {
       0.0f, 0.0f,
       1.0f, 0.0f,
       1.0f, 1.0f,
       1.0f, 1.0f,
       0.0f, 1.0f,
       0.0f, 0.0f,

       0.0f, 0.0f,
       1.0f, 0.0f,
       1.0f, 1.0f,
       1.0f, 1.0f,
       0.0f, 1.0f,
       0.0f, 0.0f,

        1.0f, 0.0f,
       1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
         0.0f, 0.0f,
         1.0f, 0.0f,

        1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f,
          0.0f, 1.0f,
          0.0f, 0.0f,
          1.0f, 0.0f,

        0.0f, 1.0f,
          1.0f, 1.0f,
         1.0f, 0.0f,
         1.0f, 0.0f,
          0.0f, 0.0f,
         0.0f, 1.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
          1.0f, 0.0f,
          0.0f, 0.0f,
         0.0f, 1.0f
    };

    //Initialize objects
    PointLight* pointLight = new PointLight(vertices, normals, "cube", lightingShader, lightCubeShader);
    Cube* myCube = new Cube(vertices, normals, texCoords, "Assets/container2.png", "Assets/container2_specular.png", lightingShader);
    Cube* myCube2 = new Cube(vertices, normals, lightingShader);
    Sphere* mySphere = new Sphere(lightingShader);

    // render loop 
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Enable this line for wireframe display

    //Initialize MVP
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 colorSlider = glm::vec3(1.0f);

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    //Set window size of ImGUI window
    //ImGui::SetNextWindowSize(ImVec2(100, 75)); // Set the desired width and height

    //Initialize Light Properties (Defaults will be used)
    pointLight->initializeLight(myCamera.cameraPos, myCamera.cameraFront);



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

        //Point Light (initialize light sets light properties that may change every frame)
        //Change the color the light emits
        //pointLight->setDiffuse(glm::vec3(abs(sin(currentFrame)), abs(cos(currentFrame)), abs(sin(currentFrame))));
        pointLight->setDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));

        // Update the camera
        updateCamera(lightingShader, view, projection);


        //World Transformation (For Cube) - set the cube position to default
        model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);
        //Render Cube
        myCube->render();

        //Shift sphere and shrink (Note that this one does not have any special lighting)
        mySphere->translatePosition(glm::vec3(0.001f));
        mySphere->setDiffuse(glm::vec3(abs(sin(currentFrame)), abs(sin(currentFrame)), abs(sin(currentFrame))));
        mySphere->render();

        //Draw Lamp Object
        pointLight->renderLight(myCamera.cameraPos, projection, view);


        if (!isWindowHidden)
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Rendering
            // (Your code clears your framebuffer, renders your other stuff etc.)
            ImGui::Begin("OpenGL Project");
            ImGui::Text("Hello World");
            ImGui::ColorEdit3("color", &colorSlider[0]);
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);//swap back frame buffer with front frame buffer.
        glfwPollEvents();

    }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    delete myCube;
    myCube = nullptr;
    delete mySphere;
    mySphere = nullptr;

    //imgui: terminate
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();



    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}