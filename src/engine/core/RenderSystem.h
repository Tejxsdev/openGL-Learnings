#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <../../src/components/Transform.h>
#include <../../src/components/MeshRenderer.h>

class RenderSystem
{
private:
    /* data */
public:
    void update(entt::registry &registry,
                glm::mat4 &model,
                glm::mat4 &view,
                glm::mat4 &projection,
                glm::vec3 &lightPos,
                glm::vec3 &cameraPos)
    {
        auto viewGroup = registry.view<components::Transform, components::MeshRenderer>();

        for (auto entity : viewGroup)
        {
            auto &transform = viewGroup.get<components::Transform>(entity);
            auto &meshComp = viewGroup.get<components::MeshRenderer>(entity);
            meshComp.shader->use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);
            model = glm::scale(model, transform.scale);

            meshComp.shader->setMat4("model", &model[0][0]);
            meshComp.shader->setMat4("projection", &projection[0][0]);
            meshComp.shader->setMat4("view", &view[0][0]);
            meshComp.shader->setVec3("lights[0].position", lightPos);
            meshComp.shader->setVec3("lights[0].ambient", glm::vec3(0.2f));
            meshComp.shader->setVec3("lights[0].diffuse", glm::vec3(0.5f));
            meshComp.shader->setVec3("lights[0].specular", glm::vec3(1.0f));
            meshComp.shader->setVec3("viewPos", cameraPos);

            meshComp.model->Draw(*meshComp.shader);
        }
    }
};