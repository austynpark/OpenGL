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

#include "BufferObject.h"
#include "UniformBuffer.h"
#include "ArrayBufferObject.h"
#include "VertexAttributeManager.h"


OG::Scene_Assignment2::Scene_Assignment2(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight),
shader(std::make_unique<Shader>()), pCamera_(std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 5.0f)))
{

}

OG::Scene_Assignment2::~Scene_Assignment2()
{
}

int OG::Scene_Assignment2::Init()
{
	return 0;
}

void OG::Scene_Assignment2::CleanUp()
{
}

int OG::Scene_Assignment2::Render(double dt)
{
    shader->LoadShaders("OG/shaders/PKO.vert", "OG/shaders/PKO.frag");
    shader->Use();

    shader->SetUniformBlock("Transforms", 0);
    shader->SetUniformBlock("LightInfo", 1);
    shader->SetUniformBlock("Material", 2);

	pUBO_light->AddSubData(0, sizeof(glm::vec4), glm::value_ptr(lightColor));
    pUBO_light->AddSubData(sizeof(glm::vec4), sizeof(GLfloat), &ambientStrength);


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
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("CS300 Assignment 2");

        if (ImGui::BeginCombo("Central Object", current_item))
        {
            for (const auto& model : models_)
            {
                bool is_selected =
                    (current_item ==
                        model.first.c_str()); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(model.first.c_str(), is_selected))
                    current_item = model.first.c_str();
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }

            ImGui::EndCombo();
        }

        if (ImGui::RadioButton("face normal", drawFaceNormal == true)) { drawFaceNormal = true; } ImGui::SameLine();
        if (ImGui::RadioButton("vertex normal", !drawFaceNormal == true)) { drawFaceNormal = false; }
        ImGui::Checkbox("Draw Normal Vector", &is_normal_vector_on);

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void OG::Scene_Assignment2::CleanupImGui()
{
}

void OG::Orbit::SetOrbit(float radius, int numberOfPoints)
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
