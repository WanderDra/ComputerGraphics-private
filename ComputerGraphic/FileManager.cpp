#include "FileManager.h"



Model FileManager::load(const char* path, const bool reverse)
{
	ifstream infile;
	infile.open(path, ios::in);
	if (!infile.is_open())
	{
		cout << "FileManager: Open file failure" << endl;
	}

	string text;
	float verNum, triangleNum;
	float pos_x, pos_y, pos_z;
	infile >> text >> verNum >> triangleNum;
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

	/*for (int i = 0; i < verNum; i++) {
		infile >> pos_x >> pos_y >> pos_z;
		vertices[i * 3] = pos_x;
		vertices[i * 3 + 1] = pos_y;
		vertices[i * 3 + 2] = pos_z;
	}*/

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

	Model model(list_point, list_polygon, false);

	//int finalTriNum = 0;
	//for (int i = 0; i < triangleNum; i++) {
	//	infile >> edgeNum;
	//	for (int k = 0; k < edgeNum; k++) {
	//		infile >> ver[k];
	//	}
	//	
	//	// Get points position and relationship for normal calculation
	//	for (int k = 0; k < edgeNum; k++){
	//		std::list<vertex>::iterator it;
	//		for (it = points.begin(); it != points.end(); ++it) {
	//			if (it->number == ver[k]) {
	//				break;
	//			}
	//		}
	//		if (it != points.end()) {
	//			for (int k = 0; k < edgeNum; k++) {
	//				if (ver[k] != it->number) {
	//					bool exist = false;
	//					std::list<int>::iterator it_neigbors;
	//					for (it_neigbors = it->neigbors.begin(); it_neigbors != it->neigbors.end(); ++it_neigbors) {
	//						if (ver[k] == *it_neigbors) {
	//							exist = true;
	//							break;
	//						}
	//					}
	//					if (!exist) {
	//						it->neigbors.insert(it->neigbors.end(), ver[k]);
	//					}
	//				}
	//			}
	//		}
	//		else {
	//			vertex new_point;
	//			new_point.x = vertices[ver[k] * 3];
	//			new_point.y = vertices[ver[k] * 3 + 1];
	//			new_point.z = vertices[ver[k] * 3 + 2];
	//			new_point.number = ver[k];
	//			new_point.normal = glm::vec3(0.0f, 0.0f, 0.0f);
	//			for (int k = 0; k < edgeNum; k++) {
	//				if (ver[k] != new_point.number) {
	//					bool exist = false;
	//					std::list<int>::iterator it_neigbors;
	//					for (it_neigbors = new_point.neigbors.begin(); it_neigbors != new_point.neigbors.end(); ++it_neigbors) {
	//						if (ver[k] == *it_neigbors) {
	//							exist = true;
	//							break;
	//						}
	//					}
	//					if (!exist) {
	//						new_point.neigbors.insert(new_point.neigbors.end(), ver[k]);
	//					}
	//				}
	//			}
	//			//new_point.neigbors.insert(new_point.neigbors.end(), ver[k]);
	//			//cout << ver[k] << endl;
	//			points.insert(points.end(), new_point);
	//		}
	//	}

		// Calculate normal
		//std::list<vertex>::iterator it = points.begin();
		//glm::vec3 ver0;
		//glm::vec3 ver1;
		//glm::vec3 ver2;
		//glm::vec3 normal;
		//int counter = 0;
		//for (it; it != points.end(); ++it) {
		//	if (it->number == ver[0]) {
		//		ver0 = glm::vec3(it->x, it->y, it->z);
		//		counter++;
		//	}
		//	if (it->number == ver[1]) {
		//		ver1 = glm::vec3(it->x, it->y, it->z);
		//		counter++;
		//	}
		//	if (it->number == ver[2]) {
		//		ver2 = glm::vec3(it->x, it->y, it->z);
		//		counter++;
		//	}
		//	if (counter == 3) {
		//		break;
		//	}
		//}
		//if (reverse) {
		//	normal = calTriNormal(ver0, ver1, ver2);
		//	//glm::vec3 vec1 = ver1 - ver0;
		//	//glm::vec3 vec2 = ver2 - ver1;
		//	//if (vec1 * vec2 < 0) {

		//	//}
		//}
		//else {
		//	normal = calTriNormal(ver2, ver1, ver0);
		//}
		//for (int i = 0; i < edgeNum; i++) {
		//	for (it = points.begin(); it != points.end(); ++it) {
		//		if (it->number == ver[i]) {
		//			it->normal += normal;
		//			//cout << it->number << endl;
		//			//cout << it->normal.x << " " << it->normal.y << " " << it->normal.z << endl;
		//			break;
		//		}
		//	}
		//}

		

	//	if (reverse) {
	//		// divide polygon to triangle
	//		for (int e = 0; e < edgeNum - 2; e++) {
	//			triangles[finalTriNum * 9] = vertices[(ver[0] - 1) * 3];					//x0
	//			triangles[finalTriNum * 9 + 1] = vertices[(ver[0] - 1) * 3 + 1];					//y0
	//			triangles[finalTriNum * 9 + 2] = vertices[(ver[0] - 1) * 3 + 2];					//z0
	//			point_number_list.insert(point_number_list.end(), ver[0]);
	//			for (int j = 1; j < 3; j++) {
	//				triangles[finalTriNum * 9 + j * 3] = vertices[(ver[e + j] - 1) * 3];			//x1 x2
	//				triangles[finalTriNum * 9 + j * 3 + 1] = vertices[(ver[e + j] - 1) * 3 + 1];	//y1 y2
	//				triangles[finalTriNum * 9 + j * 3 + 2] = vertices[(ver[e + j] - 1) * 3 + 2];	//z1 z2
	//				point_number_list.insert(point_number_list.end(), ver[e + j]);
	//			}
	//			finalTriNum++;
	//		}
	//	}
	//	else {
	//		// divide polygon to triangle
	//		for (int e = 0; e < edgeNum - 2; e++) {
	//			triangles[finalTriNum * 9] = vertices[(ver[0] - 1) * 3];					//x0
	//			triangles[finalTriNum * 9 + 1] = vertices[(ver[0] - 1) * 3 + 1];					//y0
	//			triangles[finalTriNum * 9 + 2] = vertices[(ver[0] - 1) * 3 + 2];					//z0
	//			point_number_list.insert(point_number_list.end(), ver[0]);
	//			int count = 2;
	//			for (int j = 1; j < 3; j++) {
	//				triangles[finalTriNum * 9 + j * 3] = vertices[(ver[e + count] - 1) * 3];			//x1 x2
	//				triangles[finalTriNum * 9 + j * 3 + 1] = vertices[(ver[e + count] - 1) * 3 + 1];	//y1 y2
	//				triangles[finalTriNum * 9 + j * 3 + 2] = vertices[(ver[e + count] - 1) * 3 + 2];	//z1 z2
	//				point_number_list.insert(point_number_list.end(), ver[e + count]);
	//				count--;
	//			}
	//			finalTriNum++;
	//		}
	//	}
	//}
	//std::list<vertex>::iterator it = points.begin();
	//cout << "===========================================================" << endl;
	//for (it = points.begin(); it != points.end(); ++it) {
	//	cout << it->number << endl;
	//	cout << it->normal.x << " " << it->normal.y << " " << it->normal.z << endl;
	//}

	
	//cout << text << " " << verNum << " " << finalTriNum << endl;
	//for (int i = 0; i <= finalTriNum * 3; i++) {
	//	cout << i << " " << triangles[i * 3] << " " << triangles[i * 3 + 1] << " " << triangles[i * 3 + 2] << endl;
	//}

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



