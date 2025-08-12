#include "Shader.h"

Shader::Shader(const string vertShaderPath, const string fragShaderPath)
{
	vertShaderSourcePath = vertShaderPath;
	fragShaderSourcePath = fragShaderPath;

	int vertexShader, fragShader;

	std::ifstream vsfile;
	std::ifstream fsfile;

	string vertShaderCode;
	string fragShaderCode;

	try
	{
		vsfile.open(vertShaderPath);
		fsfile.open(fragShaderPath);
		stringstream vShaderStream, fShaderStream;
		vShaderStream << vsfile.rdbuf();
		fShaderStream << fsfile.rdbuf();
		vsfile.close();
		fsfile.close();
		vertShaderCode = vShaderStream.str();
		fragShaderCode = fShaderStream.str();

	}
	catch (ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertShaderCode.c_str();
	const char* fShaderCode = fragShaderCode.c_str();

	//vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);

	//frag shader
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fShaderCode, NULL);
	glCompileShader(fragShader);

	//shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragShader);
	glLinkProgram(ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, const GLfloat* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0]);
}

void Shader::setFloat(const std::string & name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}