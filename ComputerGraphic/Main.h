#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include <time.h> 

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "FileManager.h"
#include "Utils.h"
#include <direct.h>
#include "Skybox.h"

#include <vector>
#include "Camera.h"
#include "ModelManager.h"


class Main
{
private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
};

