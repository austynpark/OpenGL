#ifndef GLAPPLICATION_H
#define GLAPPLICATION_H

#include "Scene.h"

// Include GLAD
#include <glad/glad.h>

// Include GLFW
#include <GLFW/glfw3.h>

namespace OG
{
    class Model;
    class GLApp
    {
    public:
        GLApp();
        ~GLApp();

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
}

#endif // !GLAPPLICATION_H

