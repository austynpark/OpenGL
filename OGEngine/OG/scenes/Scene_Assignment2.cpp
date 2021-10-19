/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene_Assignment2.cpp
Purpose: Scene2
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_2
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/10/17
End Header --------------------------------------------------------*/
#include "ogpch.h"
#include "Scene_Assignment2.h"

#include "Object.h"
#include "Camera.h"
#include "Model.h"
#include "BufferObject.h"
#include "UniformBuffer.h"
#include "ArrayBufferObject.h"
#include "VertexAttributeManager.h"

namespace OG
{
}

OG::Scene_Assignment2::Scene_Assignment2(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight),
PhongLighting(std::make_unique<Shader>()), pCamera_(std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 5.0f))),
pDiffuseTexture(std::make_unique<Texture>("metal_roof_diff_512x512.png", 0)), 
pSpecularTexture(std::make_unique<Texture>("metal_roof_spec_512x512.png", 1)),
pOrbit(std::make_unique<Orbit>(2.0f, 100))
{
    ambient = glm::vec3();
    diffuse = glm::vec3();
    specular = glm::vec3();
}

OG::Scene_Assignment2::~Scene_Assignment2()
{
}

int OG::Scene_Assignment2::Init()
{
    PhongLighting->LoadShaders("OG/shaders/PhongLighting.vert", "OG/shaders/PhongLighting.frag");
    PhongLighting->Use();

    PhongLighting->SetUniformBlock("Transforms", 0);
    PhongLighting->SetUniformBlock("LightInfo", 1);
    PhongLighting->SetUniformBlock("Material", 2);

    PhongLighting->SetUniform1i("diffuse_texture", 0);
    PhongLighting->SetUniform1i("specular_texture", 1);
    PhongLighting->SetUniform1f("shininess", 32.0f);

    current_item = "cube2";

    objects_.emplace_back(std::move(
        OBJECT->CreateObject("cube2",glm::vec3(0.0f), glm::vec3(1.0f))
    ));



    SetupBuffers();

    return 0;

}

void OG::Scene_Assignment2::CleanUp()
{
}

int OG::Scene_Assignment2::preRender(double dt)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_DEBUG_OUTPUT);
    //glCullFace(GL_BACK);


    return 0;
}

int OG::Scene_Assignment2::Render(double dt)
{
    PhongLighting->Use();
	
    glm::mat4 projection =
        glm::perspective(glm::radians(pCamera_->Zoom), (float)_windowWidth / (float)_windowHeight, 0.1f, 100.0f);
    glm::mat4 view = pCamera_->GetViewMatrix();

    pDiffuseTexture->Bind();
    pSpecularTexture->Bind();

    pUBO_transform->AddSubData(0, sizeof(glm::mat4), glm::value_ptr(view));
    pUBO_transform->AddSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));

	pUBO_light->AddSubData(0, sizeof(glm::vec4), glm::value_ptr(ambient));
    pUBO_light->AddSubData(sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(diffuse));
    pUBO_light->AddSubData(sizeof(glm::vec4) * 2, sizeof(glm::vec4), glm::value_ptr(specular));


	PhongLighting->SetUniform3fv("lightPos", lightPos);

    for (const auto& obj : objects_)
    {
        obj->setName(current_item);

		glm::vec3 scale = obj->getScale();
        glm::vec3 transform = obj->getPosition();
        //glm::vec3 color = obj->getColor();
		glm::mat4 model = glm::translate(transform) * glm::rotate(angleOfRotation, glm::vec3(1, 1, 1))* glm::scale(scale);

        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));

        PhongLighting->SetUniformMatrix4fv("model", model);
        PhongLighting->SetUniformMatrix3fv("normalMatrix", normalMatrix);
        //PhongLighting->SetUniform3fv("objectColor", color);

		if (OBJECT->models_.find(obj->getName()) != OBJECT->models_.end())
        {
            OBJECT->models_[obj->getName()]->Draw(GL_TRIANGLES);
        }
    }
	return 0;
}

int OG::Scene_Assignment2::postRender(double dt)
{
	angleOfRotation += static_cast<float>(dt);

	return 0;
}

void OG::Scene_Assignment2::keyboardInput(GLFWwindow* pWindow, float dt)
{
	if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
        pCamera_->process_keyboard(Camera_Movement::FORWARD, dt);
    if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)
        pCamera_->process_keyboard(Camera_Movement::BACKWARD, dt);
    if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS)
        pCamera_->process_keyboard(Camera_Movement::LEFT, dt);
    if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS)
        pCamera_->process_keyboard(Camera_Movement::RIGHT, dt);
}

