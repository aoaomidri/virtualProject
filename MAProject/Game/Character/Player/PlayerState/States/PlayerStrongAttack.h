#pragma once
#include"IPlayerState.h"
class PlayerStrongAttack : public IPlayerState{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>
	PlayerStrongAttack(PlayerContext& ctx) : IPlayerState(ctx) {}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

};

