#pragma once
#include"Model.h"

class ModelManager{
public:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager& input) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

public:
	//ファイルの名前と一致しているならば
	static std::unique_ptr<Model> LoadModelFile(const std::string& filename);
	//ファイルの名前が違うのであれば
	static std::unique_ptr<Model> LoadModelFile(const std::string& filename, const std::string& modelName);

private:
	std::pair<std::vector<std::string>, std::vector<std::string>> modelNameContainer_;

	std::vector<std::unique_ptr<Model>> modelContainer_;

};

