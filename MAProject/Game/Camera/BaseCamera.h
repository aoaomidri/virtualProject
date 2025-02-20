#pragma once
#include"ViewProjection.h"
#include"ViewingFrustum.h"
#include"Matrix.h"
#include <Input.h>
/*カメラの基底クラス*/

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

class BaseCamera{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
protected:
	//入力
	Input* input_ = nullptr;

	//カメラを動かすことが出来るフラグ
	bool isMove_;
	//カメラシェイク用変数

	//加算するオフセット
	Vector2 addOffset_{};
	//カメラシェイクのデータ
	CameraShake cameraShake_{};

	float shakePower_;

};

