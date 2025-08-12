#include "Engine.h"

uint32_t Engine::objectCounter = 0;

void Engine::setup_imgui(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
}

void Engine::processInput(GLFWwindow *window)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard)
    {
        float moveSpeed = 5.0f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPos += cameraFront * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPos -= cameraFront * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            cameraPos -= glm::normalize(glm::cross(cameraUp, cameraFront)) * moveSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            cameraPos += glm::normalize(glm::cross(cameraUp, cameraFront)) * moveSpeed;
        }
    }
}

void Engine::loadSaved(const char *path)
{
    std::ifstream in(path);
    if (in.is_open())
    {
        json scene;
        in >> scene;
        for (const auto &obj : scene)
        {
            entt::entity entity = world.createEntity(obj["name"].get<std::string>());

            glm::vec3 pos = {obj["transform"]["position"][0], obj["transform"]["position"][1], obj["transform"]["position"][2]};
            glm::vec3 rot = {obj["transform"]["rotation"][0], obj["transform"]["rotation"][1], obj["transform"]["rotation"][2]};
            glm::vec3 scale = {obj["transform"]["scale"][0], obj["transform"]["scale"][1], obj["transform"]["scale"][2]};

            world.addComponent<components::Transform>(entity, pos, rot, scale);

            if (obj.contains("meshRenderer"))
            {
                string modelPath = obj["meshRenderer"]["model_path"];
                cout << modelPath;
                string fragShaderPath = obj["meshRenderer"]["frag_shader_path"];
                string vertShaderPath = obj["meshRenderer"]["vert_shader_path"];

                Model *model = new Model(modelPath);
                Shader *mShader = new Shader("../shaders/color.vert", "../shaders/color.frag");

                world.addComponent<components::MeshRenderer>(entity, mShader, model);
            }

            if (obj.contains("rigidbody") && obj["rigidbody"]["rb"] == true)
            {
                world.addComponent<components::RigidBody>(entity);
                physicssystem.setup(world.Registry(), true);
            }
        }
        isProjectLoaded = true;
    }
}

void Engine::saveProject(const char *path)
{
    entt::registry &registry = world.Registry();
    json scene = json::array();

    for (auto entity : registry.view<components::Transform>())
    {
        json obj;
        obj["id"] = (uint64_t)entity;

        auto &n = registry.get<components::Name>(entity);
        obj["name"] = n.name;

        // Transform Component
        auto &t = registry.get<components::Transform>(entity);
        obj["transform"] = {
            {"position", {t.position.x, t.position.y, t.position.z}},
            {"rotation", {t.rotation.x, t.rotation.y, t.rotation.z}},
            {"scale", {t.scale.x, t.scale.y, t.scale.z}}};

        // MeshRenderer Component
        if (world.Registry().all_of<components::MeshRenderer>(entity))
        {
            auto &mesh = registry.get<components::MeshRenderer>(entity);
            obj["meshRenderer"] = {
                {"model_path", mesh.model->pathToModel},
                {"frag_shader_path", mesh.shader->fragShaderSourcePath},
                {"vert_shader_path", mesh.shader->vertShaderSourcePath},
            };
        }

        // RigidBody Component
        if (world.Registry().all_of<components::RigidBody>(entity))
        {
            if (world.Registry().all_of<components::RigidBody>(entity))
            {
                obj["rigidbody"] = {
                    {"rb", true},
                };
            }
        }

        scene.push_back(obj);
    }

    std::ofstream file(path);
    file << scene;
}

void Engine::newProject(const char *path)
{
    world.Registry().clear();
    saveProject(path);
    loadSaved(path);
}

