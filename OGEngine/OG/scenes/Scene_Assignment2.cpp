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


OG::Scene_Assignment2::Scene_Assignment2(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight),
shaders_(), lightShader(std::make_unique<Shader>("light_object.vert", "light_object.frag")),
pCamera_(std::make_unique<Camera>(glm::vec3(0.0f, 3.0f, 10.0f))),
pDiffuseTexture(std::make_unique<Texture>("metal_roof_diff_512x512.png", 0)), 
pSpecularTexture(std::make_unique<Texture>("metal_roof_spec_512x512.png", 1)),
pVisualizeTexture(std::make_unique<Texture>("visualize.png", 0)),
pOrbit(std::make_unique<Orbit>(5.0f, 100)),
plane_(std::move(OBJECT->CreateObject("quad", glm::vec3(0.0f, -2.f, 0.0f), glm::vec3(15.0f), glm::vec3(1.0f, 0.0f, 0.0f), 90.0f)))
{
    num_of_lights = 1;
    isRotating = true;
    bVisualizeTex = false;
    is_normal_vector_on = false;
    drawFaceNormal = false;
    bCalcOnCPU = true;
}

OG::Scene_Assignment2::~Scene_Assignment2()
{
}

int OG::Scene_Assignment2::Init()
{
    shaders_["PhongShading"] = std::make_unique<Shader>("PhongShading.vert", "PhongShading.frag");
    shaders_["PhongLighting"] = std::make_unique<Shader>("PhongLighting.vert", "PhongLighting.frag");
    
    current_shader = "PhongShading";
    
    shaders_[current_shader]->Use();

	//Initialize Uniform Buffer Block
    SetupBuffers();
    pOrbit->SetupBuffer();

    current_item = "cube2";

    objects_.emplace_back(std::move(
        OBJECT->CreateObject("cube2",glm::vec3(0.0f), glm::vec3(1.0f))
    ));
    
    OBJECT->models_["4Sphere"] = std::make_unique<Model>("OG/models/4Sphere.obj");
    OBJECT->models_["bunny_high_poly"] = std::make_unique<Model>("OG/models/bunny_high_poly.obj");
    OBJECT->models_["simpleSphere"] = std::make_unique<Model>("OG/models/simpleSphere.obj");
    OBJECT->models_["sphere_mesh"] = std::make_unique<Model>(Mesh::CreateSphere(0.1f, 36, 18));



    for (int i = 0; i < 16; ++i)
    {
        spheres_.emplace_back(std::move(
            OBJECT->CreateObject("sphere_mesh", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f))
        ));
    }

    return 0;
}

void OG::Scene_Assignment2::CleanUp()
{
}

int OG::Scene_Assignment2::preRender(double dt)
{
	glClearColor(fog_color.x, fog_color.y, fog_color.z, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_STENCIL_TEST);

    glCullFace(GL_BACK);


    for (int i = 0; i < num_of_lights; ++i)
    {
        float theta = i * 360.0f / num_of_lights;

        spheres_[i]->position_ = { pOrbit->radius * cos(glm::radians(theta)), 0.0f, pOrbit->radius * sin(glm::radians(theta)) };
    }

    lightShader->SetUniformBlock("Transforms", 0U);

	shaders_[current_shader]->SetUniformBlock("Transforms", 0U);
    shaders_[current_shader]->SetUniformBlock("LightInfo", 1U);

    shaders_[current_shader]->SetUniform1i("diffuse_texture", 0);
    shaders_[current_shader]->SetUniform1i("specular_texture", 1);

    if (bCalcOnCPU)
    {
        shaders_[current_shader]->SetUniform1i("uvType", 0);
    }
    else
    {
        shaders_[current_shader]->SetUniform1i("uvType", (GLint)(OBJECT->models_[current_item]->uvType + 1));
    }

    return 0;
}

