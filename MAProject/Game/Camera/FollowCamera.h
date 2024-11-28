#pragma once
#include"Transform.h"
#include"Input.h"
#include"Adjustment_Item.h"
#include"ViewProjection.h"
#include"ViewingFrustum.h"
#include"Matrix.h"
/*プレイヤーの後ろについて回るカメラ*/
//前方宣言
class LockOn;

struct CameraShake {
	float amplitude;  // シェイクの振幅
	float duration;   // シェイクの持続時間
	float frequency;  // シェイクの周波数
	float elapsedTime; // シェイクの経過時間

	// シェイク中かどうか判定
	bool IsActive() const {
		return elapsedTime < duration;
	}
};

class FollowCamera{
public:
	void Initialize();

	void Update();

	void Reset();

	void RotateReset();

	void CameraPosInit();

	Vector3 offsetCalculation(const Vector3& offset) const;
	/// <summary>
	/// カメラシェイク開始
	/// </summary>
	/// <param name="amplitude">振幅</param>
	/// <param name="duration">持続時間</param>
	/// <param name="frequency">周波数</param>
	void StartShake(float amplitude, float duration, float frequency = 10.0f) {
		cameraShake_.amplitude = amplitude;
		cameraShake_.duration = duration;
		cameraShake_.frequency = frequency;
		cameraShake_.elapsedTime = 0.0f;
	}
	

	void DrawImgui();
public:

	void SetTarget(const EulerTransform* target);

	void SetPosition(const Vector3& pos) { viewProjection_.translation_ = pos; }

	void SetTargetMatrix(const Matrix4x4* target) { targetRotateMatrix = target; }

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	void SetIsMove(const bool isMove) { isMove_ = isMove; };
public:
	ViewingFrustum& GetViewingFrustum() { return viewingFrustum_; }

	ViewingFrustum& GetLockViewingFrustum() { return lockViewingFrustum_; }

	const ViewProjection& GetViewProjection()const { return viewProjection_; }

	const Matrix4x4 GetProjectionInverse()const{
		Matrix4x4 mat = Matrix::Inverce(viewProjection_.matProjection_);
		return mat;
	}
private:
	/*プライベート関数*/
	void ApplyGlobalVariables();
	//揺れの更新
	void ShakeUpdate();

private:
	ViewProjection viewProjection_;

	Vector3 cameraMove_{};

	const Vector3 baseCameraOffset = { 0.0f,1.0f,-8.0f };

	Vector3 cameraOffset{};

	//視錐台
	ViewingFrustum viewingFrustum_ = {
		.translation_ = viewProjection_.translation_,
		.rotate_ = viewProjection_.rotation_,
		.direction = {0.0f,0.0f,1.0f},
		.verticalFOV = viewProjection_.fovAngleY_,
		.aspectRatio = viewProjection_.aspectRatio_,
		.nearZ = viewProjection_.nearZ_,
		.farZ = viewProjection_.farZ_
	};



	ViewingFrustum lockViewingFrustum_ = {
		.translation_ = viewProjection_.translation_,
		.rotate_ = viewProjection_.rotation_,
		.direction = {0.0f,0.0f,1.0f},
		.verticalFOV = viewProjection_.fovAngleY_/1.5f,
		.aspectRatio = viewProjection_.aspectRatio_,
		.nearZ = viewProjection_.nearZ_,
		.farZ = 100.0f
	};
	//0.45f, (1280.0f / 720.0f), 0.1f, 1000.0f

	Matrix4x4 cameraMatrix_{};

	// 追従対象
	const EulerTransform* target_ = nullptr;

	// 追従対象
	const Matrix4x4* targetRotateMatrix = nullptr;

	// 追従対象の残像座標
	Vector3 interTarget_ = {};

	float distance;
	float height_ = 1.5f;
	const float offset_t = 0.1f;

	// 位置補完レート
	float t_ = 0.1f;
	//アングル補完レート
	float angle_t = 0.1f;

	// 目標角度
	float destinationAngleY_ = 0.0f;
	float destinationAngleX_ = 0.0f;
	//基準のオフセット
	Vector3 baseOffset;

	Vector3 rootOffset;

	float maxRotate;

	float minRotate;

	float rotateSpeed = 0.05f;

	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//姿勢ベクトル
	const Vector3 Vec = { 0.0f,0.0f,1.0f };
	Vector3 postureVec_{};
	Vector3 frontVec_{};

	//カメラシェイク用変数

	//加算するオフセット
	Vector2 addOffset_{};
	//カメラシェイクのデータ
	CameraShake cameraShake_{};

	//入力
	Input* input_ = nullptr;

	//カメラを動かすことが出来るフラグ
	bool isMove_;
};

