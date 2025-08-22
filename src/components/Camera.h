#include <glm/glm.hpp>

namespace components {
    struct Camera
    {
        glm::vec4 CamersFront = glm::vec4(0, 0, -1, 0);
        glm::vec4 CamersUp = glm::vec4(0, 1, 0, 0);
    };
    
}