#pragma once
#include<iostream>
#include<fstream>
#include<stdio.h>
#include<list>
#include<glm/glm/glm.hpp>
#include "Model.h"

using namespace std;

struct vertex {
	float x, y, z;
	int number;
	std::list<int> neigbors;
	glm::vec3 normal;
};

class FileManager
{
public:
	

	Model load(const char* path, const bool reverse, const bool smooth);
	int getTriangleNum();
	std::list<vertex> getPoints();
	std::list<int> getPointNumberList();
private:
	int size;
	std::list<vertex> points;
	glm::vec3 calTriNormal(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3);
	std::list<int> point_number_list;
};

