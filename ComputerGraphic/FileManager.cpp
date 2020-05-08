#include "FileManager.h"


Model FileManager::load(const char* path, const bool reverse, const bool smooth)
{
	ifstream infile;
	infile.open(path, ios::in);
	if (!infile.is_open())
	{
		cout << "FileManager: Open file failure" << endl;
		return 0;
	}

	string text;
	float verNum, triangleNum;
	float pos_x, pos_y, pos_z;
	infile >> text >> verNum >> triangleNum;
	cout << text << verNum << endl;
	float* vertices = new float[verNum * 3];
	int edgeNum, ver[30];
	float* triangles = new float[triangleNum * 3 * 10];
	list<Point> list_point;
	list<Polygon> list_polygon;
	
	// Load points
	for (int i = 0; i < verNum; i++) {
		glm::vec3 pos;
		infile >> pos.x >> pos.y >> pos.z;
		Point insert_p(i, pos);
		list_point.insert(list_point.end(), insert_p);
	}

	// Load polygons
	int finalTriNum = 0;
	for (int i = 0; i < triangleNum; i++) {
		infile >> edgeNum;
		list<int> points;
		for (int k = 0; k < edgeNum; k++) {
			int p;
			infile >> p;
			if (!reverse) {
				points.insert(points.end(), p);
			}
			else {
				points.insert(points.begin(), p);
			}
		}
		Polygon insert_p(i + 1, points, false);
		list_polygon.insert(list_polygon.end(), insert_p);
	}

	Model model(list_point, list_polygon, false, smooth);


	/////////////////////////////////////////////////////////////DEBUG///////
	//std::list<vertex>::iterator it;
	//for (it = points.begin(); it != points.end(); ++it) {
	//	cout << it->number << endl;
	//	std::list<int>::iterator it2;
	//	it2 = it->neigbors.begin();
	//	for (it2; it2 != it->neigbors.end(); ++it2) {
	//		cout << *it2 << " ";
	//	}
	//	cout << endl;
	//	cout << it->x << " " << it->y << " " << it->z << endl;
	//	cout << it->normal.x << " " << it->normal.y << " " << it->normal.z << endl;
	//}
	//////////////////////////////////////////////////////////////////////////

	infile.close();


	size = finalTriNum;
	return model;
}

int FileManager::getTriangleNum() {
	return size;
}

std::list<vertex> FileManager::getPoints()
{
	return points;
}

std::list<int> FileManager::getPointNumberList()
{
	return point_number_list;
}

glm::vec3 FileManager::calTriNormal(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3)
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



