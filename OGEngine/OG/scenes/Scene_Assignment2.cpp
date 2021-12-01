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
#include "Texture.h"
#include "shader.hpp"
#include "FBO.h"
#include "CubeMap.h"

OG::Scene_Assignment2::Scene_Assignment2(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight),
shaders_(), lightShader(std::make_unique<Shader>("light_object.vert", "light_object.frag")),
pCamera_(std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 10.0f))),
pDiffuseTexture(std::make_unique<Texture>("metal_roof_diff_512x512.png", 0)), 
pSpecularTexture(std::make_unique<Texture>("metal_roof_spec_512x512.png", 1)),
pVisualizeTexture(std::make_unique<Texture>("visualize.png", 0)),
pOrbit(std::make_unique<Orbit>(2.5f, 300)),
pFBO(std::make_unique<FBO>()),
skybox(std::make_unique<CubeMap>())
{
    num_of_lights = 1;
    isRotating = true;
    bVisualizeTex = false;
    is_normal_vector_on = false;
    drawFaceNormal = false;
    bCalcOnCPU = false;

    refraction[0] = 1.0003f; // Air
    refraction[1] = 1.000132f; // Hydrogen
    refraction[2] = 1.33f; // Water
    refraction[3] = 1.47f; // Olive Oil
    refraction[4] = 1.31f; // Ice
    refraction[5] = 1.46f; // Quartz
    refraction[6] = 2.42f; // Diamond
    refraction[7] = 1.49f; // Acrylic
    refraction[8] = 0.0f;

    refract_index = refraction[0];
    chromatic_aberration = 0.01f;
    index_of_refract_items = 0;

    b_calcReflect = true;
    b_calcRefract = true;
    mixRatio = 1.0f;
    fresnelPower = 2.0f;
}

OG::Scene_Assignment2::~Scene_Assignment2()
{
}

int OG::Scene_Assignment2::Init()
{
    shaders_["PhongShading"] = std::make_unique<Shader>("PhongShading.vert", "PhongShading.frag");
    //shaders_["PhongLighting"] = std::make_unique<Shader>("PhongLighting.vert", "PhongLighting.frag");
    //shaders_["BlinnPhong"] = std::make_unique<Shader>("BlinnPhong.vert", "BlinnPhong.frag");
    
    current_shader = "PhongShading";
    
    shaders_[current_shader]->Use();

	//Initialize Uniform Buffer Block
    SetupBuffers();
    pOrbit->SetupBuffer();

    current_item = "simpleSphere";

    objects_.emplace_back(std::move(
        OBJECT->CreateObject(current_item, glm::vec3(0.0f), glm::vec3(1.0f))
    ));
    
    OBJECT->models_["cube"] = std::make_unique<Model>("OG/models/cube.obj");
    OBJECT->models_["4Sphere"] = std::make_unique<Model>("OG/models/4Sphere.obj");
    OBJECT->models_["bunny_high_poly"] = std::make_unique<Model>("OG/models/bunny_high_poly.obj");
    OBJECT->models_["simpleSphere"] = std::make_unique<Model>("OG/models/simpleSphere.obj");
    OBJECT->models_["sphere_mesh"] = std::make_unique<Model>(Mesh::CreateSphere(0.1f, 36, 18));
    OBJECT->models_["bunny"] = std::make_unique<Model>("OG/models/bunny.obj");
    OBJECT->models_["cup"] = std::make_unique<Model>("OG/models/cup.obj");
    OBJECT->models_["sphere"] = std::make_unique<Model>("OG/models/sphere.obj");
    OBJECT->models_["lucy_princeton"] = std::make_unique<Model>("OG/models/lucy_princeton.obj");
    OBJECT->models_["quad"] = std::make_unique<Model>("OG/models/quad.obj");
    OBJECT->models_["sphere_modified"] = std::make_unique<Model>("OG/models/sphere_modified.obj");
    OBJECT->models_["starwars1"] = std::make_unique<Model>("OG/models/starwars1.obj");
    OBJECT->models_["triangle"] = std::make_unique<Model>("OG/models/triangle.obj");
    OBJECT->models_["rhino"] = std::make_unique<Model>("OG/models/rhino.obj");

    for (int i = 0; i < 16; ++i)
    {
        spheres_.emplace_back(std::move(
            OBJECT->CreateObject("sphere_mesh", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f))
        ));
    }
    
    const char* faces[] = { "skybox/right.png",
        "skybox/left.png", "skybox/top.png", "skybox/bottom.png", "skybox/front.png", "skybox/back.png"
    };

    skybox->Init(faces, 2);

    setFramebuffer();

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
    //glEnable(GL_DEBUG_OUTPUT);
    //glEnable(GL_STENCIL_TEST);

    glCullFace(GL_BACK);

    for (int i = 0; i < num_of_lights; ++i)
    {
        float theta = (i * 360.0f / num_of_lights) + angleOfRotation;
        spheres_[i]->position_ = { pOrbit->radius * cos(glm::radians(theta)), 0.0f, pOrbit->radius * sin(glm::radians(theta)) };
    }

    return 0;
}

