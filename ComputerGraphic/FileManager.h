#pragma once
#include<iostream>
#include<fstream>
#include<stdio.h>

using namespace std;

class FileManager
{
public:
	float* load(const char* path);
	int getTriangleNum();
private:
	int size;
};

