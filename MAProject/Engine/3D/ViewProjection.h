#pragma once
#include"Matrix.h"
/*view,mat行列の計算「不要なものなので適正な箇所へ記述をして後日削除する予定」*/


class ViewProjection{
public:
	void UpdateMatrix();


public:
#pragma region ビュー行列の設定
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0, 0, 0 };
	// ローカル座標
	Vector3 translation_ = { 0, 0, -50.0f };
#pragma endregion

#pragma region 射影行列の設定
	// 垂直方向視野角
	float fovAngleY_ = 0.45f;
	// ビューポートのアスペクト比
	float aspectRatio_ = (1280.0f / 720.0f);
	// 深度限界（手前側）
	float nearZ_ = 0.1f;
	// 深度限界（奥側）
	float farZ_ = 1000.0f;
#pragma endregion
	//カメラ行列
	Matrix4x4 cameraMatrix_;
	// ビュー行列
	Matrix4x4 matView_;
	// 射影行列
	Matrix4x4 matProjection_;

	// ビュー行列＊射影行列
	Matrix4x4 matViewProjection_;
};