int OG::Scene_Assignment2::Render(double dt)
{

    glm::mat4 view = pCamera_->GetViewMatrix();
    glm::mat4 projection = pCamera_->GetProjMatrix((float)_windowWidth, (float)_windowHeight);

    shaders_[current_shader]->Use();

    lightShader->SetUniformBlock("Transforms", 0U);

    shaders_[current_shader]->SetUniformBlock("Transforms", 0U);
    shaders_[current_shader]->SetUniformBlock("LightInfo", 1U);

    shaders_[current_shader]->SetUniform1i("diffuse_texture", pDiffuseTexture->texNum_);
    shaders_[current_shader]->SetUniform1i("specular_texture", pSpecularTexture->texNum_);

    if (!bVisualizeTex)
        pDiffuseTexture->Bind();
    else
        pVisualizeTexture->Bind();
    pSpecularTexture->Bind();

    // By converting view matrix 4x4 to 3x3, remove translation section
    glm::mat4 view_without_translation = glm::mat4(glm::mat3(pCamera_->GetViewMatrix()));
   
    //glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    skybox->Render(view_without_translation, projection);
    //glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    updateFramebuffer();

	pUBO_transform->AddSubData(0, sizeof(glm::mat4), glm::value_ptr(view));
    pUBO_transform->AddSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));

    lightShader->Use();

    GLsizei offset = 0;
    for (int i = 0; i < num_of_lights; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 scale = spheres_[i]->getScale();
        glm::vec3 transform = spheres_[i]->getPosition();

        model = glm::translate(transform) * glm::scale(scale);

        lightShader->SetUniformMatrix4fv("model", model);
        lightShader->SetUniform4fv("diffuse", light[i].diffuse);

        light[i].position = glm::vec4(spheres_[i]->position_, 0.0f);
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

	if (pOrbit->radius != pOrbit->prev_radius) {
        pOrbit->prev_radius = pOrbit->radius;
        pOrbit->SetupBuffer();
    }

    lightShader->SetUniformMatrix4fv("model", pOrbit->model);
    lightShader->SetUniform4fv("diffuse", glm::vec4(1.0f));
    pOrbit->DrawOrbit();

    shaders_[current_shader]->Use();

    if (bCalcOnCPU)
    {
        shaders_[current_shader]->SetUniform1i("uvType", 0);
    }
    else
    {
        shaders_[current_shader]->SetUniform1i("uvType", (GLint)(OBJECT->models_[current_item]->uvType + 1));
    }

    pFBO->getAttachment("right")->Bind();
    shaders_[current_shader]->SetUniform1i("right", pFBO->getAttachment("right")->texNum_);
    pFBO->getAttachment("left")->Bind();
    shaders_[current_shader]->SetUniform1i("left", pFBO->getAttachment("left")->texNum_);
    pFBO->getAttachment("top")->Bind();
    shaders_[current_shader]->SetUniform1i("top", pFBO->getAttachment("top")->texNum_);
    pFBO->getAttachment("bottom")->Bind();
    shaders_[current_shader]->SetUniform1i("bottom", pFBO->getAttachment("bottom")->texNum_);
    pFBO->getAttachment("front")->Bind();
    shaders_[current_shader]->SetUniform1i("front", pFBO->getAttachment("front")->texNum_);
    pFBO->getAttachment("back")->Bind();
    shaders_[current_shader]->SetUniform1i("back", pFBO->getAttachment("back")->texNum_);

    for (const auto& obj : objects_)
    {
        obj->setName(current_item);

        glm::vec3 scale = obj->getScale();
        glm::vec3 transform = obj->getPosition();
        glm::mat4 model = glm::translate(transform) * glm::rotate(glm::radians(obj->rotation_angle_), obj->rotation_axis_) * glm::scale(scale);

        pOrbit->model = model;

        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        shaders_[current_shader]->SetUniformMatrix4fv("normalMatrix", normalMatrix);

        shaders_[current_shader]->SetUniformMatrix4fv("model", model);
        shaders_[current_shader]->SetUniform3fv("I_emissive", obj->color_);

        shaders_[current_shader]->SetUniform1f("refractIndex", refract_index);
        shaders_[current_shader]->SetUniform1f("chromatic_aberration", chromatic_aberration);
        shaders_[current_shader]->SetUniform1f("fresnelPower", fresnelPower);

        shaders_[current_shader]->SetUniform1f("mixRatio", mixRatio); // environment Mapping + phong mix ratio
        shaders_[current_shader]->SetUniform1b("b_calcReflect", b_calcReflect);
        shaders_[current_shader]->SetUniform1b("b_calcRefract", b_calcRefract);

        if (OBJECT->models_.find(obj->getName()) != OBJECT->models_.end())
        {
            obj->drawNormal = is_normal_vector_on;
            obj->drawFaceNormal = drawFaceNormal;
            obj->draw(shaders_[current_shader].get());

        }
    }

    return 0;
}

