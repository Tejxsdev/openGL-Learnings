#include "../../src/engine/renderer/Window.h"
#include "glm/fwd.hpp"
#include "stb_image.h"
#include <iostream>
#include <vector>
#include "../../src/engine/core/Engine.h"

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1000;
unsigned int loadTexture(char const *path);
bool isPaused = false;
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
#include <filesystem>
#include <iostream>

int main()
{
  // Window creation
  Window *win = new Window(SCR_WIDTH, SCR_HEIGHT, "MyGame");

  Engine e;

  World &world = e.getWorld();
  e.run(win->window, win->screenWidth, win->screenHeight);

  glEnable(GL_DEPTH_TEST);

  win->terminateWindow();
  return 0;
}