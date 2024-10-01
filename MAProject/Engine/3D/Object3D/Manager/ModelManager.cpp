#include "ModelManager.h"

ModelManager* ModelManager::GetInstance(){
	static ModelManager instance;
	return &instance;
}

std::shared_ptr<Model> ModelManager::LoadModelFile(const std::string& filename){
	//1,中で必要になる変数の宣言
	std::shared_ptr<Model> modelData = std::make_unique<Model>();//構築するModelData

	size_t size = 0;
	bool isLoad = false;

	for (size_t i = 0; i < modelNameContainer_.first.size(); i++) {
		if (modelNameContainer_.first[i] == filename and modelNameContainer_.second[i] == filename) {
			isLoad = true;
			size = i;
			break;
		}
	}

	// 既に読み込まれている場合
	if (isLoad) {
		// コンテナ内のモデルデータを返す（所有権の移動ではなく参照）
		return (modelContainer_[size]);
	}

	// 新しいモデルの読み込み処理
	modelData = std::make_unique<Model>();

	// モデル名とファイル名をコンテナに追加
	modelNameContainer_.first.push_back(filename);
	modelNameContainer_.second.push_back(filename);

	// モデルを読み込み
	modelData->LoadFromOBJInternalAssimp(filename, filename);

	// 頂点リソースの作成
	modelData->MakeVertexResource();

	// 読み込んだモデルデータをコンテナに移動
	modelContainer_.emplace_back(std::move(modelData));

	// モデルデータを返す
	return (modelContainer_.back());
}

std::shared_ptr<Model> ModelManager::LoadModelFile(const std::string& filename, const std::string& modelName){
	//1,中で必要になる変数の宣言
	std::shared_ptr<Model> modelData = std::make_unique<Model>();//構築するModelData

	size_t size = 0;
	bool isLoad = false;

	for (size_t i = 0; i < modelNameContainer_.first.size(); i++) {
		if (modelNameContainer_.first[i] == filename and modelNameContainer_.second[i] == modelName) {
			isLoad = true;
			size = i;
			break;
		}		
	}

	// 既に読み込まれている場合
	if (isLoad) {
		// コンテナ内のモデルデータを返す（所有権の移動ではなく参照）
		return (modelContainer_[size]);
	}

	// 新しいモデルの読み込み処理
	modelData = std::make_unique<Model>();

	// モデル名とファイル名をコンテナに追加
	modelNameContainer_.first.push_back(filename);
	modelNameContainer_.second.push_back(modelName);

	// モデルを読み込み
	modelData->LoadFromOBJInternalAssimp(filename, modelName);

	// 頂点リソースの作成
	modelData->MakeVertexResource();

	// 読み込んだモデルデータをコンテナに移動
	modelContainer_.emplace_back(std::move(modelData));

	// モデルデータを返す
	return  (modelContainer_.back());
}
