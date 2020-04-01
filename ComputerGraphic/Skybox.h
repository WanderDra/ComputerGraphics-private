#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "Shader.h"

using namespace std;

class Skybox
{
public:
	Skybox(std::vector<std::string> faces, Shader *shader);
	unsigned int loadCubemap();
	void show();
	void clear();
private:
	int obj_id;
	unsigned int textureID;
	vector<string> faces;
	Shader *shader;
	unsigned int skyboxVAO, skyboxVBO;
};

