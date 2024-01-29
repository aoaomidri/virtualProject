#pragma once
#include"Transform.h"
#include"Input.h"
#include"Adjustment_Item.h"
#include"ViewProjection.h"
#include"Matrix.h"

//前方宣言
class LockOn;

struct ViewingFrustum {
	Vector3 translation_; // カメラの座標
	Vector3 rotate_;      // カメラの回転
	Vector3 direction;    // カメラの向き
	float verticalFOV;    // 縦画角
	float aspectRatio;    // アスペクト比
	float nearZ;          // 深度限界（手前側）
	float farZ;           // 深度限界（奥側）
};

class FollowCamera{
public:
	void Initialize();

	void Update();

	void Reset();

	void RotateReset();

	Vector3 offsetCalculation(const Vector3& offset) const;

	void DrawImgui();
public:

	void SetTarget(const Transform* target);

	void SetTargetMatrix(const Matrix4x4* target) { targetRotateMatrix = target; }

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }
public:
	ViewingFrustum& GetViewingFrustum() { return viewingFrustum_; }

	ViewingFrustum& GetLockViewingFrustum() { return lockViewingFrustum_; }

	const ViewProjection& GetViewProjection()const { return viewProjection_; }
private:
	

	void ApplyGlobalVariables();

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
	const Transform* target_ = nullptr;

	// 追従対象
	const Matrix4x4* targetRotateMatrix = nullptr;

	// 追従対象の残像座標
	Vector3 interTarget_ = {};

	float distance;
	const float offset_t = 0.1f;

	// 位置補完レート
	float t = 0.1f;
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

	//入力
	Input* input_ = nullptr;
};

