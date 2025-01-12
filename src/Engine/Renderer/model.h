#pragma once
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "mesh.h"

using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
	vector<Texture> textures_loaded;
	vector <Mesh> meshes;
	vector<unsigned int> vertexCount;
	vector<unsigned int> triangleCount;
	string dir;
	bool gammaCorrection;

	Model(string const& path);
	void Draw(shader &shader);
private:
	void loadModel(string const& path);
	void processNode(aiNode* Node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};