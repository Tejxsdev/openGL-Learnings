#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <reactphysics3d/collision/shapes/ConcaveMeshShape.h>
#include <reactphysics3d/reactphysics3d.h>
#include "../../src/Engine/Renderer/shader.h"
#include "glm/fwd.hpp"
#include "stb_image.h"
#include "../../src/Engine/Renderer/model.h"
#include "../../src/Engine/Renderer/Window.h"
#include "../../src/Objects/Player.h"
#include "../../src/Engine/Physics/ContactListener.h"
#include "../../src/Objects/GameObjects.h"

using namespace reactphysics3d;

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1000;
unsigned int loadTexture(char const *path);

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

glm::vec3 lightPos(-2.5f, 1.0f, 1.5f);

// Camera Setup
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
// backpack pos
glm::vec3 backPackPos = glm::vec3(0.0f, 0.0f, 0.0f);

float deltaTime = 0.0f; // Time between current frame and lst frame
float lastFrame = 0.0f; // Time of last frame

PhysicsCommon physicsCommon;
PhysicsWorld *world = physicsCommon.createPhysicsWorld();
ContactListener contactListener; 

Player player(world, &physicsCommon);
Vector3 positionG(0, -3, 0);
Quaternion orientationG = Quaternion::identity();
Transform transformG(positionG, orientationG);
RigidBody *groundBody = world->createRigidBody(transformG);

