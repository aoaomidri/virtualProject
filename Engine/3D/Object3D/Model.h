#pragma once
#include"Log.h"
#include"Transform.h"
#include<sstream>
#include<vector>
#include <Windows.h>
#include <wrl.h>
#include"assimp/Importer.hpp"
#include"assimp/scene.h"
#include"assimp/postprocess.h"

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
	float shininess;
};

struct DirectionalLight {
	Vector4 color;		//ライトの色
	Vector3 direction;	//ライトの向き
	float intensity;	//輝度
};

struct PointLight {
	Vector4 color;		//ライトの色
	Vector3 position;	//ライトの位置
	float intensity;	//輝度
	float radius;		//ライトの届く最大距離
	float decay;		//減衰率
	float padding[2];
};

struct SpotLight{
	Vector4 color;
	Vector3 position;
	float intensity;
};



class Model{
public:
	void Draw(ID3D12GraphicsCommandList* CommandList);

	static Model* GetInstance();

	static std::unique_ptr<Model> LoadObjFile(const std::string& filename);

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);



	static void SetDevice(ID3D12Device* device) { Model::device_ = device; }

	void Finalize();

public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView()const { return vertexBufferView_; }

	MaterialData GetMaterial()const { return material_; }

	const std::vector<VertexData> GetVertexData()const { return indices_; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device, size_t sizeInBytes);

	//OBJファイルから3Dモデルを読み込む(非公開)
	void LoadFromOBJInternal(const std::string& filename);

	void LoadFromOBJInternalAssimp(const std::string& filename);

	void MakeVertexResource();

private:
	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//頂点リソースにデータを書き込む
	VertexData* vertexDate_ = nullptr;

	const std::string ResourcesPath_ = "resources/Model/";

	//頂点インデックス配列
	std::vector<VertexData> indices_;

	static ID3D12Device* device_;

	MaterialData material_;
};

