#pragma once
#include"Matrix.h"
#include"Transform.h"
#include"Log.h"
#include<vector>
#include<fstream>
#include<sstream>
#include"TextureManager.h"
#include"Model.h"
#include"ViewProjection.h"


class Object3D{
public:
	struct CameraForGPU{
		Vector3 worldPosition;
	};

	void Initialize(Model* model);
	
	void Update(const Matrix4x4& worldMatrix,const ViewProjection& viewProjection);

	void Draw(uint32_t textureNumber);

	void DrawImgui(std::string name);

	void SetPosition(const Vector3& position) { position_ = position; }

	void SetScale(const Vector3& scale) { scale_ = scale; }

	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	
	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	void SetDirectionalLight(const DirectionalLight* light);

	void SetPointLight(const PointLight* pLight);

	const Vector3& GetPosition()const { return position_; }

	const bool& GetIsDraw()const { return isDraw_; }

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	void makeResource();

public:
	Matrix4x4* parent_{};

private:
	//モデル
	Model* model_ = nullptr;

	HRESULT hr;


	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//マテリアルにデータを書き込む
	Material* materialDate = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;

	//データを書き込む
	TransformationMatrix* wvpData = nullptr;

	/*ライト関連*/
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;

	//マテリアルにデータを書き込む
	DirectionalLight* directionalLightDate = nullptr;

	const DirectionalLight* directionalLight = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource;

	PointLight* pointLightData = nullptr;

	const PointLight* pointLight = nullptr;
	/*カメラ関連*/
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;

	CameraForGPU* cameraForGPU_ = nullptr;

	//データを書き込む

	Vector3 position_ = { 0.0f,0.0f ,0.0f };

	Vector3 scale_ = { 1.0f,1.0f ,1.0f };

	Vector3 rotate_ = { 0.0f,0.0f ,0.0f };

	Vector4 chackMatrix_ = {};

	Matrix4x4 worldMatrix_{};

	Transform transform{};

	bool isUseLight_ = false;

	bool isDraw_ = true;

public:
	//反射強度
	float shininess_ = 1.0f;
};

