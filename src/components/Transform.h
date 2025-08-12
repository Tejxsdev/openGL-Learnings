#pragma once
#include <glm/glm.hpp>

namespace components
{
    struct  Transform
    {
        glm::vec3 position = {1.0f, 1.0f, 1.0f};
        glm::vec3 rotation = {1.0f, 1.0f, 1.0f};
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    };
}