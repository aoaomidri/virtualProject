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

	void Initialize(const std::string fileName, const bool isLoop);

	void Initialize(const std::string fileName, const std::string& modelName, const bool isLoop);
	
	void Update(const Matrix4x4& worldMatrix,const ViewProjection& viewProjection);

	void SkeletonUpdate(Model::Skeleton& skeleton);

	void SkeletonUpdate(Model::SkinCluster& skinCluster, Model::Skeleton& skeleton);

	void Draw();

	void DrawImgui(std::string name);
	
	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	void SetDirectionalLight(const Model::DirectionalLight* light);

	void SetPointLight(const Model::PointLight* pLight);


	/*アニメーション関連の関数*/

	void SetAnimation(const std::string fileName, const std::string& modelName, const bool isLoop) {
		Model::Animation animation;

		animation = Model::LoadAnimationFile(fileName, modelName);
		animation.isAnimLoop = isLoop;

		animations_.push_back({ modelName, animation });

		animationName_.push_back(modelName);
	}

	void SetAnimation(const std::string fileName, const bool isLoop) {
		Model::Animation animation;

		animation = Model::LoadAnimationFile(fileName, fileName);
		animation.isAnimLoop = isLoop;

		animations_.push_back({ fileName, animation });

		animationName_.push_back(fileName);
	}

	void ChangeAnimation(const std::string& modelName) {
		for (size_t i = 0; i < animations_.size(); i++){
			if (animations_[i].first == modelName) {
				if (animation_.duration == animations_[i].second.duration) {
					break;
				}
				animation_ = animations_[i].second;
				RestartAnimation();
				break;
			}
		}	
	}

	/// <summary>
	/// 現在のアニメーションと引数のアニメーションを比較
	/// </summary>
	/// <param name="name">比較したいアニメーション</param>
	bool ChackAnimation(const std::string& name) const{
		


		return false;
	}

	/// <summary>
	/// 現在のアニメーションの名前
	/// </summary>
	std::string ChackAnimationName() {
		for (size_t i = 0; i < animations_.size(); i++) {			
			if (animation_.duration == animations_[i].second.duration) {
				return animations_[i].first;
			}
		}

		return std::string();
	}

	const std::vector<std::string>& GetAnimations() {
		return animationName_;
	}

	void SetAnimSpeed(const float speed) {
		animSpeed_ = speed;
	}

	void AnimationStop() {
		isAnimation_ = false;
	}
	
	void RestartAnimation() {
		animationTime_ = 0;
	}

	/// <summary>
	/// 時間になったらアニメーションを止める
	/// </summary>
	/// <param name="time">アニメーションを止める時間(フレーム)</param>
	void AnimationTimeStop(const float time) {
		if (animationTime_ > time / 60.0f) {
			AnimationStop();
		}
	}

	void AnimationStart() {
		isAnimation_ = true;
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

	std::vector<std::string> animationName_;

	std::vector<bool> animationLoopFlug_;

	Model::Animation animation_;

	Model::Skeleton skeleton_;

	Model::SkinCluster skinCluster_;

	uint32_t texHandle_;

	HRESULT hr;


	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルにデータを書き込む
	Model::Material* materialDate_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;

	//データを書き込む
	TransformationMatrix* wvpData_ = nullptr;

	/*ライト関連*/
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;

	//マテリアルにデータを書き込む
	Model::DirectionalLight* directionalLightDate_ = nullptr;

	const Model::DirectionalLight* directionalLight_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;

	Model::PointLight* pointLightData_ = nullptr;

	const Model::PointLight* pointLight_ = nullptr;
	/*カメラ関連*/
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;

	CameraForGPU* cameraForGPU_ = nullptr;

	//データを書き込む
	//アニメーションの再生中の時刻
	float animationTime_ = 0.0f;

	float animSpeed_ = 1.0f;

	bool isAnimation_ = true;

	Vector4 chackMatrix_ = {};

	Matrix4x4 worldMatrix_{};

	Matrix4x4 localMatrix_;

	EulerTransform transform_;

	bool isSkinAnim_ = false;

	bool isUseLight_ = false;

	bool isDraw_ = true;

public:
	//反射強度
	float shininess_ = 1.0f;
};

