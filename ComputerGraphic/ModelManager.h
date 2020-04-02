#pragma once
#include<vector>
#include<glm/glm/glm.hpp>
#include"Model.h"

using namespace std;

struct ModelAttributes {
	Model* model;
	glm::vec3 location;
};

class ModelManager
{
public:
	void addModel(Model* model, glm::vec3 pos);
	vector<ModelAttributes>* getModelAttributes();
	int getAmount();
private:
	vector<ModelAttributes> model_attr;
};