void Engine::SetupFramebuffer(int width, int height)
{
    framebufferWidth = width;
    framebufferHeight = height;

    if (fbo)
    {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &fboTexture);
        glDeleteRenderbuffers(1, &rboDepth);
    }

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, fboTexture, 0);

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::run(GLFWwindow *window, int SCR_WIDTH, int SCR_HEIGHT)
{
    ZoneScoped;
    setup_imgui(window);
    SetupFramebuffer(SCR_WIDTH, SCR_HEIGHT);
    physicssystem.setup(world.Registry(), false);

    // Lighting
    float vertices[] = {
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f};

    unsigned int lightVAO, lightVBO;

    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0); // Position
    glEnableVertexAttribArray(0);
    auto viewGroup = world.Registry().view<components::Transform>();

    while (!glfwWindowShouldClose(window))
    {
        ZoneScoped;
        // Input
        processInput(window);
        static char projectPath[256] = "";

        // Bind the framebuffer for offscreen rendering
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("DockSpace Demo", nullptr, window_flags);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {
                    world.Registry().clear();
                    isProjectLoaded = false;
                    selectedEntity = entt::null;
                }
                if (ImGui::MenuItem("Save"))
                {
                    if (isProjectLoaded)
                    {
                        saveProject(projectPath);
                    }
                }
                if (ImGui::MenuItem("Open"))
                {
                    world.Registry().clear();
                    selectedEntity = entt::null;
                    isProjectLoaded = false;
                }
                if (ImGui::MenuItem("Exit"))
                {
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        ImGui::End();
        ImGui::PopStyleVar(2);

        if (!isProjectLoaded)
        {
            ImGui::ShowDemoWindow();
            ImGui::Begin("Menu");
            ImGui::InputText("Saved Path", projectPath, sizeof(projectPath));

            if (ImGui::Button("Open Saved"))
            {
                loadSaved(projectPath);
            }

            if (ImGui::Button("New"))
            {
                newProject(projectPath);
            }
        }
        else
        {

            projection =
                glm::perspective(glm::degrees(100.0f),
                                 (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            view = glm::rotate(view, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            {
            ZoneScopedN("PhysicsUpdate");
            physicssystem.update(world.Registry());

            }

            {
            ZoneScopedN("RenderScene");
            renderSystem.update(world.Registry(), model, view, projection, lightPos, cameraPos);

            }

            // Lights
            lightShader.use();
            glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1,
                               GL_FALSE, &projection[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1,
                               GL_FALSE, &view[0][0]);
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
            glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1,
                               GL_FALSE, &model[0][0]);
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));

            ImGui::Begin("Viewport");
            ImVec2 availSize = ImGui::GetContentRegionAvail();
            ImGui::Image((void *)(intptr_t)fboTexture, availSize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();

            static char pathInput[256] = "";
            ImGui::Begin("Scene");
            for (auto entity : viewGroup)
            {
                if (ImGui::Button(world.getComponent<components::Name>(entity).name.c_str(), ImVec2(100, 20)))
                {
                    selectedEntity = entity;
                    memset(pathInput, 0, sizeof(pathInput));
                }
            }
            if (ImGui::Button("Create new gameobject"))
            {
                entt::entity newEntity = world.createEntity("GameObject " + to_string(objectCounter++));
                world.addComponent<components::Transform>(newEntity);
            }

            ImGui::End();

            ImGui::Begin("Inspector", &isProjectLoaded);
            if (selectedEntity != entt::null)
            {
                auto &transform = world.Registry().get<components::Transform>(selectedEntity);
                float pos[3] = {transform.position.x, transform.position.y, transform.position.z};
                float rot[3] = {transform.rotation.x, transform.rotation.y, transform.rotation.z};
                float scale[3] = {transform.scale.x, transform.scale.y, transform.scale.z};

                static char objName[256];
                std::strncpy(objName, world.getComponent<components::Name>(selectedEntity).name.c_str(), sizeof(world.getComponent<components::Name>(selectedEntity).name));
                if (ImGui::InputText("#Name: ", objName, sizeof(objName)) && objName[0] != NULL)
                {
                    world.getComponent<components::Name>(selectedEntity).name = objName;
                }

                if (ImGui::CollapsingHeader("Transform"))
                {
                    if (ImGui::DragFloat3("Position ", pos))
                    {
                        transform.position = glm::vec3(pos[0], pos[1], pos[2]);
                    }
                    if (ImGui::DragFloat3("Rotation ", rot))
                    {
                        transform.rotation = glm::vec3(rot[0], rot[1], rot[2]);
                    }
                    if (ImGui::DragFloat3("Scale ", scale))
                    {
                        transform.scale = glm::vec3(scale[0], scale[1], scale[2]);
                    }
                }

                if (world.Registry().all_of<components::MeshRenderer>(selectedEntity))
                {
                    if (ImGui::CollapsingHeader("Mesh Renderer"))
                    {
                        auto &mesh = world.Registry().get<components::MeshRenderer>(selectedEntity);
                        ImGui::Text("Path: %s", mesh.model->dir.c_str());
                    }
                }
                if (ImGui::Button("Add RigidBody Component"))
                {
                    if (!world.Registry().all_of<components::RigidBody>(selectedEntity))
                    {
                        world.addComponent<components::RigidBody>(selectedEntity);
                        physicssystem.setup(world.Registry(), true);
                    }
                }
                ImGui::InputText("Model Path", pathInput, sizeof(pathInput));

                if (ImGui::Button("Add Mesh Component"))
                {
                    if (!world.Registry().all_of<components::MeshRenderer>(selectedEntity))
                    {
                        Model *model = new Model(pathInput);
                        Shader *planeShader = new Shader("../shaders/color.vert", "../shaders/color.frag");
                        world.addComponent<components::MeshRenderer>(selectedEntity, planeShader, model);
                    }
                }
            }
        }

        ImGui::End();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::Render();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
