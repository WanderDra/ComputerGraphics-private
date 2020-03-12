#pragma once
#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <list>
#include <stdio.h>
#include <iostream>

using namespace std;

class Point {
public:
	Point(const int no, const glm::vec3 pos);
	int no;
	glm::vec3 pos;
	glm::vec4 color;
	glm::vec3 normal;
	glm::vec3 avg_normal;
	list<int> neighbors;
};

class Polygon {
public:
	glm::vec3 normal;
	Polygon(const int no, const list<int> points, const bool reverse);
	list<unsigned int> getIndices(bool reverse);
	list<int> getPoints();
	int getNo();

private:
	int no;
	int size;
	bool reverse;
	list<int> points;
	list<glm::vec3> indices;
};

class Model
{
public:
	Model(const bool reverse);
	Model(const list<Point> points, const list<Polygon> polygons, const bool reverse);
	void setColor(list<glm::vec4> color_map);
	void loadPoints(list<Point> points);
	void loadPolygons(const list<Polygon> polygons);
	float* getVertices(bool smooth);
	float* getNoneEBOVertices(bool smooth);
	void getIndeices(bool reverse);
	list<Polygon> getPolygon();
	int getVerticesAmount();
	int getPolygonsAmount();
	int getSizeOfVertices();
	list<unsigned int> indices;
	unsigned int indices_ui;

private:
	bool reverse;
	list<Point> points;
	list<Polygon> polygons;
	list<glm::vec4> color_map;
	list<Polygon> divided_polygons;
	int indices_length;
	int vertices_size;
	void smooth();														// Calculate ave_normal in points
	glm::vec3 calTriNormal(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3);
	void addNeighbor();
};



