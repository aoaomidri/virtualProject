#pragma once
#include"Vector3.h"
//しすい台の定義
struct ViewingFrustum {
	Vector3 translation_; // カメラの座標
	Vector3 rotate_;      // カメラの回転
	Vector3 direction;    // カメラの向き
	float verticalFOV;    // 縦画角
	float aspectRatio;    // アスペクト比
	float nearZ;          // 深度限界（手前側）
	float farZ;           // 深度限界（奥側）
};