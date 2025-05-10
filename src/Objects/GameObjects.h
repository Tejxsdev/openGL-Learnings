#include "../../src/Engine/Renderer/model.h"
#include "../../src/Engine/Renderer/shader.h"
#include "GRigidBody.h"
#include <cstddef>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <reactphysics3d/engine/PhysicsCommon.h>
#include <vector>

using namespace std;

class GameObjects {
private:
  vector<float> position;
  vector<float> rotation;
  vector<float> scale;
  PhysicsWorld *world;
  shader material = shader("../shaders/color.vert", "../shaders/color.frag");

public:
  Model mesh = Model("../resources/sofa/source/ready.obj");

  GRigidBody rb = GRigidBody();
  GameObjects(PhysicsWorld *world);
  void render(glm::mat4 projection, glm::mat4 model, glm::mat4 view,
              glm::vec3 lightPos, glm::vec3 cameraPos);
  void addRigidBody();
  void addConcaveCollider(Model mesh, PhysicsCommon physicsCommon);
};

GameObjects::GameObjects(PhysicsWorld *world) {
  this->world = world;
  position = {0.0f, 50.0f, -1.0f};
  rotation = {0.0f, 0.0f, 0.0f};
  scale = {1.0f, 1.0f, 1.0f};
}

void GameObjects::render(glm::mat4 projection, glm::mat4 model, glm::mat4 view,
                         glm::vec3 lightPos, glm::vec3 cameraPos) {
  float angle =
      2.0f *
      acos(rb.body->getTransform().getOrientation().w); // Angle in radians

  material.use();
  model = glm::mat4(1.0f);
  if (rb.body == nullptr) {
    cout << world;
    model =
        glm::translate(model, glm::vec3(position[0], position[1], position[2]));
  } else {

    model = glm::translate(model,
                           glm::vec3(rb.body->getTransform().getPosition().x,
                                     rb.body->getTransform().getPosition().y,
                                     rb.body->getTransform().getPosition().z));
  }
  model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
  model = glm::rotate(
      model, angle,
      glm::vec3(rb.body->getTransform().getOrientation().getVectorV().x,
                0.0000000000001f +
                    rb.body->getTransform().getOrientation().getVectorV().y,
                rb.body->getTransform().getOrientation().getVectorV().z));
  material.setMat4("model", &model[0][0]);
  material.setMat4("projection", &projection[0][0]);
  material.setMat4("view", &view[0][0]);
  material.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
  material.setVec3("light.position", lightPos);
  material.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
  material.setVec3("viewPos", cameraPos);
  material.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

  mesh.Draw(material);
}

void GameObjects::addRigidBody() {
  rb = GRigidBody(this->world, Vector3(position[0], position[1], position[2]));
}

void GameObjects::addConcaveCollider(Model mesh, PhysicsCommon physicsCommon) {}
