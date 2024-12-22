#pragma once
#include"IPlayerState.h"
#include"PlayerStateManager.h"

//前方宣言
class LockOn;

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