void OG::Scene_Assignment2::SetupBuffers()
{
   // Generate UNIFORM_BUFFER, and Set 'binding point' as an 'index'
    pUBO_transform = std::make_unique<UniformBuffer>(static_cast<GLsizei>(2 * sizeof(glm::mat4)), 0U, GL_DYNAMIC_DRAW);
    pUBO_light = std::make_unique<UniformBuffer>(static_cast<GLsizei>((sizeof(glm::vec4)) + sizeof(glm::vec3)), 1U, GL_STATIC_DRAW);

}

void OG::Scene_Assignment2::SetupImGui(GLFWwindow* pWindow)
{
    //// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(pWindow, true);
    ImGui_ImplOpenGL3_Init("#version 450 core");
}

void OG::Scene_Assignment2::DrawImGui(GLFWwindow* pWindow)
{
    bool show = true;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

	ImGui::ShowDemoWindow(&show);

    {
        ImGui::Begin("CS300 Assignment 2");
        if (ImGui::BeginCombo("Central Object", current_item))
        {
            
            for (const auto& model : OBJECT->models_)
            {
                bool is_selected =
                    (current_item ==
                        model.first.c_str()); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(model.first.c_str(), is_selected))
                    current_item = model.first.c_str();
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            
                ImGui::Checkbox("Draw Normal Vector", &is_normal_vector_on);
            }

            ImGui::EndCombo();
        }

        if (ImGui::TreeNode("Light Info"))
        {
            
            ImGui::ColorEdit3("ambient", glm::value_ptr(ambient));
            ImGui::ColorEdit3("diffuse", glm::value_ptr(diffuse));
            ImGui::ColorEdit3("specular", glm::value_ptr(specular));

            ImGui::TreePop();
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void OG::Scene_Assignment2::CleanupImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

//////////////////////////////////////////////////////
// ///////////////////////////////////////////////////
//Orbit Function Start
OG::Orbit::Orbit(float radius, int numberOfPoints)
{
    this->radius = radius;
    this->numberOfPoints = numberOfPoints;
    SetOrbit();
    SetupBuffer();
}

OG::Orbit::~Orbit()
{
}

void OG::Orbit::SetOrbit()
{
	float sectorAngle = 360.0f / numberOfPoints;
    float theta = 0;

    for (int i = 0; i < numberOfPoints; ++i) {
        theta = i * sectorAngle;
        orbit_points.emplace_back(radius * cos(glm::radians(theta)), 0.0f, radius * sin(glm::radians(theta)));
        orbit_indices.push_back(i);
        orbit_indices.push_back((i + 1) % numberOfPoints);
    }
}

void OG::Orbit::DrawOrbit()
{
    pVAO->Bind();
    glDrawElements(GL_LINES, static_cast<GLsizei>(orbit_indices.size()), GL_UNSIGNED_INT, 0);
    pVAO->UnBind();
}

void OG::Orbit::SetupBuffer()
{
    // Bind function is called in the constructor
    pVAO = std::make_unique<ArrayBufferObject>(); // pVAO->Bind();
    pVBO = std::make_unique<BufferObject>(orbit_points.data(), static_cast<GLuint>(orbit_points.size() * sizeof(glm::vec3))); // pVBO->Bind();
    pEBO = std::make_unique<BufferObject>(orbit_indices.data(), static_cast<GLuint>(orbit_indices.size() * sizeof(GLuint)), GL_ELEMENT_ARRAY_BUFFER); // pEBO->Bind();

    // Vertex Attribute Setting
    VertexAttributeManager attributeManager;
    // Store VertexBufferElement as a vector that has size, type, type size, and store stride at VertexAttributeManager
    attributeManager.AddAttribute(3, GL_FLOAT, sizeof(GLfloat));
    // VertexBufferElement have size, type, type_size
    // Get stored VertexAttribute to set glVertexAttribPointer
    std::vector<VertexBufferElement> layouts = attributeManager.GetLayout();

    unsigned int lastIndex = static_cast<unsigned int>(layouts.size());
    uint64_t offset = 0;

    for (unsigned int i = 0; i < lastIndex; ++i)
    {
        const VertexBufferElement elem = layouts[i];
        glVertexAttribPointer(i, elem.count_, elem.type_, elem.isNormalized_, attributeManager.GetStride(), (const void*)offset);
        glEnableVertexAttribArray(i);
        offset += elem.count_ * elem.typeSize_;
    }
}
// Orbit Function End