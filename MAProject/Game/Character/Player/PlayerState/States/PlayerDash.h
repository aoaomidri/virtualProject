#pragma once
#include"BasePlayerState.h"
#include"PlayerStateManager.h"

//前方宣言
class LockOn;

class PlayerDash : public BasePlayerState {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>
	PlayerDash(PlayerContext& ctx) : BasePlayerState(ctx) {}

	//ファイルから読み込み代入する
	void ApplyGlobalVariables();
	//変数をImguiやファイルに書き出す
	void InitGlobalVariables() const;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Vector3& cameraRotate)override;
public:
	//InitGlobalVariablesを行ったかどうか
	static bool isLoad_;
private:

	//ダッシュ時のスピード倍率
	float dashSpeed_ = 10.0f;
	//ダッシュのクールタイムのベース
	int dashCoolTimeBase_ = 20;

	
};

