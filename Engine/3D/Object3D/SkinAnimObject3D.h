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


class SkinAnimObject3D{
public:
	struct CameraForGPU{
		Vector3 worldPosition;
	};

	void Initialize(const std::string fileName);

	void Initialize(const std::string fileName, const std::string& modelName);
	
	void Update(const Matrix4x4& worldMatrix,const ViewProjection& viewProjection);

	void SkeletonUpdate(Model::Skeleton& skeleton);

	void SkeletonUpdate(Model::SkinCluster& skinCluster, Model::Skeleton& skeleton);

	void Draw();

	void DrawImgui(std::string name);
	
	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	void SetDirectionalLight(const Model::DirectionalLight* light);

	void SetPointLight(const Model::PointLight* pLight);

	void SetAnimation(const std::string fileName, const std::string& modelName) {
		Model::Animation animation;

		animation = Model::LoadAnimationFile(fileName, modelName);

		animations_.push_back({ modelName, animation });

		animationName.push_back(modelName);
	}

	void ChangeAnimation(const std::string& modelName) {
		for (size_t i = 0; i < animations_.size(); i++){
			if (animations_[i].first == modelName) {
				animation_ = animations_[i].second;
				break;
			}
		}

		
	}
	const std::vector<std::string>& GetAnimations() {
		return animationName;
	}


	const bool GetIsDraw()const { return isDraw_; }

	const std::vector<Model::Joint>& GetJoint()const { return skeleton_.joints; }

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	void makeResource();

private:
	Vector3 CalculateValue(const std::vector<Model::KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<Model::KeyframeQuaternion>& keyframes, float time);

	void ApplyAnimation(Model::Skeleton& skeleton, Model::Animation& animation, float animationTime);
public:
	Matrix4x4* parent_{};

private:
	//モデル
	std::unique_ptr<Model> model_;

	uint32_t animNum_ = 0;

	std::vector<std::pair<std::string, Model::Animation>>animations_;

	std::vector<std::string> animationName;

	Model::Animation animation_;

	Model::Skeleton skeleton_;

	Model::SkinCluster skinCluster_;

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
	Model::DirectionalLight* directionalLightDate = nullptr;

	const Model::DirectionalLight* directionalLight = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource;

	Model::PointLight* pointLightData = nullptr;

	const Model::PointLight* pointLight = nullptr;
	/*カメラ関連*/
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;

	CameraForGPU* cameraForGPU_ = nullptr;

	//データを書き込む
	//アニメーションの再生中の時刻
	float animationTime = 0.0f;


	Vector4 chackMatrix_ = {};



	Matrix4x4 worldMatrix_{};

	Matrix4x4 localMatrix_;

	EulerTransform transform;

	bool isSkinAnim_ = false;

	bool isUseLight_ = false;

	bool isDraw_ = true;

public:
	//反射強度
	float shininess_ = 1.0f;
};

