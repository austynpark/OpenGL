/* Start Header -------------------------------------------------------
File Name: Graphics.h
Purpose: Main System Handler, Including GLFW, glad, scene init, and call render function of scene.
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Scene.h"

// Include GLAD
#include <glad/glad.h>

// Include GLFW
#include <GLFW/glfw3.h>

namespace OG
{
    class Model;

	class Graphics
	{
	public:
        Graphics();
        ~Graphics();

        /// <summary>
        /// // Create Window, Load OpenGL function, Initialize the scene to use for rendering objects
        /// </summary>
        /// <returns>return false when initialize setting fails</returns>
        bool Init();
        void Update(double dt);
        void CleanUp();

        GLFWwindow* GetWindow() { return window; }
        Scene* GetScene() { return scene; }
        //static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        //static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    private:
        GLFWwindow* window = nullptr;
        Scene* scene = nullptr;
        const int windowWidth = 1380;
        const int windowHeight = 600;

        float lastX = windowWidth / 2.0f;
        float lastY = windowHeight / 2.0f;
        bool firstMouse = true;
	};
    extern Graphics* GRAPHICS;
}

#endif // !GRAPHICS_H
