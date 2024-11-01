#pragma once
#include"Matrix.h"
/*デバック用のカメラ*/
class DebugCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>


private:
	//X,Y,Z軸周りのローカル回転角
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	//ローカル座標
	Vector3 translation_ = { 0.0f,0.0f,-50.0f };
	//ビュー行列
	Matrix4x4 viewMatrix{};
	//射影行列
	Matrix4x4 projectionMatrix{};

};

