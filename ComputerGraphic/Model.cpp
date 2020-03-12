#include "Model.h"

Model::Model(const bool reverse)
{
	this->reverse = reverse;
}

Model::Model(const list<Point> points, const list<Polygon> polygons, const bool reverse)
{
	loadPoints(points);
	loadPolygons(polygons);
	this->reverse = reverse;
}

void Model::setColor(list<glm::vec4> color_map)
{
	list<Point>::iterator it_p;
	list<glm::vec4>::iterator it_c;
	it_c = color_map.begin();
	for (it_p = points.begin(); it_p != points.end(); ++it_p) {
		if (it_c == color_map.end()) {
			cout << "Color map size is mismatching." << endl;
			break;
		}
		it_p->color = *it_c;
		++it_c;
	}
}

void Model::loadPoints(const list<Point> points)
{
	this->points = points;
}

void Model::loadPolygons(list<Polygon> polygons)
{
	// Calculate normal
	list<Polygon>::iterator it_p;
	for (it_p = polygons.begin(); it_p != polygons.end(); ++it_p) {
		list<int>::iterator it_pl;
		list<Point>::iterator it_point;
		glm::vec3 vec[3];
		if (!reverse) {
			list<int> points_temp = it_p->getPoints();
			it_pl = points_temp.begin();
			for (int i = 0; i < 3; i++) {
				it_point = points.begin();
				advance(it_point, *it_pl - 1);
				vec[i] = it_point->pos;
				++it_pl;
			}
		}
		else {
			list<int> points_temp = it_p->getPoints();
			it_pl = points_temp.end();
			for (int i = 0; i < 3; i++) {
				it_point = points.begin();
				advance(it_point, *it_pl - 1);
				vec[i] = it_point->pos;
				--it_pl;
			}
		}
		it_p->normal = calTriNormal(vec[0], vec[1], vec[2]);
		//list<int> points_temp = it_p->getPoints();
		//it_pl = points_temp.begin();
		//for (it_pl; it_pl != points_temp.end(); ++it_pl) {
		//	it_point = points.begin();
		//	advance(it_point, *it_pl - 1);
		//	it_point->normal = it_p->normal;
		//}
	}
	this->polygons = polygons;
	addNeighbor();
	getIndeices(reverse);
}

void Model::smooth()
{
	list<Point>::iterator it_p;
	for (it_p = points.begin(); it_p != points.end(); ++it_p) {
		glm::vec3 sum_normal = glm::vec3(0.0f, 0.0f, 0.0f);
		bool empty = true;
		int neighbor_amount = 0;
		list<int>::iterator it_n;
		for (it_n = it_p->neighbors.begin(); it_n != it_p->neighbors.end(); ++it_n) {
			if (*it_n - 1 > polygons.size()) {
				cout << "The neighbor polygon does not exist." << endl;
				break;
			}
			list<Polygon>::iterator it_p2 = polygons.begin();
			advance(it_p2, *it_n - 1);
			if (empty) {
				sum_normal = it_p2->normal;
				empty = false;
			}
			else {
				sum_normal += it_p2->normal;
			}
		}
		it_p->avg_normal = sum_normal;
	}
}

float* Model::getVertices(bool smooth)
{
	list<float> ver_list; //pos, color, normal
	list<float>::iterator it_vl;
	list<Point>::iterator it_p;
	if (smooth) {
		this->smooth();
	}
	for (it_p = points.begin(); it_p != points.end(); ++it_p) {
		// Pos
		ver_list.insert(ver_list.end(), it_p->pos.x);
		ver_list.insert(ver_list.end(), it_p->pos.y);
		ver_list.insert(ver_list.end(), it_p->pos.z);
		// Color
		ver_list.insert(ver_list.end(), it_p->color.r);
		ver_list.insert(ver_list.end(), it_p->color.g);
		ver_list.insert(ver_list.end(), it_p->color.b);
		// Normal
		if (smooth) {
			ver_list.insert(ver_list.end(), it_p->avg_normal.x);
			ver_list.insert(ver_list.end(), it_p->avg_normal.y);
			ver_list.insert(ver_list.end(), it_p->avg_normal.z);
		}
		else {
			ver_list.insert(ver_list.end(), it_p->normal.x);
			ver_list.insert(ver_list.end(), it_p->normal.y);
			ver_list.insert(ver_list.end(), it_p->normal.z);
		}
	}
	it_vl = ver_list.begin();
	float* vertices = new float[ver_list.size()];
	for (int i = 0; i < ver_list.size(); i++) {
		vertices[i] = *it_vl;
		++it_vl;
	}
	vertices_size = ver_list.size();
	ver_list.clear();
	return vertices;
}

