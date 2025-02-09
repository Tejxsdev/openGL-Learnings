#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

using namespace reactphysics3d;
using namespace std;



class Player
{
private:
    struct ObjectTransform
    {
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);
    };
    Transform transform;
    Transform coliderTransform = Transform::identity();
public:
    Vector3 position;
    Quaternion orientation;
    ObjectTransform PlayerTransform;
    RigidBody *body;
    float capsuleRadius = 1.0f;
    float capsuleHeight = 2.0f;

    Player(PhysicsWorld *world, PhysicsCommon *physicsCommon);
    void debugEnabled(bool debug);
};

Player::Player(PhysicsWorld *world, PhysicsCommon *physicsCommon)
{
    position.x = PlayerTransform.Position.x;
    position.y = 15;
    position.z = PlayerTransform.Position.z;
    orientation.w = 1;
    orientation.x = PlayerTransform.Rotation.x;
    orientation.y = PlayerTransform.Rotation.y;
    orientation.z = PlayerTransform.Rotation.z;
    transform = Transform(position, orientation);

    body = world->createRigidBody(transform);
    // body->setType(BodyType::STATIC);

    CapsuleShape *capsuleShape = physicsCommon->createCapsuleShape(capsuleRadius, capsuleHeight);
    
    Collider *collider;
    collider = body->addCollider(capsuleShape, coliderTransform);

    rp3d::Transform transform = body->getTransform();
    rp3d::Quaternion q = rp3d::Quaternion::fromEulerAngles(0, rp3d::decimal(M_PI_2)*2, 0);
    transform.setOrientation(q);
    body->setTransform(transform);

    collider->getMaterial().setBounciness(0);
    collider->getMaterial().setFrictionCoefficient(0.75);
    body->setAngularLockAxisFactor(Vector3(0,0,0));
}

void Player::debugEnabled(bool debug) {
    if (debug)
    {
        body->setIsDebugEnabled(true);
    }
    else
    {
        body->setIsDebugEnabled(false);
    }
}