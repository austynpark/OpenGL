/* Start Header -------------------------------------------------------
File Name: GLApplication.cpp
Purpose: Main Loop, Create Window using GLFW, Handle Input, Call every init and update(render) graphics.
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#include "ogpch.h"
#include "Graphics.h"

// Include GLEW
#include <glad/glad.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Local / project headers
#include "Scene.h"
#include "shader.hpp"


using namespace OG;
void processInput(GLFWwindow* window);
int main()
{
    std::unique_ptr<Graphics> graphics = std::make_unique<Graphics>();

    // Create Window, Load OpenGL function, Initialize the scene to use for rendering objects
    bool init_result = graphics->Init();

    if (!init_result)
        return 0;

    GLFWwindow* window = graphics->GetWindow();
    graphics->GetScene()->SetupImGui(window);

    double deltaTime = 0.0;
    double lastFrame = 0.0;

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        processInput(window);

        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        graphics->Update(deltaTime);
        graphics->GetScene()->DrawImGui(window);
        graphics->GetScene()->keyboardInput(window, static_cast<float>(deltaTime));

        // Swap buffers
        glfwSwapBuffers(window);
    }
  
    graphics->CleanUp();

    return 0;
}

// Check if the ESC key was pressed or the window was closed
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////