int OG::Scene_Assignment2::postRender(double dt)
{
    if (isRotating)
        angleOfRotation += rotation_speed * static_cast<float>(dt) * 100.0f;

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
    //bool show = true;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::ShowDemoWindow(&show);

    {
        ImGui::Begin("Graphics");

        if (ImGui::BeginTabBar("Tab Bar"))
        {
            if (ImGui::BeginTabItem("General"))
            {
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

				ImGui::Spacing();
				ImGui::Separator();

				if (ImGui::RadioButton("face normal", drawFaceNormal == true)) { drawFaceNormal = true; } ImGui::SameLine();
				if (ImGui::RadioButton("vertex normal", !drawFaceNormal == true)) { drawFaceNormal = false; }
				ImGui::Checkbox("Draw Normal Vector", &is_normal_vector_on);

                ImGui::Spacing();
                ImGui::Separator();

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

                ImGui::Spacing();

                if (ImGui::Button("Reload Shader"))
                {
                    shaders_[current_shader]->ReloadShaders();
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Material"))
            {
                ImGui::SliderFloat("Blending", &mixRatio, 0.0f, 1.0f);

                ImGui::Separator();

                ImGui::ColorEdit3("Emissive", glm::value_ptr(objects_[0]->color_));

                ImGui::Separator();

                ImGui::Checkbox("Visualize UV", &bVisualizeTex);

                ImGui::Combo("Texture Entity", (int*)&OBJECT->models_[current_item]->isNormMapping, "POSITION\0NORMAL\0\0");
                ImGui::Combo("Projection Mode", (int*)&OBJECT->models_[current_item]->uvType, "PLANAR\0CYLINDRICAL\0SPHERICAL\0NONE\0\0");

                ImGui::Spacing();

                if (bCalcOnCPU)
                {
                    if (ImGui::Button("Calculate UV"))
                    {
                        OBJECT->models_[current_item]->RemapUV();
                    }
                }

                ImGui::Spacing();

                if (ImGui::RadioButton("calculate on CPU", bCalcOnCPU == true)) { bCalcOnCPU = true; } ImGui::SameLine();
                if (ImGui::RadioButton("calculate on GPU", bCalcOnCPU == false)) { bCalcOnCPU = false; }

                ImGui::Separator();
                ImGui::Spacing();

                ImGui::Checkbox("Calculate Reflect", &b_calcReflect);
                ImGui::Checkbox("Calculate Refract", &b_calcRefract);

				ImGui::Spacing();
                ImGui::Separator();

                if (b_calcRefract) {
                    //ImGui::SliderFloat("Refraction Index", )

                    const char* refract_name[] = { "Air", "Hydrogen", "Water", "Olive Oil", "Ice", "Quartz", "Diamond", "Acrylic"};

                    
                    ImGui::PushItemWidth(80);
					ImGui::Text("Refraction ");
                    ImGui::SameLine();
					ImGui::Text("Refraction Index ");
                    ImGui::SameLine();
                    ImGui::Text("Chromatic Aberration");

                    ImGui::PushID(0);
                    if (ImGui::BeginListBox("", ImVec2(80, 120)))
                    {
                        for (int i = 0; i < sizeof(refract_name) / sizeof(refract_name[0]) - 1; ++i)
                        {
                            const bool is_selected = (index_of_refract_items == i);
                            if (ImGui::Selectable(refract_name[i], is_selected))
                                index_of_refract_items = i;

                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                                refract_index = refraction[index_of_refract_items];
                            }
                        }

                        ImGui::EndListBox();
                    }
                    ImGui::PopID();
					ImGui::SameLine();

                    ImGui::PushID(1);
                    if (ImGui::VSliderFloat(" ", ImVec2(100, 120), &refract_index, 1.0f, 100.0f, "%.1f"))
                    {
                        // refract[8] will have not setted ( it'll just use slider value)
                        index_of_refract_items = 8;
                    }
                    ImGui::PopID();

                    ImGui::SameLine();

					ImGui::PushID(2);
                    ImGui::VSliderFloat(" ", ImVec2(100, 120), &chromatic_aberration, 0.0f, 1.0f,"%.3f");
                    ImGui::PopID();


                    ImGui::Dummy(ImVec2(10.0f, 1.0f));
                    ImGui::PopItemWidth();

                }

		        ImGui::Spacing();
				ImGui::Separator();

                if (b_calcReflect && b_calcRefract)
                {
                    ImGui::SliderFloat("Fresnel Power", &fresnelPower, 0.1f, 10.0f);
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Global Constant"))
            {
                if (ImGui::CollapsingHeader("Global"))
                {
                    ImGui::SliderFloat3("Attenutation", glm::value_ptr(att), 0.0f, 1.0f);
                    ImGui::ColorEdit3("Global Ambient Color", glm::value_ptr(global_ambient));
                    ImGui::ColorEdit3("Global Fog Color", glm::value_ptr(fog_color));
                    ImGui::SliderFloat("Near", &near, 0.0f, far);
                    ImGui::SliderFloat("Far", &far, near, 100.0f);
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Light"))
            {
                if (ImGui::CollapsingHeader("Light"))
                {
                    ImGui::Text("Lighting Scenarios");
                    if (ImGui::Button("Scenario 1")) setScenario1(); ImGui::SameLine();
                    if (ImGui::Button("Scenario 2")) setScenario2(); ImGui::SameLine();
                    if (ImGui::Button("Scenario 3")) setScenario3();

                    ImGui::SliderFloat("Orbit Radius", &pOrbit->radius, 1.0f, 10.0f);
                    ImGui::SliderFloat("Rotation Speed", &rotation_speed, -3.0f, 3.0f);
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

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
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

/// <summary>
/// Frame Buffer, Add 6 attachments for CUBE MAP
/// </summary>
void OG::Scene_Assignment2::setFramebuffer()
{
    pFBO->init(_windowWidth, _windowHeight);
    pFBO->bind();

    std::unique_ptr<Texture> right = std::make_unique<Texture>(_windowWidth, _windowHeight, 2);
    std::unique_ptr<Texture> left = std::make_unique<Texture>(_windowWidth, _windowHeight, 3);
    std::unique_ptr<Texture> top = std::make_unique<Texture>(_windowWidth, _windowHeight, 4);
    std::unique_ptr<Texture> bottom = std::make_unique<Texture>(_windowWidth, _windowHeight, 5);
    std::unique_ptr<Texture> front = std::make_unique<Texture>(_windowWidth, _windowHeight, 6);
    std::unique_ptr<Texture> back = std::make_unique<Texture>(_windowWidth, _windowHeight, 7);

    pFBO->setTextureAttachment("right", right);
    pFBO->setTextureAttachment("left", left);
    pFBO->setTextureAttachment("top", top);
    pFBO->setTextureAttachment("bottom", bottom);
    pFBO->setTextureAttachment("front", front);
    pFBO->setTextureAttachment("back", back);

    pFBO->setDepthBuffer();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Frame Buffer Not Ready" << std::endl;
    }

    pFBO->unbind();
}

void OG::Scene_Assignment2::updateFramebuffer()
{
    pFBO->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);    
    pUBO_transform->AddSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));

    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(pCamera_->Position)); // 96 * 4 + 16, 16
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 2, sizeof(glm::vec3), glm::value_ptr(global_ambient)); // 96 + 32, 16
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 3, sizeof(glm::vec3), glm::value_ptr(fog_color)); // 96 + 48, 16
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 3 + sizeof(glm::vec3), sizeof(GLfloat), &near);
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 3 + sizeof(glm::vec3) + sizeof(GLfloat), sizeof(GLfloat), &far);
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 3 + sizeof(glm::vec3) + sizeof(GLfloat) * 2, sizeof(GLfloat), &num_of_lights);
    pUBO_light->AddSubData(sizeof(Light) * 16, sizeof(glm::vec3), glm::value_ptr(att)); //96 * 4, 16

    for (int i = 0; i < 6; ++i) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glm::mat4 view = skybox->camera_lookat[i];
        pUBO_transform->AddSubData(0, sizeof(glm::mat4), glm::value_ptr(view));
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);

        glDepthMask(GL_FALSE);
        skybox->Render(view, projection);
        glDepthMask(GL_TRUE);

        lightShader->Use();

        GLsizei offset = 0;
        for (int i = 0; i < num_of_lights; ++i) {
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 scale = spheres_[i]->getScale();
            glm::vec3 transform = spheres_[i]->getPosition();

            model = glm::translate(transform) * glm::scale(scale);

            lightShader->SetUniformMatrix4fv("model", model);
            lightShader->SetUniform4fv("diffuse", light[i].diffuse);

            light[i].position = glm::vec4(spheres_[i]->position_, 0.0f);
            light[i].direction = glm::vec4(objects_[0]->position_, 1.0f) - light[i].position;

            OBJECT->models_[spheres_[i]->getName()]->Draw(shaders_[current_shader].get());
            pUBO_light->AddSubData(offset, static_cast<GLsizei>(sizeof(glm::vec4) * 6), &light[i]);
            offset += static_cast<GLsizei>(sizeof(glm::vec4) * 6);
        }

        lightShader->SetUniformMatrix4fv("model", pOrbit->model);
        lightShader->SetUniform4fv("diffuse", glm::vec4(1.0f));
        pOrbit->DrawOrbit();
    }

    pFBO->unbind();
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
    glLineWidth(4.0f);
    glDrawElements(GL_LINES, static_cast<GLsizei>(orbit_indices.size()), GL_UNSIGNED_INT, 0);
    glLineWidth(1.0f);
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

