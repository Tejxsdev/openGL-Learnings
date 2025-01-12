#pragma once
#include <glad/glad.h> // include glad to get the required OpenGL headers
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
using namespace std;

class shader
{
public:
	int ID;
	shader(const char *vertShaderSource, const char *fragShaderSource);
	void use();
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setMat4(const std::string& name, const GLfloat* value) const;
	void setFloat(const std::string& name, float value) const;
	void setInt(const std::string& name, int value) const;
private:

};