int main()
{
  // OpenGL
  Window *win = new Window(SCR_WIDTH, SCR_HEIGHT, "MyGame");
  // shaders
  shader ourShader("../shaders/color.vert", "../shaders/color.frag");
  shader lightShader("../shaders/light.vert", "../shaders/light.frag");
  shader debugShader("../shaders/debug.vert", "../shaders/debug.frag");
  shader planeShader("../shaders/color.vert", "../shaders/color.frag");

  glEnable(GL_DEPTH_TEST);

  Model ourModel("../resources/backpack/backpack.obj");
  GameObjects sofa(world);
  int count = 0;
  int iCount = 0;

  cout << count << endl;
  const int nV = count * 3;

  vector<float> verticesB;
  vector<int> indexB;
  vector<float> normalB;
  Vector3 scaling(1.0, 1.0, 1.0);
  Transform transformC = Transform::identity();
  vector<ConcaveMeshShape *> ListOfConcaveMeshShape;

  world->setEventListener(&contactListener);

  for (int k = 0; k < ourModel.meshes.size(); k++)
  {
    for (int i = 0; i < ourModel.meshes[k].vertices.size(); i++)
    {
      verticesB.push_back(ourModel.meshes[k].vertices[i].Position.x);
      verticesB.push_back(ourModel.meshes[k].vertices[i].Position.y);
      verticesB.push_back(ourModel.meshes[k].vertices[i].Position.z);
      normalB.push_back(ourModel.meshes[k].vertices[i].Normal.x);
      normalB.push_back(ourModel.meshes[k].vertices[i].Normal.y);
      normalB.push_back(ourModel.meshes[k].vertices[i].Normal.z);
    }
    for (int j = 0; j < ourModel.meshes[k].indices.size(); j++)
    {
      indexB.push_back(ourModel.meshes[k].indices[j]);
      if (j == ourModel.meshes[k].indices.size() - 1)
      {
        rp3d::TriangleVertexArray triangleArray(
            ourModel.vertexCount[k], verticesB.data(), 3 * sizeof(float),
            normalB.data(), 3 * sizeof(float), ourModel.triangleCount[k],
            indexB.data(), 3 * sizeof(uint32_t),
            rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
            rp3d::TriangleVertexArray::NormalDataType::NORMAL_FLOAT_TYPE,
            rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);
        std::vector<rp3d::Message> messages;
        TriangleMesh *triangleMesh = physicsCommon.createTriangleMesh(triangleArray, messages);
        ConcaveMeshShape *concaveMeshShape =
            physicsCommon.createConcaveMeshShape(triangleMesh, scaling);
        ListOfConcaveMeshShape.push_back(concaveMeshShape);

        indexB.clear();
        verticesB.clear();
        normalB.clear();
      }
    }
  }

  Model ground("../resources/plane/plane.obj");

  glm::mat4 model = glm::mat4(1.0f);

  world->setIsDebugRenderingEnabled(true);

  sofa.addRigidBody();
  sofa.rb.addConcaveCollider(sofa.mesh, physicsCommon);

  Vector3 position(0, 0, 0);
  Quaternion orientation = Quaternion::identity();
  Transform transform(position, orientation);
  RigidBody *body = world->createRigidBody(transform);
  body->setMass(0.1f);
  // body->setType(BodyType::STATIC);

  Collider *collider;
  for (auto mesh : ListOfConcaveMeshShape)
  {
    collider = body->addCollider(mesh, transformC);
  }

  // Create a rigidbody for ground
  groundBody->setType(BodyType::STATIC);
  Vector3 halfExtentsG(50.0, 1, 50.0);
  BoxShape *boxG = physicsCommon.createBoxShape(halfExtentsG);
  // Relative transform of the collider relative to the body origin
  Transform transformGC = Transform::identity();

  // Add the collider to the rigid body
  Collider *colliderG;
  colliderG = groundBody->addCollider(boxG, transformGC);
  const decimal timeStep = 1.0f / 60.0f;

  body->setIsDebugEnabled(true);
  groundBody->setIsDebugEnabled(true);
  player.debugEnabled(true);

  DebugRenderer &debugRenderer = world->getDebugRenderer();

  // Select the contact points and contact normals to be displayed
  debugRenderer.setIsDebugItemDisplayed(DebugRenderer::DebugItem::CONTACT_POINT,
                                        true);
  debugRenderer.setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLIDER_AABB,
                                        true);
  debugRenderer.setIsDebugItemDisplayed(
      DebugRenderer::DebugItem::COLLISION_SHAPE, true);
  debugRenderer.setIsDebugItemDisplayed(
      DebugRenderer::DebugItem::CONTACT_NORMAL, true);
  debugRenderer.setIsDebugItemDisplayed(
      DebugRenderer::DebugItem::COLLISION_SHAPE_NORMAL, true);
  debugRenderer.setIsDebugItemDisplayed(
      DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);

  glm::vec3 euler = glm::vec3(0, 0, 0);
  float angle =
      2.0f * acos(body->getTransform().getOrientation().w); // Angle in radians

  // Lighting
  float vertices[] = {
      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,
      1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, 1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, -1.0f,
      1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,
      1.0f, -1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, 1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 1.0f};

  unsigned int lightVAO, lightVBO;
  
  glGenVertexArrays(1, &lightVAO);
  glGenBuffers(1, &lightVBO);

  glBindVertexArray(lightVAO);
  glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
               vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (void *)0); // Position
  glEnableVertexAttribArray(0);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glfwSetInputMode(win->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(win->window, mouse_callback);

  unsigned int DebugVAO, DebugVBO;
  while (!glfwWindowShouldClose(win->window))
  {
    rp3d::Transform pTransform = player.body->getTransform();
    cameraPos.x = -pTransform.getPosition().x; 
    cameraPos.y = -pTransform.getPosition().y; 
    cameraPos.z = pTransform.getPosition().z; 

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    float timeValue = (float)glfwGetTime();

    lightPos.y = sin(currentFrame);

    world->update(timeStep);

    // Get the updated position of the body
    const Transform &transform = body->getTransform();
    const Vector3 &position = transform.getPosition();

    // Input
    processInput(win->window);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view;
    glm::mat4 projection =
        glm::perspective(glm::degrees(150.0f),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    view = glm::rotate(view, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // Transform Ptransform(pos, player.orientation);
    // player.body->setTransform(Ptransform);

    angle = 2.0f * acos(body->getTransform().getOrientation().w);
    sofa.render(projection,model,view, lightPos, cameraPos);

    ourShader.use();
    model = glm::mat4(1.0f);
    model =
        glm::translate(model, glm::vec3(body->getTransform().getPosition().x,
                                        body->getTransform().getPosition().y,
                                        body->getTransform().getPosition().z));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::rotate(
        model, angle,
        glm::vec3(body->getTransform().getOrientation().getVectorV().x,
                  0.0000000000001f +
                      body->getTransform().getOrientation().getVectorV().y,
                  body->getTransform().getOrientation().getVectorV().z));
    ourShader.setMat4("model", &model[0][0]);
    ourShader.setMat4("projection", &projection[0][0]);
    ourShader.setMat4("view", &view[0][0]);
    ourShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    ourShader.setVec3("light.position", lightPos);
    ourShader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    ourShader.setVec3("viewPos", cameraPos);
    ourShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    ourModel.Draw(ourShader);

    planeShader.use();

    model = glm::mat4(1.0f);
    model = glm::translate(
        model, glm::vec3(groundBody->getTransform().getPosition().x,
                         groundBody->getTransform().getPosition().y + 1,
                         groundBody->getTransform().getPosition().z));
    model = glm::scale(model, glm::vec3(50, 50, 50));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    planeShader.setMat4("model", &model[0][0]);

    planeShader.setMat4("projection", &projection[0][0]);
    planeShader.setMat4("view", &view[0][0]);
    planeShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    planeShader.setVec3("light.position", lightPos);
    planeShader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    planeShader.setVec3("viewPos", cameraPos);
    planeShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    ground.Draw(planeShader);

    const rp3d::Array<rp3d::DebugRenderer::DebugTriangle> &line =
        debugRenderer.getTriangles();

    vector<float> vertexArr;

    for (auto points : line)
    {
      vertexArr.push_back(points.point1.x);
      vertexArr.push_back(points.point1.y);
      vertexArr.push_back(points.point1.z);

      vertexArr.push_back(points.point2.x);
      vertexArr.push_back(points.point2.y);
      vertexArr.push_back(points.point2.z);

      vertexArr.push_back(points.point3.x);
      vertexArr.push_back(points.point3.y);
      vertexArr.push_back(points.point3.z);
    }

    glGenVertexArrays(1, &DebugVAO);
    glGenBuffers(1, &DebugVBO);

    glBindVertexArray(DebugVAO);
    glBindBuffer(GL_ARRAY_BUFFER, DebugVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexArr.size() * sizeof(float),
                 vertexArr.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0); // Position
    glEnableVertexAttribArray(0);

    

    debugShader.use();
    glUniformMatrix4fv(glGetUniformLocation(debugShader.ID, "projection"), 1,
                       GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(debugShader.ID, "view"), 1,
                       GL_FALSE, &view[0][0]);
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::rotate(
        model, glm::radians(0.0f),
        glm::vec3(body->getTransform().getOrientation().getVectorV().x,
                  0.0000000000001f +
                      body->getTransform().getOrientation().getVectorV().y,
                  body->getTransform().getOrientation().getVectorV().z));
    glUniformMatrix4fv(glGetUniformLocation(debugShader.ID, "model"), 1,
                       GL_FALSE, &model[0][0]);
    glBindVertexArray(DebugVAO);
    glDrawArrays(GL_LINES, 0, vertexArr.size());

    // Lights
    lightShader.use();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1,
                       GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1,
                       GL_FALSE, &view[0][0]);
    model = glm::mat4(1.0f);
    model = glm::translate(
        model, lightPos);
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    model = glm::rotate(
        model, glm::radians(0.0f),
        glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1,
                       GL_FALSE, &model[0][0]);
    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));

    // glDrawElements(GL_TRIANGLES, 80, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(win->window);
    glfwPollEvents();
  }
  world->destroyRigidBody(body);
  physicsCommon.destroyPhysicsWorld(world);
  win->terminateWindow();
  return 0;
}


