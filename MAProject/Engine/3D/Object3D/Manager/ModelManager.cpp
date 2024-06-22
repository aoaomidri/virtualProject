#include "ModelManager.h"

std::unique_ptr<Model> ModelManager::LoadModelFile(const std::string& filename){
	//1,中で必要になる変数の宣言
	std::unique_ptr<Model> modelData = std::make_unique<Model>();//構築するModelData

	modelData->LoadFromOBJInternalAssimp(filename, filename);

	modelData->MakeVertexResource();

	//4,ModelDataを返す
	return modelData;
}

std::unique_ptr<Model> ModelManager::LoadModelFile(const std::string& filename, const std::string& modelName){
	//1,中で必要になる変数の宣言
	std::unique_ptr<Model> modelData = std::make_unique<Model>();//構築するModelData

	modelData->LoadFromOBJInternalAssimp(filename, modelName);

	modelData->MakeVertexResource();

	//4,ModelDataを返す
	return modelData;
}
