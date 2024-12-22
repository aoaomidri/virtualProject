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

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Vector3& cameraRotate)override;

};

