/* Start Header -------------------------------------------------------
File Name: Scene_Assignment2.cpp
Purpose: Scene2
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
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
shaders_(),
pCamera_(std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 10.0f))),
pDiffuseTexture(std::make_unique<Texture>("metal_roof_diff_512x512.png", 0)), 
pSpecularTexture(std::make_unique<Texture>("metal_roof_spec_512x512.png", 1)),
pVisualizeTexture(std::make_unique<Texture>("visualize.png", 0)),
pOrbit(std::make_unique<Orbit>(2.5f, 300)),
pFBO(std::make_unique<FBO>())
{
    num_of_lights = 1;
    isRotating = true;
    bVisualizeTex = false;
    is_normal_vector_on = false;
    drawFaceNormal = false;
    bCalcOnCPU = false;
}

OG::Scene_Assignment2::~Scene_Assignment2()
{
}

int OG::Scene_Assignment2::Init()
{
    // create gbuffer graphics pipeline
    gbuffer_shader = std::make_unique<Shader>("GBuffer.vert", "GBuffer.frag");
    deferred_shader = std::make_unique<Shader>("DeferredShading.vert", "DeferredShading.frag");
    
    current_shader = "gbuffer_shader";

	//Initialize Uniform Buffer Block
    SetupBuffers();
    pOrbit->SetupBuffer();

    current_item = "cube";

    objects_.emplace_back(std::move(
        OBJECT->CreateObject(current_item, glm::vec3(0.0f), glm::vec3(1.0f))
    ));
    
    OBJECT->models_["cube"] = std::make_unique<Model>("OG/models/cube.obj");
	OBJECT->models_["quad"] = std::make_unique<Model>("OG/models/quad.obj");
    OBJECT->models_["4Sphere"] = std::make_unique<Model>("OG/models/4Sphere.obj");
    OBJECT->models_["bunny_high_poly"] = std::make_unique<Model>("OG/models/bunny_high_poly.obj");
    OBJECT->models_["simpleSphere"] = std::make_unique<Model>("OG/models/simpleSphere.obj");
    OBJECT->models_["sphere_mesh"] = std::make_unique<Model>(Mesh::CreateSphere(0.1f, 36, 18));
#if 0
    models_["bunny"] = std::make_unique<Model>("OG/models/bunny.obj");
    models_["cup"] = std::make_unique<Model>("OG/models/cup.obj");
    models_["sphere"] = std::make_unique<Model>("OG/models/sphere.obj");
    models_["lucy_princeton"] = std::make_unique<Model>("OG/models/lucy_princeton.obj");
    models_["sphere_modified"] = std::make_unique<Model>("OG/models/sphere_modified.obj");
    models_["starwars1"] = std::make_unique<Model>("OG/models/starwars1.obj");
    models_["triangle"] = std::make_unique<Model>("OG/models/triangle.obj");
    models_["rhino"] = std::make_unique<Model>("OG/models/rhino.obj");
#endif

    for (int i = 0; i < 16; ++i)
    {
        spheres_.emplace_back(std::move(
            OBJECT->CreateObject("sphere_mesh", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f))
        ));
    }
    
    setFramebuffer();

    deferred_shader->SetUniform1i("gbuffer_position", 0);
    deferred_shader->SetUniform1i("gbuffer_normal", 1);
    deferred_shader->SetUniform1i("gbuffer_material", 2);
    deferred_shader->SetUniform1i("gbuffer_depth", 3);

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
   
	pUBO_transform->AddSubData(0, sizeof(glm::mat4), glm::value_ptr(view));
    pUBO_transform->AddSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));

    render_deferred_objects();
    
    //render_debug_objects();

    return 0;
}

int OG::Scene_Assignment2::postRender(double dt)
{
    if (isRotating)
        angleOfRotation += rotation_speed * static_cast<float>(dt) * 100.0f;

	return 0;
}

void OG::Scene_Assignment2::render_deferred_objects()
{
	pFBO->bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


    gbuffer_shader->Use();
	gbuffer_shader->SetUniform1i("diffuse_texture", pDiffuseTexture->texNum_);
    gbuffer_shader->SetUniform1i("specular_texture", pSpecularTexture->texNum_);
    gbuffer_shader->SetUniformBlock("Transforms", 0U);

    if (!bVisualizeTex)
        pDiffuseTexture->Bind();
    else
        pVisualizeTexture->Bind();
    pSpecularTexture->Bind();


    if (bCalcOnCPU)
    {
        gbuffer_shader->SetUniform1i("uvType", 0);
    }
    else
    {
        gbuffer_shader->SetUniform1i("uvType", (GLint)(OBJECT->models_[current_item]->uvType + 1));
    }

    for (const auto& obj : objects_)
    {
        obj->setName(current_item);

        glm::vec3 scale = obj->getScale();
        glm::vec3 transform = obj->getPosition();
        glm::mat4 model = glm::translate(transform) * glm::rotate(glm::radians(obj->rotation_angle_), obj->rotation_axis_) * glm::scale(scale);

        pOrbit->model = model;

        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(pCamera_->GetViewMatrix() * model)));
        gbuffer_shader->SetUniformMatrix3fv("normalMatrix", normalMatrix);

        gbuffer_shader->SetUniformMatrix4fv("model", model);
        gbuffer_shader->SetUniform3fv("I_emissive", obj->color_);
        gbuffer_shader->SetUniform1b("isNormMapping", OBJECT->models_[current_item]->isNormMapping);

        if (OBJECT->models_.find(obj->getName()) != OBJECT->models_.end())
        {
            obj->draw();
        }
    }

    //TODO: should i render this into g-buffer?
#if 0 
    for (int i = 0; i < num_of_lights; ++i) {

        glm::vec3 scale = spheres_[i]->getScale();
        glm::vec3 transform = spheres_[i]->getPosition();

        glm::mat4 model = glm::translate(transform)* glm::scale(scale);
        
        //NOTE: i don't need normal matrix for the lights but if glsl will complain
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(pCamera_->GetViewMatrix() * model)));
        gbuffer_shader->SetUniformMatrix3fv("normalMatrix", normalMatrix);

        gbuffer_shader->SetUniformMatrix4fv("model", model);
    }
#endif

    pFBO->unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    deferred_shader->Use();

	pFBO->getAttachment("gbuffer_position")->Bind();
    pFBO->getAttachment("gbuffer_normal")->Bind();
    pFBO->getAttachment("gbuffer_material")->Bind();
    pFBO->getAttachment("gbuffer_depth")->Bind();

    Mesh::renderFullScreenQuad();

	GLsizei offset = 0;
    for (int i = 0; i < num_of_lights; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 scale = spheres_[i]->getScale();
        glm::vec3 transform = spheres_[i]->getPosition();

        model = glm::translate(transform) * glm::scale(scale);

        //lightShader->SetUniformMatrix4fv("model", model);
        //lightShader->SetUniform4fv("diffuse", light[i].diffuse);

		light[i].direction = pCamera_->GetViewMatrix() * model * (glm::vec4(objects_[0]->position_, 1.0f) - light[i].position);
        light[i].position = pCamera_->GetViewMatrix() * model * glm::vec4(spheres_[i]->position_, 0.0f);


        //OBJECT->models_[spheres_[i]->getName()]->Draw();
        /************************************* ADD LIGHT UNIFORM BUFFER DATA ****************************************************************************/
        pUBO_light->AddSubData(offset, static_cast<GLsizei>(sizeof(glm::vec4) * 6), &light[i]);
        offset += static_cast<GLsizei>(sizeof(glm::vec4) * 6);
    }

    pUBO_light->AddSubData(sizeof(Light) * 16, sizeof(glm::vec3), glm::value_ptr(att)); //96 * 4, 16
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(global_ambient)); // 96 + 32, 16
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 2, sizeof(glm::vec3), glm::value_ptr(fog_color)); // 96 + 48, 16
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 2 + sizeof(glm::vec3), sizeof(GLfloat), &near);
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 2 + sizeof(glm::vec3) + sizeof(GLfloat), sizeof(GLfloat), &far);
    pUBO_light->AddSubData(sizeof(Light) * 16 + sizeof(glm::vec4) * 2 + sizeof(glm::vec3) + sizeof(GLfloat) * 2, sizeof(GLfloat), &num_of_lights);
    /*************************************************************************************************************************************************/


    pFBO->unbind();
}

