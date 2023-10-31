#pragma once
#include"Log.h"
#include"Transform.h"
#include<sstream>
#include<vector>

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct DirectionalLight {
	Vector4 color;		//ライトの色
	Vector3 direction;	//ライトの向き
	float intensity;	//輝度
};

class Model{
public:
	static Model* GetInstance();

	ModelData LoadObjFile(const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
private:
	const std::string ResourcesPath = "resources/";
};

