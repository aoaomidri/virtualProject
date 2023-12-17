#pragma once
#include "../../../math/Matrix.h"
#include"../Transform.h"
#include"../../Base/Log.h"
#include<vector>
#include<fstream>
#include<sstream>
#include"Model.h"
#include"../ViewProjection.h"


class Object3D{
public:
	struct CameraForGPU{
		Vector3 worldPosition;
	};

	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	
	void Update(const Matrix4x4& worldMatrix,const ViewProjection& viewProjection);

	void Draw(D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle);

	void DrawImgui();

	void SetPosition(const Vector3& position) { position_ = position; }

	void SetScale(const Vector3& scale) { scale_ = scale; }

	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	
	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	void SetModel(Model* model) { this->model_ = model; }

	const Vector3& GetPosition()const { return position_; }

	const bool& GetIsDraw()const { return isDraw_; }

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device, size_t sizeInBytes);

	void makeResource();

public:
	Matrix4x4* parent_{};

private:
	//モデル
	Model* model_ = nullptr;

	HRESULT hr;

	ID3D12Device* device_ = nullptr;

	ID3D12GraphicsCommandList* commandList_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//マテリアルにデータを書き込む
	Material* materialDate = nullptr;

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

	const CameraForGPU* cameraForGPU_ = nullptr;

	bool isDraw_ = true;
};

