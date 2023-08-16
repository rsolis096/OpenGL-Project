#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

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

    // glad: load all OpenGL function pointers (Initialize GLAD)
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Compile and link shaders
    Shader myShader("shader.vert", "shader.frag");

    //Triangle Vertices
    /*
    float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };*/

    //Rectangle Vertices (USING EBO FOR INDICES)
    /*
    float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
    };*/

    //Triangle Vertices LEFT VBO[0] (USING EBO FOR INDICES)
    float verticesT1[] = {
     0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f, 0.0f, 0.5f, 1.0f   // top left
    };

    //Triangle Vertices RIGHT VBO[1] (USING EBO FOR INDICES)
    float verticesT2[] = {
     0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,  // bottom right
    -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f  // top left
    };

    /*
    unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
    };*/

    //Create a VertexArrayObjects VAO[2]
    unsigned int VAO[2];
    glGenVertexArrays(2, VAO);

    //Create a VertexBufferObjects (VBO[2]) 
    unsigned int VBO[2];
    glGenBuffers(2, VBO); //(Quantity of VBO's, Reference to VBO) One VBO for this triangle.

    //Get our vertex attributes pointers (location in vertex shader)
    unsigned int attribLocation = glGetAttribLocation(myShader.ID, "aPos");
    unsigned int attribLocation2 = glGetAttribLocation(myShader.ID, "aRGB");
    glEnableVertexAttribArray(attribLocation);
    glEnableVertexAttribArray(attribLocation2);


    //First Triangle
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesT1), verticesT1, GL_DYNAMIC_DRAW); //Copy user-defined data into the currently bound buffer. (currently bound buffer is VBO of type GL_ARRAY_BUFFER (This sends vertex data to GPU), //Use STATIC_DRAW because our triangle isnt moving/changing
    glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //( 0 for position vertex attribute (see vertex shader), size of vertex attribute (3 positions per vertex) so vec3, normalize=false, stride = 12,  start of buffer (void*)0
    glVertexAttribPointer(attribLocation2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //( 0 for position vertex attribute (see vertex shader), size of vertex attribute (3 positions per vertex) so vec3, normalize=false, stride = 12,  start of buffer (void*)0
    glEnableVertexAttribArray(attribLocation);
    glEnableVertexAttribArray(attribLocation2);


    //Second Triangle
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesT2), verticesT2, GL_DYNAMIC_DRAW); //Copy user-defined data into the currently bound buffer. (currently bound buffer is VBO of type GL_ARRAY_BUFFER (This sends vertex data to GPU), //Use STATIC_DRAW because our triangle isnt moving/changing
    glVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //( 0 for position vertex attribute (see vertex shader), size of vertex attribute (3 positions per vertex) so vec3, normalize=false, stride = 12,  start of buffer (void*)0
    glVertexAttribPointer(attribLocation2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float))); //( 0 for position vertex attribute (see vertex shader), size of vertex attribute (3 positions per vertex) so vec3, normalize=false, stride = 12,  start of buffer (void*)0
    glEnableVertexAttribArray(attribLocation);
    glEnableVertexAttribArray(attribLocation2);


    //Create EBO
    /*
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    */

    // render loop
    // -----------
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Enable this line for wireframe display
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render (render to back buffer)
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //sets the clear color for the color buffer
        glClear(GL_COLOR_BUFFER_BIT);//The back buffer currently only contains the color buffer, this clears and updates it with the colour specified by glClearColor.

        //Render Triangle
        myShader.use();

        //Update Triangle
        float timeValue = glfwGetTime();
        float redValue = sin(timeValue) / 2.0f + 0.5f;
        float greenValue = cos(timeValue) / 2.0f + 0.5f;
        float blueValue = 1.0 - greenValue;
        std::cout << redValue << " " << greenValue << std::endl;
        verticesT1[3] = redValue;
        verticesT1[10] = greenValue;
        verticesT1[17] = blueValue;

        verticesT2[10] = redValue;
        verticesT2[17] = greenValue;
        verticesT2[3] = blueValue;
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesT1), verticesT1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesT2), verticesT2);

        //Draw first triangle using data from the first VAO
        glBindVertexArray(VAO[0]); //BIND VAO. Specifies VAO 0 to be rendered
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //Draw first triangle using data from the Second VAO
        glBindVertexArray(VAO[1]); //BIND VAO. Specifies VAO 1 to be rendered 
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); //UNBIND VAO (at the moment, this step is redundant)

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);//swap back frame buffer with front frame buffer.
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}



