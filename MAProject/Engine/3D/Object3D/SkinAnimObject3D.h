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
#include"PostEffect.h"
#include <DirectionalLight.h>
#include <PointLight.h>
/*スキニングアニメーションのついた3Dオブジェクトの描画*/

class SkinAnimObject3D{
public:
	struct CameraForGPU{
		Vector3 worldPosition;
	};
	//ファイルネームが一致しているときの初期化
	void Initialize(const std::string fileName, const bool isLoop);
	//ファイルネームが違うときの初期化
	void Initialize(const std::string fileName, const std::string& modelName, const bool isLoop);
	//更新処理
	void Update(const ViewProjection& viewProjection);
	//skeletonの更新
	void SkeletonUpdate(Model::Skeleton& skeleton);
	//クラスターの更新
	void SkeletonUpdate(Model::SkinCluster& skinCluster, Model::Skeleton& skeleton);
	//描画処理
	void Draw();
	//imgui描画処理
	void DrawImgui(std::string name);
	/*setter*/
	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	void SetIsLighting(const bool& isLighting) { isUseLight_ = isLighting; }

	void SetDirectionalLight(const DirectionalLight::DirectionalLightData* light);

	void SetPointLight(const PointLight::PointLightData* pLight);

	void SetDissolve(const float data) {
		dissolveData_->threshold = data;
	}
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
	//アニメーションの変更
	void ChangeAnimation(const std::string& modelName) {
		if (nowAnimName_ == modelName) {
			return;
		}

		for (size_t i = 0; i < animations_.size(); i++){
			if (animations_[i].first == modelName) {
				blendFactor_ = 0;
				nowAnimName_ = modelName;
				afterAnimation_ = animations_[i].second;
				RestartAnimation();
				break;
			}
		}	
	}

	
	/// <summary>
	/// 現在のアニメーションの名前
	/// </summary>
	std::string ChackAnimationName() {
		for (size_t i = 0; i < animations_.size(); i++) {			
			if (beforeAnimation_.duration == animations_[i].second.duration) {
				return animations_[i].first;
			}
		}

		return std::string();
	}
	/*Getter*/
	const std::vector<std::string>& GetAnimations() {
		return animationName_;
	}

	void SetMatrix(const Matrix4x4& matrix) {
		setMatrix_ = matrix;
	}

	void SetAnimSpeed(const float speed) {
		animSpeed_ = speed;
	}

	void SetChangeAnimSpeed(const float speed) {
		changeAnimSpeed_ = speed;
	}
	//アニメーションの停止
	void AnimationStop() {
		isAnimation_ = false;
	}
	//最初から再生する
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
	//アニメーションを再開する
	void AnimationStart() {
		isAnimation_ = true;
	}

	const bool GetIsDraw()const { return isDraw_; }

	const EulerTransform& GetTransform()const { return transform_; }

	const std::vector<Model::Joint>& GetJoint()const { return skeleton_.joints; }
	//バッファリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	//リソースの生成
	void makeResource();

private:
	/*キーフレームの計算処理*/
	Vector3 CalculateValue(const std::vector<Model::KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<Model::KeyframeQuaternion>& keyframes, float time);

	void ApplyAnimation(Model::Skeleton& skeleton, Model::Animation& beforeAnimation, Model::Animation& afterAnimation, float animationTime);
public:
	Matrix4x4* parent_{};

	EulerTransform transform_;

private:
	//モデル
	std::unique_ptr<Model> model_;

	uint32_t animNum_ = 0;

	std::vector<std::pair<std::string, Model::Animation>>animations_;

	std::vector<std::string> animationName_;

	std::vector<bool> animationLoopFlug_;

	std::string nowAnimName_;

	//変更前のアニメーション
	Model::Animation beforeAnimation_;
	//変更後のアニメーション
	Model::Animation afterAnimation_;

	//アニメーションの補完係数
	float blendFactor_;

	Model::Skeleton skeleton_;

	Model::SkinCluster skinCluster_;

	uint32_t texHandle_;

	HRESULT hr_;


	ComPtr<ID3D12Resource> materialResource_;
	//マテリアルにデータを書き込む
	Model::Material* materialDate_ = nullptr;

	ComPtr<ID3D12Resource> wvpResource_;

	//データを書き込む
	TransformationMatrix* wvpData_ = nullptr;

	/*ライト関連*/
	ComPtr<ID3D12Resource> directionalLightResource_;

	//マテリアルにデータを書き込む
	DirectionalLight::DirectionalLightData* directionalLightDate_ = nullptr;

	const DirectionalLight::DirectionalLightData* directionalLight_ = nullptr;

	ComPtr<ID3D12Resource> pointLightResource_;

	PointLight::PointLightData* pointLightData_ = nullptr;

	const PointLight::PointLightData* pointLight_ = nullptr;
	/*カメラ関連*/
	ComPtr<ID3D12Resource> cameraResource_;

	CameraForGPU* cameraForGPU_ = nullptr;

	//ディゾルブ関係
	ComPtr<ID3D12Resource> dissolveResource_;

	PostEffect::Threshold* dissolveData_ = nullptr;

	//データを書き込む
	//アニメーションの再生中の時刻
	float animationTime_ = 0.0f;

	float animSpeed_ = 1.0f;

	float changeAnimSpeed_ = 1.0f;

	bool isAnimation_ = true;

	Vector4 chackMatrix_ = {};

	Matrix4x4 worldMatrix_{};

	Matrix4x4 localMatrix_;

	Matrix4x4 setMatrix_{};

	bool isSkinAnim_ = false;

	bool isUseLight_ = false;

	bool isDraw_ = true;

public:
	//反射強度
	float shininess_ = 1.0f;
};

