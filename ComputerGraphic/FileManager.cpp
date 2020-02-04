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
	int edgeNum, ver[30];
	float* triangles = new float[triangleNum * 3 * 10];

	for (int i = 0; i < verNum; i++) {
		infile >> pos_x >> pos_y >> pos_z;
		vertices[i * 3] = pos_x;
		vertices[i * 3 + 1] = pos_y;
		vertices[i * 3 + 2] = pos_z;
	}

	int finalTriNum = 0;
	for (int i = 0; i < triangleNum; i++) {
		infile >> edgeNum;
		for (int k = 0; k < edgeNum; k++) {
			infile >> ver[k];
		}
		for (int e = 0; e < edgeNum - 2; e++) {
			for (int j = 0; j < 3; j++) {
				triangles[finalTriNum * 9 + j * 3] = vertices[(ver[j + e] - 1) * 3];
				triangles[finalTriNum * 9 + j * 3 + 1] = vertices[(ver[j + e] - 1) * 3 + 1];
				triangles[finalTriNum * 9 + j * 3 + 2] = vertices[(ver[j + e] - 1) * 3 + 2];
			}
			finalTriNum++;
		}
	}

	
	//cout << text << " " << verNum << " " << finalTriNum << endl;
	//for (int i = 0; i <= finalTriNum * 3; i++) {
	//	cout << i << " " << triangles[i * 3] << " " << triangles[i * 3 + 1] << " " << triangles[i * 3 + 2] << endl;
	//}
	infile.close();


	size = finalTriNum;
	return triangles;
}

int FileManager::getTriangleNum() {
	return size;
}
