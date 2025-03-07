#pragma once
#include"Matrix.h"
#include"Transform.h"
#include"Log.h"
#include<vector>
#include<fstream>
#include<sstream>
#include"TextureManager.h"
#include"ModelManager.h"
#include"PostEffect.h"
#include"ViewProjection.h"
#include"DirectionalLight.h"
#include"PointLight.h"
#include"Effect/TrailEffect.h"
/*3Dオブジェクトの描画*/

class Object3D{
public:
	struct CameraForGPU{
		Vector3 worldPosition;
	};
	//初期化処理
	void Initialize(const std::string& fileName);
	//更新処理
	void Update(const ViewProjection& viewProjection);
	//オブジェの場所が必要な時などの特殊な時に掛ける更新処理
	void UniqueUpdate();
	//描画処理
	void Draw();
	//imgui描画処理
	void DrawImgui(std::string name);
	/*Setter*/
	void SetIsDraw(const bool isDraw) { isDraw_ = isDraw; }

	void SetIsLighting(const bool isLighting) { isUseLight_ = isLighting; }

	void SetIsGetTop(const bool isGetTop) { isGetTop_ = isGetTop; }

	void SetShininess(const float data) { shininess_ = data; }

	void SetDirectionalLight(const DirectionalLight::DirectionalLightData* light);

	void SetPointLight(const PointLight::PointLightData* pLight);

	void SetTexture(const std::string& fileName) {
		texHandle_ = TextureManager::GetInstance()->Load(fileName);
	}

	void SetDissolve(const float data) {
		dissolveData_->threshold = data;
	}

	void SetColor(const Vector4 data) {
		materialDate_->color = data;
	}

	void SetTrailPos(const Vector2 trailData) {
		trailData_ = trailData;
	}

	void SetMatrix(const Matrix4x4& matrix) {
		setMatrix_ = matrix;
	}

	void SetTimeScale(const float scale) {
		timeScale_ = scale;
	}

	void SetAnimation(const Model::Animation animation) {
		animation_ = animation;
	}
	/*Getter*/
	const bool& GetIsDraw()const { return isDraw_; }

	const EulerTransform& GetTransform()const { return transform_; }

	/// <summary>
	/// 最も上の頂点の座標を取得する。またそれに付随してその頂点より下側に位置する座標も取得する
	/// </summary>
	/// <returns></returns>
	const TrailEffect::PosBuffer& GetTopAndTailVerTex() const {
		return vectorTop_;
	}

	const Matrix4x4& GetTopVerTexMat() const {
		return matTop_;
	}

	const Matrix4x4& GetTailVerTexMat() const {
		return matTail_;
	}
	//バッファリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	//色味の初期化
	void ColorReset() {
		materialDate_->color= { 1.0f, 1.0f, 1.0f, 1.0f };
	}
	//Resource生成
	void makeResource();

private:
	//キーフレームからの計算処理
	Vector3 CalculateValue(const std::vector<Model::KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<Model::KeyframeQuaternion>& keyframes, float time);
public:
	Matrix4x4* parent_{};

	EulerTransform transform_;

private:
	//モデル
	std::shared_ptr<Model> model_;

	Model::Animation animation_;

	uint32_t texHandle_;

	HRESULT hr_{};


	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルにデータを書き込む
	Model::Material* materialDate_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;

	//データを書き込む
	TransformationMatrix* wvpData_ = nullptr;

	/*ライト関連*/
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;

	//マテリアルにデータを書き込む
	DirectionalLight::DirectionalLightData* directionalLightDate_ = nullptr;

	const DirectionalLight::DirectionalLightData* directionalLight_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;

	PointLight::PointLightData* pointLightData_ = nullptr;

	const PointLight::PointLightData* pointLight_ = nullptr;
	/*カメラ関連*/
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;

	CameraForGPU* cameraForGPU_ = nullptr;

	//ディゾルブ関係
	ComPtr<ID3D12Resource> dissolveResource_;

	PostEffect::Threshold* dissolveData_ = nullptr;

	//データを書き込む
	//アニメーションの再生中の時刻
	float animationTime_ = 0.0f;

	float timeScale_ = 1.0f;

	Vector2 trailData_;

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