int OG::Scene_Assignment2::Render(double dt)
{
    shaders_[current_shader]->Use();

    glm::mat4 projection = pCamera_->GetProjMatrix((float)_windowWidth, (float)_windowHeight);
	glm::mat4 view = pCamera_->GetViewMatrix();
    
    if (!bVisualizeTex)
        pDiffuseTexture->Bind();
    else
        pVisualizeTexture->Bind();
    pSpecularTexture->Bind();

    pUBO_transform->AddSubData(0, sizeof(glm::mat4), glm::value_ptr(view));
    pUBO_transform->AddSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));


	lightShader->Use();

    GLsizei offset = 0;
    for (int i = 0; i < num_of_lights; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 scale = spheres_[i]->getScale();
        glm::vec3 transform = spheres_[i]->getPosition();

        model = glm::rotate(angleOfRotation, glm::vec3(0, 1, 0)) * glm::translate(transform) * glm::scale(scale);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

        lightShader->SetUniformMatrix3fv("normalMatrix", normalMatrix);
        lightShader->SetUniformMatrix4fv("model", model);

        light[i].position = model * glm::vec4(spheres_[i]->position_, 1.0f);
        light[i].direction = glm::vec4(objects_[0]->position_, 1.0f) - light[i].position;

        
        OBJECT->models_[spheres_[i]->getName()]->Draw(shaders_[current_shader].get());
 /************************************* ADD LIGHT UNIFORM BUFFER DATA ****************************************************************************/
        pUBO_light->AddSubData(offset, static_cast<GLsizei>(sizeof(glm::vec4) * 6), &light[i]);
        offset += static_cast<GLsizei>(sizeof(glm::vec4) * 6);
    }
   
    pUBO_light->AddSubData(sizeof(Light) * 16, sizeof(glm::vec3), glm::value_ptr(att)); //96 * 4, 16
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(pCamera_->Position)); // 96 * 4 + 16, 16
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 2, sizeof(glm::vec3), glm::value_ptr(global_ambient)); // 96 + 32, 16
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 3, sizeof(glm::vec3), glm::value_ptr(fog_color)); // 96 + 48, 16
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 3 + sizeof(glm::vec3), sizeof(GLfloat), &near);
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 3 + sizeof(glm::vec3) + sizeof(GLfloat), sizeof(GLfloat), &far);
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 3 + sizeof(glm::vec3) + sizeof(GLfloat) * 2, sizeof(GLfloat), &num_of_lights);
/*************************************************************************************************************************************************/
    shaders_[current_shader]->Use();

    for (const auto& obj : objects_)
    {
        obj->setName(current_item);

        glm::vec3 scale = obj->getScale();
        glm::vec3 transform = obj->getPosition();   
        glm::mat4 model = glm::translate(transform) * glm::rotate(glm::radians(obj->rotation_angle_), obj->rotation_axis_) * glm::scale(scale);

        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        //shaders_[current_shader]->SetUniformMatrix3fv("normalMatrix", normalMatrix);

        shaders_[current_shader]->SetUniformMatrix4fv("model", model);

        if (OBJECT->models_.find(obj->getName()) != OBJECT->models_.end())
        {
            OBJECT->models_[obj->getName()]->Draw(shaders_[current_shader].get());

			OBJECT->models_[obj->getName()]->drawNormal = is_normal_vector_on;
            OBJECT->models_[obj->getName()]->drawFaceNormal = drawFaceNormal;
        }
    }

    if (pOrbit->radius != pOrbit->prev_radius) {
        pOrbit->prev_radius = pOrbit->radius;
		pOrbit->SetupBuffer();
    }

	pOrbit->DrawOrbit();


    glm::vec3 scale = plane_->getScale();
    glm::vec3 transform = plane_->getPosition();
    glm::mat4 model = glm::translate(transform) * glm::rotate(glm::radians(-90.0f), plane_->rotation_axis_) * glm::scale(scale);
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

    shaders_[current_shader]->SetUniformMatrix4fv("model", model);
    //shaders_[current_shader]->SetUniformMatrix3fv("normalMatrix", normalMatrix);
    OBJECT->models_[plane_->getName()]->Draw(shaders_[current_shader].get());

    return 0;
}

