#include <entt/entt.hpp>
#include <../../src/components/Transform.h>
#include <../../src/components/Name.h>

class World
{
private:
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
};