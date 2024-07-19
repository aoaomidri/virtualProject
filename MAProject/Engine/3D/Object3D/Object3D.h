#pragma once
#include"Matrix.h"
#include"Transform.h"
#include"Log.h"
#include<vector>
#include<fstream>
#include<sstream>
#include"TextureManager.h"
#include"Model.h"
#include"PostEffect.h"
#include"ViewProjection.h"
#include"DirectionalLight.h"
#include"PointLight.h"
#include"Effect/TrailEffect.h"

class Object3D{
public:
	struct CameraForGPU{
		Vector3 worldPosition;
	};

	void Initialize(const std::string fileName);
	
	void Update(const ViewProjection& viewProjection);

	void Draw();

	void DrawImgui(std::string name);
	
	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	void SetIsLighting(const bool& isLighting) { isUseLight_ = isLighting; }

	void SetIsGetTop(const bool& isGetTop) { isGetTop_ = isGetTop; }

	void SetShininess(const float data) { shininess_ = data; }

	void SetDirectionalLight(const DirectionalLight::DirectionalLightData* light);

	void SetPointLight(const PointLight::PointLightData* pLight);

	void SetDissolve(const float data) {
		dissolveData_->threshold = data;
	}

	void SetMatrix(const Matrix4x4& matrix) {
		setMatrix_ = matrix;
	}

	void SetAnimation(const Model::Animation animation) {
		animation_ = animation;
	}

	const bool& GetIsDraw()const { return isDraw_; }

	const EulerTransform& GetTransform()const { return transform_; }

	const TrailEffect::PosBuffer& GetTopVerTex() const{		
		return vectorTop_;
	}

	const Matrix4x4& GetTopVerTexMat() const {
		return matTop_;
	}

	const Matrix4x4& GetTailVerTexMat() const {
		return matTail_;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	void makeResource();

private:
	Vector3 CalculateValue(const std::vector<Model::KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<Model::KeyframeQuaternion>& keyframes, float time);
public:
	Matrix4x4* parent_{};

	EulerTransform transform_;

private:
	//モデル
	std::unique_ptr<Model> model_;

	Model::Animation animation_;

	uint32_t texHandle_;

	HRESULT hr;


	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//マテリアルにデータを書き込む
	Model::Material* materialDate = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;

	//データを書き込む
	TransformationMatrix* wvpData = nullptr;

	/*ライト関連*/
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;

	//マテリアルにデータを書き込む
	DirectionalLight::DirectionalLightData* directionalLightDate = nullptr;

	const DirectionalLight::DirectionalLightData* directionalLight = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource;

	PointLight::PointLightData* pointLightData = nullptr;

	const PointLight::PointLightData* pointLight = nullptr;
	/*カメラ関連*/
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;

	CameraForGPU* cameraForGPU_ = nullptr;

	//ディゾルブ関係
	ComPtr<ID3D12Resource> dissolveResource_;

	PostEffect::Threshold* dissolveData_ = nullptr;

	//データを書き込む
	//アニメーションの再生中の時刻
	float animationTime = 0.0f;

	Vector3 animeTranslate_;

	Quaternion animeRotate_;

	Vector3 animeScale_;

	Vector4 chackMatrix_ = {};

	Matrix4x4 worldMatrix_{};

	Matrix4x4 localMatrix_{};

	Matrix4x4 setMatrix_{};

	bool isUseLight_ = false;

	bool isDraw_ = true;

	bool isGetTop_ = false;

public:
	//反射強度
	float shininess_ = 1.0f;

	TrailEffect::PosBuffer vectorTop_{};

	Matrix4x4 matTop_{};

	Matrix4x4 matTail_{};
};