int OG::Scene_Assignment2::postRender(double dt)
{
    if (isRotating)
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

	if (glfwGetKey(pWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
        pCamera_->ProcessMouseMovement(-10*dt, 0);
    if (glfwGetKey(pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
        pCamera_->ProcessMouseMovement(10*dt, 0);
    if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS)
        pCamera_->ProcessMouseMovement(0, 10*dt);
    if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
        pCamera_->ProcessMouseMovement(0, 10*-dt);

}

void OG::Scene_Assignment2::SetupBuffers()
{
   // Generate UNIFORM_BUFFER, and Set 'binding point' as an 'index'
    pUBO_transform = std::make_unique<UniformBuffer>(static_cast<GLsizei>(2 * sizeof(glm::mat4)), 0U, GL_DYNAMIC_DRAW);
    pUBO_light = std::make_unique<UniformBuffer>(static_cast<GLsizei>((sizeof(glm::vec4) * 101)), 1U, GL_DYNAMIC_DRAW);
    //pUBO_global = std::make_unique<UniformBuffer>(static_cast<GLsizei>(sizeof(glm::vec4) + sizeof(glm::vec4)), 2U, GL_DYNAMIC_DRAW);

//	pUBO_global->AddSubData(0, sizeof(glm::vec4), glm::value_ptr(att));
	//pUBO_global->AddSubData(sizeof(glm::vec4), sizeof(GLfloat), &num_of_lights);

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
            }

            ImGui::EndCombo();
        }

        if (ImGui::RadioButton("face normal", drawFaceNormal == true)) { drawFaceNormal = true; } ImGui::SameLine();
        if (ImGui::RadioButton("vertex normal", !drawFaceNormal == true)) { drawFaceNormal = false; }
        ImGui::Checkbox("Draw Normal Vector", &is_normal_vector_on);

        ImGui::Separator();

        ImGui::Checkbox("Visualize UV", &bVisualizeTex);

        ImGui::Combo("Texture Entity", (int*)&OBJECT->models_[current_item]->isNormMapping, "POSITION\0NORMAL\0\0");
        ImGui::Combo("Projection Mode", (int*)&OBJECT->models_[current_item]->uvType, "PLANAR\0CYLINDRICAL\0SPHERICAL\0\0");

        ImGui::Spacing();

        if (ImGui::RadioButton("calculate on CPU", bCalcOnCPU == true)) { bCalcOnCPU = true; } ImGui::SameLine();
        if (ImGui::RadioButton("calculate on GPU", bCalcOnCPU == false)) { bCalcOnCPU = false; }

        ImGui::Spacing();

        if (bCalcOnCPU)
        {
            if (ImGui::Button("Calculate UV"))
            {
                OBJECT->models_[current_item]->RemapUV();
            }
        }

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::BeginCombo("Shader", current_shader))
        {
            for (const auto& shader : shaders_)
            {
                bool is_selected =
                    (current_shader ==
                        shader.first.c_str()); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(shader.first.c_str(), is_selected))
                    current_shader = shader.first.c_str();
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)

            }

            ImGui::EndCombo();
        }
        if (ImGui::Button("Reload Shader"))
        {
            shaders_[current_shader]->ReloadShaders();
        }

        ImGui::Spacing();
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Global"))
        {
            ImGui::SliderFloat3("Attenutation", glm::value_ptr(att), 0.0f, 1.0f);
            ImGui::ColorEdit3("Global Ambient Color", glm::value_ptr(global_ambient));
            ImGui::ColorEdit3("Global Fog Color", glm::value_ptr(fog_color));
            ImGui::SliderFloat("Near", &near, 0.0f, far);
            ImGui::SliderFloat("Far", &far, near, 100.0f);
        }


        if (ImGui::CollapsingHeader("Light"))
        {
            ImGui::SliderFloat("Orbit Radius", &pOrbit->radius, 1.0f, 10.0f);
            ImGui::Checkbox("Rotate", &isRotating);

            ImGui::Separator();

            // Slider for num_of_lights
            ImGui::SliderInt("Number of Light", &num_of_lights, 1, 16);

            for (int i = 0; i < num_of_lights; ++i)
            {
                std::string lightName = "light" + std::to_string(i + 1);
                if (ImGui::TreeNode(lightName.c_str()))
                {

                    ImGui::ColorEdit3("ambient", glm::value_ptr(light[i].ambient), ImGuiColorEditFlags_::ImGuiColorEditFlags_InputRGB);
                    ImGui::ColorEdit3("diffuse", glm::value_ptr(light[i].diffuse));
                    ImGui::ColorEdit3("specular", glm::value_ptr(light[i].specular));

                    ImGui::RadioButton("Point", (int*)&light[i].type, 0); ImGui::SameLine();
                    ImGui::RadioButton("Spot", (int*)&light[i].type, 1); ImGui::SameLine();
                    ImGui::RadioButton("Dir", (int*)&light[i].type, 2);

                    if (light[i].type == lightType::E_SPOT)
                    {
                        ImGui::SliderFloat("Inner Angle", &light[i].cutOff, 0.0f, 90.0f);
                        ImGui::SliderFloat("Outer Angle", &light[i].outerCutOff, 0.0f, 90.0f);
                        ImGui::SliderFloat("Fall off", &light[i].fallOut, 0.0f, 10.0f);
                    }

                    // Radio button for light type
                    ImGui::TreePop();
                }
            }

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
    orbit_points.clear();
    orbit_indices.clear();

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
    SetOrbit();
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