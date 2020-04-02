#include "ModelManager.h"

void ModelManager::addModel(Model* model, glm::vec3 pos)
{
	ModelAttributes ma = { model, pos };
	model_attr.insert(model_attr.end(), ma);
}

vector<ModelAttributes>* ModelManager::getModelAttributes()
{
	return &model_attr;
}

int ModelManager::getAmount()
{
	return model_attr.size();
}
