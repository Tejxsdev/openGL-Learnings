#include <entt/entt.hpp>
#include <../../src/components/Transform.h>
#include <../../src/components/Name.h>
#include <../../src/components/Camera.h>
#include <../../src/components/Tag.h>
#include <vector>

class World
{
private:
    std::vector<std::string> listOfTags = {"none"};
    entt::registry registry;
public:
    entt::registry& Registry() {return registry; }
    entt::entity createEntity(string name);
    template<typename T, typename... Args>
    T& addComponent(entt::entity entity, Args&&... args) {
        return registry.emplace<T>(entity, std::forward<Args>(args)...);
    }
    template<typename T>
    T& getComponent(entt::entity entity) {
        return registry.get<T>(entity);
    }
    entt::entity findGameObjectsWithName(string s) {
        auto viewGroup = registry.view<components::Name>();
        for (auto entity : viewGroup)
        {
            if (getComponent<components::Name>(entity).name == s)
            {
                return entity;
            }
        }
        return entt::null;
    }
    void addTag(entt::entity entity, string tag) {
        getComponent<components::Tag>(entity).Tag = tag;
    }
};