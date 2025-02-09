#include <reactphysics3d/reactphysics3d.h>

class ContactListener : public rp3d::EventListener {
public:
    void onContact(const rp3d::CollisionCallback::CallbackData& callbackData) override {
        for (rp3d::uint i = 0; i < callbackData.getNbContactPairs(); i++) {
            auto contactPair = callbackData.getContactPair(i);

            // Get the bodies involved in the collision
            rp3d::Body *body1 = contactPair.getBody1();
            rp3d::Body *body2 = contactPair.getBody2();

            //std::cout << "Collision detected between two bodies!" << std::endl;
        }
    }
};