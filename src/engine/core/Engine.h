#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../../src/World.h"
#include <glm/glm.hpp>
#include "../../src/engine/renderer/Shader.h"
#include "../../src/engine/core/RenderSystem.h"
#include "../../src/engine/core/PhysicsSystem.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include <tracy/Tracy.hpp>

using json = nlohmann::json;

class Engine
{
private:
    World world;
    RenderSystem renderSystem;
    PhysicsSystem physicssystem;

    static uint32_t objectCounter;

    glm::vec3 cameraPos = glm::vec3(0.0f, -7.0f, 10.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 lightPos = glm::vec3(-2.5f, 1.0f, 1.5f);
    Shader lightShader = Shader("../shaders/light.vert", "../shaders/light.frag");
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view;
    glm::mat4 projection;
    entt::entity selectedEntity = entt::null;
    bool isProjectLoaded = false;
    GLuint fbo = 0;
    GLuint fboTexture = 0;
    GLuint rboDepth = 0;
    int framebufferWidth = 0;
    int framebufferHeight = 0;

public:
    World &getWorld() { return world; };

    void setup_imgui(GLFWwindow *window);
    void processInput(GLFWwindow *window);
    void loadSaved(const char *path);
    void saveProject(const char *path);
    void newProject(const char *path);
    void SetupFramebuffer(int width, int height);
    void run(GLFWwindow *window, int SCR_WIDTH, int SCR_HEIGHT);
};