#include "World.h"

entt::entity World::createEntity(string name) {
    entt::entity entity = registry.create();
    addComponent<components::Name>(entity, name);
    return entity;
}