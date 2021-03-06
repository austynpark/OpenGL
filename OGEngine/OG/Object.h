/* Start Header -------------------------------------------------------
File Name: Object.h
Purpose: Object contains model name and position, scale, rotation, color. In the rendering loop-
model file is loaded by the model name in Object class and pass uniform variables
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>

#include "Texture.h"
#include "Model.h"

namespace OG
{
    class Shader;

    class Object
    {
    public:
        Object(const std::string& fileName);

        Object* CreateObject(const std::string& fileName, glm::vec3 position, glm::vec3 scale,
            glm::vec3 rotation_axis = glm::vec3(1.0f), GLfloat angle = 0.0f, glm::vec3 color = glm::vec3(0.0f));

        std::string name_;

        glm::vec3 position_;
        glm::vec3 scale_;
        glm::vec3 rotation_axis_;
        GLfloat rotation_angle_;
        glm::vec3 color_; // I_emissive


        const std::string& getName() const; 
        const glm::vec3& getPosition() const; 
        const glm::vec3& getScale() const; 
        const glm::vec3& getRotation() const;
        const glm::vec3& getColor() const;

        void setPosition(const glm::vec3& pos);
        void setRotation(const glm::vec3& rot); 
        void setScale(const glm::vec3& s);
        void setColor(const glm::vec3& col);
        void setName(const char* name);
        void draw();
        void drawNormal(bool drawFaceNormal);

		static std::unordered_map<std::string, std::unique_ptr<Model>> models_;

    private:
    };
    extern Object* OBJECT;
}

#endif



