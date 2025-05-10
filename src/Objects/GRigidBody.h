#include "../Engine/Renderer/model.h"
#include <reactphysics3d/collision/shapes/ConcaveMeshShape.h>
#include <reactphysics3d/engine/PhysicsCommon.h>
#include <reactphysics3d/reactphysics3d.h>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace reactphysics3d;

class GRigidBody {
private:
  /* data */
public:
  RigidBody *body = nullptr;
  GRigidBody();
  GRigidBody(PhysicsWorld *world, Vector3 position);
  void addConcaveCollider(Model mesh, PhysicsCommon &physicsCommon);
};

GRigidBody::GRigidBody() {}

GRigidBody::GRigidBody(PhysicsWorld *world, Vector3 position) {
  Quaternion orientation = Quaternion::identity();
  Transform transform(position, orientation);
  body = world->createRigidBody(transform);
}

void GRigidBody::addConcaveCollider(Model mesh, PhysicsCommon &physicsCommon) {
  vector<float> verticesB;
  vector<int> indexB;
  vector<float> normalB;
  Vector3 scaling(1.0, 1.0, 1.0);
  Transform transformC = Transform::identity();
  vector<ConcaveMeshShape *> ListOfConcaveMeshShape;

  for (int k = 0; k < mesh.meshes.size(); k++) {
    for (int i = 0; i < mesh.meshes[k].vertices.size(); i++) {
      verticesB.push_back(mesh.meshes[k].vertices[i].Position.x);
      verticesB.push_back(mesh.meshes[k].vertices[i].Position.y);
      verticesB.push_back(mesh.meshes[k].vertices[i].Position.z);
      normalB.push_back(mesh.meshes[k].vertices[i].Normal.x);
      normalB.push_back(mesh.meshes[k].vertices[i].Normal.y);
      normalB.push_back(mesh.meshes[k].vertices[i].Normal.z);
    }
    for (int j = 0; j < mesh.meshes[k].indices.size(); j++) {
      indexB.push_back(mesh.meshes[k].indices[j]);
      if (j == mesh.meshes[k].indices.size() - 1) {
        rp3d::TriangleVertexArray triangleArray(
            mesh.vertexCount[k], verticesB.data(), 3 * sizeof(float),
            normalB.data(), 3 * sizeof(float), mesh.triangleCount[k],
            indexB.data(), 3 * sizeof(uint32_t),
            rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
            rp3d::TriangleVertexArray::NormalDataType::NORMAL_FLOAT_TYPE,
            rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);
        std::vector<rp3d::Message> messages;
        TriangleMesh *triangleMesh =
            physicsCommon.createTriangleMesh(triangleArray, messages);
        ConcaveMeshShape *concaveMeshShape =
            physicsCommon.createConcaveMeshShape(triangleMesh, scaling);
        ListOfConcaveMeshShape.push_back(concaveMeshShape);

        indexB.clear();
        verticesB.clear();
        normalB.clear();
      }
    }
  }
  Collider *collider;
  for (auto mesh : ListOfConcaveMeshShape) {
    collider = body->addCollider(mesh, transformC);
  }
}
