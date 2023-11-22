#pragma once
#include "math/Matrix.h"
#include <Windows.h>
#include <wrl.h>
#include"Transform.h"
#include"Log.h"
#include<vector>
#include<fstream>
#include<sstream>
#include"Model.h"
#include"3D/ViewProjection.h"


class Object3D{
public:


	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const std::string fileName);
	
	void Update(const Matrix4x4& worldMatrix,const ViewProjection& viewProjection);

	void Draw(D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle);

	void DrawImgui();

	void SetPosition(const Vector3& position) { position_ = position; }

	void SetScale(const Vector3& scale) { scale_ = scale; }

	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	
	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	const Vector3& GetPosition()const { return position_; }

	const bool& GetIsDraw()const { return isDraw_; }

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device, size_t sizeInBytes);

	void makeResource();
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	ModelData LoadObjFile(const std::string& filename);

public:
	Matrix4x4* parent_{};

private:
	const std::string ResourcesPath = "resources/";



	HRESULT hr;

	ID3D12Device* device_ = nullptr;

	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//頂点リソースにデータを書き込む
	VertexData* vertexDate = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//マテリアルにデータを書き込む
	Material* materialDate = nullptr;

	//モデル読み込み
	ModelData modelData;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;

	//データを書き込む
	TransformationMatrix* wvpData = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;

	//マテリアルにデータを書き込む
	DirectionalLight* directionalLightDate = nullptr;

	//データを書き込む

	Vector3 position_ = { 0.0f,0.0f ,0.0f };

	Vector3 scale_ = { 1.0f,1.0f ,1.0f };

	Vector3 rotate_ = { 0.0f,0.0f ,0.0f };

	Vector4 chackMatrix_ = {};

	Matrix4x4 worldMatrix_{};

	Transform transform{};

	Transform cameraTransform{};

	bool isDraw_ = true;
};