void processInput(GLFWwindow *window)
{
  float moveSpeed = 5.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    if (world->testOverlap(player.body , groundBody))
    {
      std::cout << "AABB Overlap detected!" << std::endl;
      player.body->applyLocalForceAtCenterOfMass(Vector3(0,800,0));
    }
    
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    // cameraPos += cameraFront * moveSpeed;
    player.body->applyLocalForceAtCenterOfMass(Vector3(cameraFront.x*10.0,0,cameraFront.z*-10));
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    // cameraPos -= cameraFront * moveSpeed;
    player.body->applyLocalForceAtCenterOfMass(Vector3(cameraFront.x*-10,0,cameraFront.z*10));
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    // cameraPos -= glm::normalize(glm::cross(cameraUp, cameraFront)) * moveSpeed;
    player.body->applyLocalForceAtCenterOfMass(Vector3(glm::normalize(glm::cross(cameraUp, cameraFront)).x*-10,0,glm::normalize(glm::cross(cameraUp, cameraFront)).z*10));

  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    // cameraPos += glm::normalize(glm::cross(cameraUp, cameraFront)) * moveSpeed;
    player.body->applyLocalForceAtCenterOfMass(Vector3(glm::normalize(glm::cross(cameraUp, cameraFront)).x*10,0,glm::normalize(glm::cross(cameraUp, cameraFront)).z*-10));
  }
}

float lastX = 400, lastY = 300;
float firstMouse = true;

float yaw = -90.0f;
float pitch = 0.0f;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 direction;
  direction.x = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = -sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(direction);
}

unsigned int loadTexture(char const *path)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}