/*
* Debug objects refer orbit, ray, vertex normal, face normal...
*/
void OG::Scene_Assignment2::render_debug_objects()
{
    //lightShader->Use();

    if (pOrbit->radius != pOrbit->prev_radius) {
        pOrbit->prev_radius = pOrbit->radius;
        pOrbit->SetupBuffer();
    }

    //lightShader->SetUniformMatrix4fv("model", pOrbit->model);
    //lightShader->SetUniform4fv("diffuse", glm::vec4(1.0f));
    pOrbit->DrawOrbit();

    for (const auto& obj : objects_) {
        if (is_normal_vector_on)
            obj->drawNormal(drawFaceNormal);
    }

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
    pUBO_light = std::make_unique<UniformBuffer>(static_cast<GLsizei>((sizeof(glm::vec4) * 100)), 1U, GL_DYNAMIC_DRAW);

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

                //NOT USING shaders_ for now
#if 0
                if (ImGui::Button("Reload Shader"))
                {
                    shaders_[current_shader]->ReloadShaders();
                }
#endif

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Material"))
            {
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

/*
* pFBO: G-buffer
* 
*/
void OG::Scene_Assignment2::setFramebuffer()
{
    pFBO->init(_windowWidth, _windowHeight);


    pFBO->bind();

    std::unique_ptr<Texture> gbuffer_position = std::make_unique<GBufferTexture>(_windowWidth, _windowHeight, GL_RGBA16F, 0);
    std::unique_ptr<Texture> gbuffer_normal = std::make_unique<GBufferTexture>(_windowWidth, _windowHeight, GL_RGBA16F, 1);
    std::unique_ptr<Texture> gbuffer_material = std::make_unique<GBufferTexture>(_windowWidth, _windowHeight, GL_RGBA16F, 2);
    std::unique_ptr<Texture> gbuffer_depth = std::make_unique<GBufferTexture>(_windowWidth, _windowHeight, GL_RGBA16F, 3);

    pFBO->setTextureAttachment("gbuffer_position", gbuffer_position);
    pFBO->setTextureAttachment("gbuffer_normal", gbuffer_normal);
    pFBO->setTextureAttachment("gbuffer_material", gbuffer_material);
    pFBO->setTextureAttachment("gbuffer_depth", gbuffer_depth);

	/*
	* OpenGL by default only renders to a framebuffer's first color attachment.
	* Must explicitly tell OpenGL rendering multiple colorbuffers
	*/

    GLuint attachments[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };

    glDrawBuffers(static_cast<int>(sizeof(attachments) / sizeof(GLuint)), attachments);

    pFBO->setDepthBuffer();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer Not Complete" << std::endl;
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
