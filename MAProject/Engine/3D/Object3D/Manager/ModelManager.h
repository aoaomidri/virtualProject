#pragma once
#include"Model.h"

class ModelManager{
public:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager& input) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

	static ModelManager* GetInstance();

public:
	//ファイルの名前と一致しているならば
	std::shared_ptr<Model> LoadModelFile(const std::string& filename);
	//ファイルの名前が違うのであれば
	std::shared_ptr<Model> LoadModelFile(const std::string& filename, const std::string& modelName);

private:
	std::pair<std::vector<std::string>, std::vector<std::string>> modelNameContainer_;

	std::vector<std::shared_ptr<Model>> modelContainer_;

};

