#include "FileManager.h"

float* FileManager::load(const char* path)
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
	int edgeNum, ver[3];
	float* triangles = new float[triangleNum * 3 * 3];

	for (int i = 0; i < verNum; i++) {
		infile >> pos_x >> pos_y >> pos_z;
		vertices[i * 3] = pos_x;
		vertices[i * 3 + 1] = pos_y;
		vertices[i * 3 + 2] = pos_z;
	}

	for (int i = 0; i < triangleNum; i++) {
		infile >> edgeNum >> ver[0] >> ver[1] >> ver[2];
		for (int j = 0; j < 3; j++) {
			triangles[i * 9 + j * 3] = vertices[(ver[j] - 1) * 3];
			triangles[i * 9 + j * 3 + 1] = vertices[(ver[j] - 1) * 3 + 1];
			triangles[i * 9 + j * 3 + 2] = vertices[(ver[j] - 1) * 3 + 2];
		}
	}

	
	cout << text << " " << verNum << " " << triangleNum << endl;
	//for (int i = 0; i < triangleNum; i++) {
	//	cout << i << " " << triangles[i * 3] << " " << triangles[i * 3 + 1] << " " << triangles[i * 3 + 2] << endl;
	//}
	infile.close();


	size = triangleNum;
	return triangles;
}

int FileManager::getTriangleNum() {
	return size;
}
