#pragma once
#include"IPlayerState.h"
#include"PlayerStateManager.h"

//前方宣言
class LockOn;

class PlayerAttack : public IPlayerState{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>
	PlayerAttack(PlayerContext& ctx) : IPlayerState(ctx) {}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

};