void OG::Scene_Assignment2::setScenario1()
{
    pOrbit->radius = 2.5f;
    rotation_speed = 1.0f;

    num_of_lights = 4;

    light[0].type = lightType::E_POINT;
    light[0].ambient = glm::vec4(1.0f);
    light[0].diffuse = glm::vec4(1.0f);
    light[0].specular = glm::vec4(1.0f);

	light[1].type = lightType::E_POINT;
    light[1].ambient = glm::vec4(1.0f);
    light[1].diffuse = glm::vec4(1.0f);
    light[1].specular = glm::vec4(1.0f);

	light[2].type = lightType::E_POINT;
    light[2].ambient = glm::vec4(1.0f);
    light[2].diffuse = glm::vec4(1.0f);
    light[2].specular = glm::vec4(1.0f);

	light[3].type = lightType::E_POINT;
    light[3].ambient = glm::vec4(1.0f);
    light[3].diffuse = glm::vec4(1.0f);
    light[3].specular = glm::vec4(1.0f);
}

void OG::Scene_Assignment2::setScenario2()
{
	pOrbit->radius = 2.5f;
    rotation_speed = 3.0f;

    num_of_lights = 6;

    light[0].type = lightType::E_SPOT;
    light[0].ambient = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
    light[0].diffuse = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
    light[0].specular = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
    light[0].cutOff = 45.0f;
    light[0].outerCutOff = 60.0f;
    light[0].fallOut = 5.0f;

	light[1].type = lightType::E_SPOT;
    light[1].ambient = glm::vec4(1.0f, 1.0f, 0.5f, 1.0f);
    light[1].diffuse = glm::vec4(1.0f, 1.0f, 0.5f, 1.0f);
    light[1].specular = glm::vec4(1.0f, 1.0f, 0.5f, 1.0f);
    light[1].cutOff = 45.0f;
    light[1].outerCutOff = 60.0f;
    light[1].fallOut = 5.0f;

	light[2].type = lightType::E_SPOT;
    light[2].ambient = glm::vec4(1.0f, 0.5f, 1.0f, 1.0f);
    light[2].diffuse = glm::vec4(1.0f, 0.5f, 1.0f, 1.0f);
    light[2].specular = glm::vec4(1.0f, 0.5f, 1.0f, 1.0f);
    light[2].cutOff = 45.0f;
    light[2].outerCutOff = 60.0f;
    light[2].fallOut = 5.0f;

	light[3].type = lightType::E_SPOT;
    light[3].ambient = glm::vec4(0.5f, 1.0f, 1.0f, 1.0f);
    light[3].diffuse = glm::vec4(0.5f, 1.0f, 1.0f, 1.0f);
    light[3].specular = glm::vec4(0.5f, 1.0f, 1.0f, 1.0f);
    light[3].cutOff = 45.0f;
    light[3].outerCutOff = 60.0f;
    light[3].fallOut = 5.0f;

	light[4].type = lightType::E_SPOT;
    light[4].ambient = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);
    light[4].diffuse = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);
    light[4].specular = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);
    light[4].cutOff = 45.0f;
    light[4].outerCutOff = 60.0f;
    light[4].fallOut = 5.0f;

	light[5].type = lightType::E_SPOT;
    light[5].ambient = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    light[5].diffuse = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    light[5].specular = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    light[5].cutOff = 45.0f;
    light[5].outerCutOff = 60.0f;
    light[5].fallOut = 5.0f;
}

