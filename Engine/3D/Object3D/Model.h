#pragma once
#include"../../Base/Log.h"
#include"../Transform.h"
#include<sstream>
#include<vector>

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct DirectionalLight {
	Vector4 color;		//ライトの色
	Vector3 direction;	//ライトの向き
	float intensity;	//輝度
};

class Model{
public:
	static Model* GetInstance();

	static Model* LoadObjFile(const std::string& filename);

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

public:
	static void SetDevice(ID3D12Device* device) { Model::device_ = device; }

private:
	//デバイス
	static ID3D12Device* device_;

	//OBJファイルから3Dモデルを読み込む(非公開)
	void LoadFromOBJInternal(const std::string& filename);

	const std::string ResourcesPath = "resources/";

	//頂点インデックス配列
	std::vector<VertexData> indices;

	MaterialData material;
};

