/* Start Header -------------------------------------------------------
File Name: Scene_Assignment1.cpp
Purpose: Scene1, Draw Lines for orbit, load and render models, and process Input for camera movement
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OG Renderer
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#include "ogpch.h"
#include "Scene_Assignment1.h"
#include "Object.h"
#include "Camera.h"

#include "BufferObject.h"
#include "UniformBuffer.h"
#include "ArrayBufferObject.h"
#include "VertexAttributeManager.h"

OG::Scene_Assignment1::Scene_Assignment1(int windowWidth, int windwoHeight) : Scene(windowWidth, windwoHeight),
shader(std::make_unique<Shader>()), pCamera_(std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f)))
{
    drawFaceNormal = false;
    is_normal_vector_on = false;
}

OG::Scene_Assignment1::~Scene_Assignment1()
{
    CleanUp();
}

int OG::Scene_Assignment1::Init()
{
    // Create and compile our GLSL program from the shaders
    shader->LoadShaders("OG/shaders/PKO.vert",
        "OG/shaders/PKO.frag");
    shader->Use();

    // Get Location of Uniform from Shader and link it to 'binding point'
    shader->SetUniformBlock("Transforms", 0);
    shader->SetUniformBlock("LightInfo", 1);

    // Set Geometry container
    models_["cube2"] = std::make_unique<Model>("OG/models/cube2.obj");
#if 0
    models_["cube"] = std::make_unique<Model>("OG/models/cube.obj");
    models_["cup"] = std::make_unique<Model>("OG/models/cup.obj");
    models_["sphere"] = std::make_unique<Model>("OG/models/sphere.obj");
    models_["bunny"] = std::make_unique<Model>("OG/models/bunny.obj");
    models_["4Sphere"] = std::make_unique<Model>("OG/models/4Sphere.obj");
    models_["bunny_high_poly"] = std::make_unique<Model>("OG/models/bunny_high_poly.obj");
    models_["lucy_princeton"] = std::make_unique<Model>("OG/models/lucy_princeton.obj");
    models_["quad"] = std::make_unique<Model>("OG/models/quad.obj");
    models_["simpleSphere"] = std::make_unique<Model>("OG/models/simpleSphere.obj");
    models_["sphere_modified"] = std::make_unique<Model>("OG/models/sphere_modified.obj");
    models_["starwars1"] = std::make_unique<Model>("OG/models/starwars1.obj");
    models_["triangle"] = std::make_unique<Model>("OG/models/triangle.obj");
    models_["rhino"] = std::make_unique<Model>("OG/models/rhino.obj");
#endif // 0


    models_["sphere_mesh"] = std::make_unique<Model>(Mesh::CreateSphere(0.1f, 36, 18));

    current_item = "cube2";

    objects_.emplace_back(std::move(
        OBJECT->CreateObject(current_item, glm::vec3(0.0f), glm::vec3(1.0f))
    ));

    for (int i = 0; i < 8; ++i)
    {
        float theta = i * 360.0f / 8.0f;
        
        spheres_.emplace_back(std::move(
            OBJECT->CreateObject("sphere_mesh", glm::vec3(2 * cos(glm::radians(theta)), 0.0f, 2 * sin(glm::radians(theta))), glm::vec3(1.0f))
        ));
    }


    SetOrbit(2.0f, 100);
    SetupBuffers();


    pUBO_light->AddSubData(0, sizeof(glm::vec4), glm::value_ptr(lightColor));
    pUBO_light->AddSubData(sizeof(glm::vec4), sizeof(GLfloat), &ambientStrength);

    return Scene::Init();
}

void OG::Scene_Assignment1::CleanUp()
{
    shader->CleanUp();
}

int OG::Scene_Assignment1::Render(double dt)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_DEBUG_OUTPUT);
    //glCullFace(GL_BACK);

    shader->Use();

    glm::mat4 projection =
        glm::perspective(glm::radians(pCamera_->Zoom), (float)_windowWidth / (float)_windowHeight, 0.1f, 100.0f);
    glm::mat4 view = pCamera_->GetViewMatrix();


    shader->SetUniform3fv("lightPos", lightPos);
    //pUBO_light->AddSubData(0, sizeof(glm::vec4), glm::value_ptr(lightPos));
    // Add Actual Data to Uniform Buffer
    pUBO_transform->AddSubData(0, sizeof(glm::mat4), glm::value_ptr(view));
    pUBO_transform->AddSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));

	 glm::mat4 model = glm::translate(glm::vec3(0,0,0)) * glm::scale(glm::vec3(1,1,1));

     glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
        
     shader->SetUniformMatrix3fv("normalMatrix", normalMatrix);
     shader->SetUniformMatrix4fv("model", model);

     DrawOrbit();

    for (const auto& sphere : spheres_)
    {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 scale = sphere->getScale();
        glm::vec3 transform = sphere->getPosition();
        glm::vec3 color = sphere->getColor() + glm::vec3(1.0f, 0.5f, 0.0f);
        
        model = glm::rotate(angleOfRotation, glm::vec3(0, 1, 0))* glm::translate(transform) * glm::scale(scale);

        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));
        
        shader->SetUniformMatrix3fv("normalMatrix", normalMatrix);
        shader->SetUniformMatrix4fv("model", model);
        shader->SetUniform3fv("objectColor", color);

        if (models_.find(sphere->getName()) != models_.end())
            models_[sphere->getName()]->Draw(shader.get());
    }

    for (const auto& obj : objects_)
    {
        obj->setName(current_item);

        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 scale = obj->getScale();
        glm::vec3 transform = obj->getPosition();
        glm::vec3 color = obj->getColor();

        model = glm::translate(transform) * glm::rotate(angleOfRotation, glm::vec3(1, 1, 1))* glm::scale(scale);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model)));

        shader->SetUniformMatrix3fv("normalMatrix", normalMatrix);
        shader->SetUniformMatrix4fv("model", model);
        shader->SetUniform3fv("objectColor", color);

        if (models_.find(obj->getName()) != models_.end())
        {
            if (is_normal_vector_on) {
                obj->drawNormal(drawFaceNormal);
            }

            obj->draw(shader.get());
        }
    }

	return 0;
}

int OG::Scene_Assignment1::postRender(double dt)
{
    angleOfRotation += static_cast<float>(dt);

	return 0;
}

void OG::Scene_Assignment1::SetupBuffers()
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

    // Generate UNIFORM_BUFFER, and Set 'binding point' as an 'index'
    pUBO_transform = std::make_unique<UniformBuffer>(static_cast<GLsizei>(2 * sizeof(glm::mat4)), 0U, GL_DYNAMIC_DRAW);
    pUBO_light = std::make_unique<UniformBuffer>(static_cast<GLsizei>((sizeof(glm::vec4)) + sizeof(glm::vec3)), 1U, GL_STATIC_DRAW);
}

void OG::Scene_Assignment1::SetupImGui(GLFWwindow* pWindow)
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

void OG::Scene_Assignment1::DrawImGui(GLFWwindow* pWindow)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("CS300 Assignment 1");

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

void OG::Scene_Assignment1::CleanupImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void OG::Scene_Assignment1::keyboardInput(GLFWwindow* pWindow, float dt)
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

/*
* Functions For Orbit VertexBuffer and Drawing
* 
* 		std::vector<glm::vec3> orbit_points;
*       std::vector<GLuint> orbit_indices;
 		std::unique_ptr<ArrayBufferObject> pVAO;
        std::unique_ptr<BufferObject> pVBO;
        std::unique_ptr<BufferObject> pEBO;
* 
*/
void OG::Scene_Assignment1::SetOrbit(float radius, int numberOfPoints)
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

void OG::Scene_Assignment1::DrawOrbit()
{
    pVAO->Bind();
    glDrawElements(GL_LINES, static_cast<GLsizei>(orbit_indices.size()), GL_UNSIGNED_INT, 0);
    pVAO->UnBind();
}
