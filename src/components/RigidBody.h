#include <reactphysics3d/reactphysics3d.h>

namespace components {
    struct RigidBody
    {
        rp3d::RigidBody *rigidbody;
        rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    };
    
}