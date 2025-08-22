#include <entt/entt.hpp>
#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/mathematics/Vector3.h>
#include <../../src/components/Transform.h>
#include <../../src/components/RigidBody.h>
#include <glm/glm.hpp>

class PhysicsSystem
{
private:
    rp3d::PhysicsCommon physicsCommon;
    rp3d::PhysicsWorld *world;
    const float timeStep = 1.0f / 60.0f;

public:
    void setup(entt::registry &registry)
    {
        world = physicsCommon.createPhysicsWorld();
        updateWorld(registry);
    }

    void createRigidBody(entt::registry &registry)
    {
        auto viewGroup = registry.view<components::Transform, components::RigidBody>();
        for (auto entity : viewGroup)
        {
            auto &transform = viewGroup.get<components::Transform>(entity);
            auto &rigidbody = viewGroup.get<components::RigidBody>(entity);
            rp3d::Transform pTransform(rp3d::Vector3(transform.position.x, transform.position.y, transform.position.z), rigidbody.orientation);
            rigidbody.rigidbody = world->createRigidBody(pTransform);
        }
    }

    void update(entt::registry &registry)
    {
        world->update(timeStep);
        auto viewGroup = registry.view<components::Transform, components::RigidBody>();
        for (auto entity : viewGroup)
        {
            auto &transform = viewGroup.get<components::Transform>(entity);
            auto &rigidbody = viewGroup.get<components::RigidBody>(entity);
            transform.position = glm::vec3(rigidbody.rigidbody->getTransform().getPosition().x, rigidbody.rigidbody->getTransform().getPosition().y, rigidbody.rigidbody->getTransform().getPosition().z);
        }
    }

    void updateWorld(entt::registry &registry)
    {
        auto viewGroup = registry.view<components::Transform, components::RigidBody>();
        for (auto entity : viewGroup)
        {
            auto &transform = viewGroup.get<components::Transform>(entity);
            auto &rigidbody = viewGroup.get<components::RigidBody>(entity);
            rp3d::Transform pTransform(rp3d::Vector3(transform.position.x, transform.position.y, transform.position.z), rigidbody.orientation);
            rigidbody.rigidbody = world->createRigidBody(pTransform);
        }
    }
};