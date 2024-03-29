#pragma once
#include"../../Base/Log.h"
#include"../Transform.h"
#include<sstream>
#include<vector>
#include <Windows.h>
#include <wrl.h>


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
	void Draw(ID3D12GraphicsCommandList* CommandList);

	static Model* GetInstance();

	static Model* LoadObjFile(const std::string& filename);

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView()const { return vertexBufferView; }

	const std::vector<VertexData> GetVertexData()const { return indices; }

	static void SetDevice(ID3D12Device* device) { Model::device_ = device; }


private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device, size_t sizeInBytes);

	//OBJファイルから3Dモデルを読み込む(非公開)
	void LoadFromOBJInternal(const std::string& filename);

	void MakeVertexResource();

private:
	//デバイス
	static ID3D12Device* device_;

	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//頂点リソースにデータを書き込む
	VertexData* vertexDate = nullptr;

	const std::string ResourcesPath = "resources/";

	//頂点インデックス配列
	std::vector<VertexData> indices;

	MaterialData material;
};

