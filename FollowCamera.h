#pragma once
#include"Transform.h"
#include"Input.h"
#include"Adjustment_Item.h"
#include"math/Matrix.h"
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

	void Update(Input* input_);

	void Reset();

	Vector3 offsetCalculation(const Vector3& offset) const;

	void SetTarget(const Transform* target);

	ViewingFrustum& GetViewingFrustum() { return viewingFrustum_; }

	const Transform& GetCameraTransform()const { return cameraTransform; }
private:
	void DrawImgui();

	void ApplyGlobalVariables();

private:
	Transform cameraTransform{
		.scale = {1.0f,1.0f,1.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	Vector3 cameraMove_{};

	const Vector3 baseCameraOffset = { 0.0f,1.0f,-8.0f };

	Vector3 cameraOffset{};

	ViewingFrustum viewingFrustum_;

	// 追従対象
	const Transform* target_ = nullptr;

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

	Vector3 shotOffset;

	Vector3 rootOffset;

	float maxRotate;

	float minRotate;

	float rotateSpeed = 0.05f;

	


};