float* Model::getNoneEBOVertices(bool smooth)
{
	list<float> ver_list; //pos, color, normal
	list<float>::iterator it_vl;
	list<Point>::iterator it_p;
	list<int>::iterator it_point;
	list<Polygon>::iterator it_div_pl;
	if (smooth) {
		this->smooth();
	}
	for (it_div_pl = divided_polygons.begin(); it_div_pl != divided_polygons.end(); ++it_div_pl) {
		list<int> nodes = it_div_pl->getPoints();
		for (it_point = nodes.begin(); it_point != nodes.end(); ++it_point){
			it_p = points.begin();
			advance(it_p, *it_point - 1);
			// Pos
			ver_list.insert(ver_list.end(), it_p->pos.x);
			ver_list.insert(ver_list.end(), it_p->pos.y);
			ver_list.insert(ver_list.end(), it_p->pos.z);
			// Color
			ver_list.insert(ver_list.end(), it_p->color.r);
			ver_list.insert(ver_list.end(), it_p->color.g);
			ver_list.insert(ver_list.end(), it_p->color.b);
			// Normal
			if (smooth) {
				ver_list.insert(ver_list.end(), it_p->avg_normal.x);
				ver_list.insert(ver_list.end(), it_p->avg_normal.y);
				ver_list.insert(ver_list.end(), it_p->avg_normal.z);
			}
			else {
				ver_list.insert(ver_list.end(), it_div_pl->normal.x);
				ver_list.insert(ver_list.end(), it_div_pl->normal.y);
				ver_list.insert(ver_list.end(), it_div_pl->normal.z);
			}
		}
	}
	it_vl = ver_list.begin();
	float* vertices = new float[ver_list.size()];
	for (int i = 0; i < ver_list.size(); i++) {
		vertices[i] = *it_vl;
		++it_vl;
	}
	vertices_size = ver_list.size();
	ver_list.clear();
	return vertices;
}

void Model::getIndeices(bool reverse)
{
	list<Polygon>::iterator it_p;
	list<unsigned int>::iterator it_i;
	int no = 0;
	for (it_p = polygons.begin(); it_p != polygons.end(); ++it_p) {
		list<unsigned int> index = it_p->getIndices(reverse);
		it_i = index.begin();
		list<int> points;
		int count = 0;
		for (it_i; it_i != index.end(); ++it_i) {
			indices.insert(indices.end(), *it_i);
			if (count != 2) {
				points.insert(points.end(), *it_i);
				count++;
			}
			else {
				count = 0;
				points.insert(points.end(), *it_i);
				Polygon new_polygon(no, points, reverse);
				no++;
				new_polygon.normal = it_p->normal;
				divided_polygons.insert(divided_polygons.end(), new_polygon);
				points.clear();	
			}
			
		}
		
		
	}
	unsigned int* result = new unsigned int[indices.size()];
	it_i = indices.begin();
	for (int i = 0; i < indices.size(); i++) {
		result[i] = *it_i - 1;
		++it_i;
	}
	indices_ui = *result;
}

list<Polygon> Model::getPolygon()
{
	return this->polygons;
}

int Model::getVerticesAmount()
{
	return this->points.size();
}

int Model::getPolygonsAmount()
{
	return this->polygons.size();
}

int Model::getSizeOfVertices()
{
	return vertices_size;
}

glm::vec3 Model::calTriNormal(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3)
{
	float temp1[3], temp2[3], normal[3];
	float length = 0.0f;
	temp1[0] = ver2[0] - ver1[0];
	temp1[1] = ver2[1] - ver1[1];
	temp1[2] = ver2[2] - ver1[2];
	temp2[0] = ver3[0] - ver2[0];
	temp2[1] = ver3[1] - ver2[1];
	temp2[2] = ver3[2] - ver2[2];
	// Calculate normal
	normal[0] = temp1[1] * temp2[2] - temp1[2] * temp2[1];
	normal[1] = -(temp1[0] * temp2[2] - temp1[2] * temp2[0]);
	normal[2] = temp1[0] * temp2[1] - temp1[1] * temp2[0];
	// Unit
	length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	if (length == 0.0f) { length = 1.0f; }
	normal[0] /= length;
	normal[1] /= length;
	normal[2] /= length;
	glm::vec3 e_normal(normal[0], normal[1], normal[2]);
	//cout << e_normal[0] << " " << e_normal[1] << " " << e_normal[2] << endl;
	return e_normal;
}

void Model::addNeighbor()
{
	list<Polygon>::iterator it_p;
	list<int>::iterator it_pl;
	list<Point>::iterator it_point;
	for (it_p = polygons.begin(); it_p != polygons.end(); ++it_p) {
		list<int> point_temp = it_p->getPoints();
		it_pl = point_temp.begin();
		for (it_pl; it_pl != point_temp.end(); ++it_pl) {
			it_point = points.begin();
			advance(it_point, *it_pl - 1);
			it_point->neighbors.insert(it_point->neighbors.end(), it_p->getNo());
		}
	}
}

Polygon::Polygon(const int no, const list<int> points, const bool reverse)
{
	this->no = no;
	this->points = points;
	this->reverse = reverse;
	size = points.size();
}


list<unsigned int> Polygon::getIndices(bool reverse)
{
	list<unsigned int> indices;
	list<int>::iterator it_v;
	list<int>::iterator it_p;
	list<int>::iterator it_next;
	it_p = points.begin();
	int head_point = *it_p;
	++it_p;
	it_next = it_p;
	++it_next;
	for (it_p; it_next != points.end();) {
		indices.insert(indices.end(), head_point);
		indices.insert(indices.end(), *it_p);
		++it_p;
		++it_next;
		indices.insert(indices.end(), *it_p);
	}
	if (reverse) {
		indices.reverse();
	}
	return indices;
}

list<int> Polygon::getPoints()
{
	return this->points;
}

int Polygon::getNo()
{
	return this->no;
}

Point::Point(const int no, const glm::vec3 pos)
{
	this->no = no;
	this->pos = pos;
}