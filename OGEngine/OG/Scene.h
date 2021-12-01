/* Start Header -------------------------------------------------------
File Name: Scene.h
Purpose: Interface of Scene
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef SAMPLE3_2_FBO_SCENE_H
#define SAMPLE3_2_FBO_SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include "SceneManager.h"
//#include "VertexDataManager.h"


#define _GET_GL_ERROR   { GLenum err = glGetError(); std::cout << "[OpenGL Error] " << glewGetErrorString(err) << std::endl; }
namespace OG 
{
    class Scene
    {

    public:
        Scene();
        Scene(int windowWidth, int windowHeight);
        virtual ~Scene();

        // Public methods

        // Init: called once when the scene is initialized
        virtual int Init();

        // LoadAllShaders: This is the placeholder for loading the shader files
        virtual void LoadAllShaders();

        // Display : encapsulates per-frame behavior of the scene
        virtual int Display(double dt);

        // preRender : called to setup stuff prior to rendering the frame
        virtual int preRender(double dt);

        // Render : per frame rendering of the scene
        virtual int Render(double dt);

        // postRender : Any updates to calculate after current frame
        virtual int postRender(double dt);

        // cleanup before destruction
        virtual void CleanUp();

        virtual void setFramebuffer() = 0;

        // IMGUI stuff
        virtual void SetupImGui(GLFWwindow* pWindow) = 0;
        virtual void DrawImGui(GLFWwindow* pWindow) = 0;
        virtual void CleanupImGui() = 0;
        virtual void keyboardInput(GLFWwindow* pWindow, float dt) = 0;

        void SetWindowSize(int width, int height);
    protected:
        int _windowHeight, _windowWidth;

        // Common functionality for all scenes
    //    SceneManager                    _scnManager;
    //    std::vector<VertexDataManager>   VAOs;

    };

}

#endif //SAMPLE3_2_FBO_SCENE_H
