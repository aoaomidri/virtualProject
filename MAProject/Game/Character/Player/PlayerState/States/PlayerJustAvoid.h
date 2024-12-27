#pragma once
#include"BasePlayerState.h"
#include"PlayerStateManager.h"

//前方宣言
class LockOn;

class PlayerJustAvoid : public BasePlayerState {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>
	PlayerJustAvoid(PlayerContext& ctx) : BasePlayerState(ctx) {}

	//ファイルから読み込み代入する
	void ApplyGlobalVariables();
	//変数をImguiやファイルに書き出す
	void InitGlobalVariables() const;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	//移動部分の初期化
	void InitializeMove();
	//攻撃動作部分の初期化
	void InitializeAttack();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Vector3& cameraRotate)override;

private:
	//回避反撃を行うかどうか
	bool isAvoidAttack_ = false;
	//突きを行うかどうか
	bool isThrust_ = false;

	float waitTimeBase_ = 7.0f;
	float waitTime_ = 0;
	float easeT_ = 0.0f;
	float addEaseT_ = 0.0f;
	float addEaseSpeed_ = 0.04f;
	float addEaseSpeedStrong_ = 0.08f;

	//引きと押し
	Vector2 justAvoidEaseMagnification_ = { 0.5f,4.0f };
	Vector2 justAvoidEaseStart_ = { 1.5f,0.0f };
	Vector2 justAvoidEaseEnd_ = { 0.0f,4.0f };

	Vector3 justAvoidWeaponRotate_ = { 1.57f,0.0f,0.0f };
};