void OG::Scene_Assignment2::setScenario3()
{
	pOrbit->radius = 3.5f;
    rotation_speed = -3.0f;

    num_of_lights = 6;

	light[0].type = lightType::E_POINT;
    light[0].ambient = glm::vec4(1.0f);
    light[0].diffuse = glm::vec4(1.0f);
    light[0].specular = glm::vec4(1.0f);

	light[1].type = lightType::E_POINT;
    light[1].ambient = glm::vec4(1.0f);
    light[1].diffuse = glm::vec4(1.0f);
    light[1].specular = glm::vec4(1.0f);

	light[2].type = lightType::E_SPOT;
    light[2].ambient = glm::vec4(0.5f, 1.0f, 1.0f, 1.0f);
    light[2].diffuse = glm::vec4(0.5f, 1.0f, 1.0f, 1.0f);
    light[2].specular = glm::vec4(0.5f, 1.0f, 1.0f, 1.0f);

    light[3].type = lightType::E_SPOT;
    light[3].ambient = glm::vec4(1.0f, 0.5f, 1.0f, 1.0f);
    light[3].diffuse = glm::vec4(1.0f, 0.5f, 1.0f, 1.0f);
    light[3].specular = glm::vec4(1.0f, 0.5f, 1.0f, 1.0f);

    light[2].cutOff = 45.0f;
    light[2].outerCutOff = 60.0f;
    light[2].fallOut = 5.0f;

	light[4].type = lightType::E_DIR;
    light[4].ambient = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);
    light[4].diffuse = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);
    light[4].specular = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);

	light[5].type = lightType::E_SPOT;
    light[5].ambient = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    light[5].diffuse = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    light[5].specular = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

    light[5].cutOff = 45.0f;
    light[5].outerCutOff = 60.0f;
    light[5].fallOut = 5.0f;
}
