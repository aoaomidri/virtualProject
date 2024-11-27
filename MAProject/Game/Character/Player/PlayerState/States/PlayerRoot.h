#pragma once
#include"IPlayerState.h"

/*プレイヤーの通常時の行動*/

class PlayerRoot : public IPlayerState{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>
	PlayerRoot(PlayerContext& ctx) : IPlayerState(ctx) {}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

private:
	//ロックオン
	const LockOn* lockOn_ = nullptr;

};

