/* Start Header -------------------------------------------------------
File Name: Scene.cpp
Purpose: Interface of Scene
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#include "ogpch.h"
#include "Scene.h"

namespace OG 
{
    Scene::Scene() : _windowWidth(100), _windowHeight(100)
    {

    }

    Scene::Scene(int windowWidth, int windowHeight)
    {
        _windowHeight = windowHeight;
        _windowWidth = windowWidth;
    }

    Scene::~Scene()
    {
        CleanUp();
    }

    // Public methods

    // Init: called once when the scene is initialized
    int Scene::Init()
    {
        return -1;
    }

    // LoadAllShaders: This is the placeholder for loading the shader files
    void Scene::LoadAllShaders()
    {
        return;
    }


    // preRender : called to setup stuff prior to rendering the frame
    int Scene::preRender(double dt)
    {
        return -1;
    }

    // Render : per frame rendering of the scene
    int Scene::Render(double dt)
    {
        return -1;
    }

    // postRender : Any updates to calculate after current frame
    int Scene::postRender(double dt)
    {
        return -1;
    }

    // CleanUp : clean up resources before destruction
    void Scene::CleanUp()
    {
        return;
    }

    void Scene::SetWindowSize(int width, int height)
    {
        _windowWidth = width;
        _windowHeight = height;
    }

    // Display : Per-frame execution of the scene
    int Scene::Display(double dt)
    {
        preRender(dt);

        Render(dt);

        postRender(dt);

        return -1;
    }
}
