#pragma once
#include"BasePlayerState.h"
#include"PlayerStateManager.h"

class PlayerLeaningBack : public BasePlayerState {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>
	PlayerLeaningBack(PlayerContext& ctx) : BasePlayerState(ctx) {}

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
	bool isHit_ = false;
private:
	float rotateSpeed_ = 1.5f;

};


