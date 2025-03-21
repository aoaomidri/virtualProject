#pragma once
#include"BasePlayerState.h"
#include"PlayerStateManager.h"

//前方宣言
class LockOn;

/*プレイヤーの通常時の行動*/

class PlayerRoot : public BasePlayerState {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>
	PlayerRoot(PlayerContext& ctx) : BasePlayerState(ctx) {}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Vector3& cameraRotate)override;

private:
	const float kWeaponRootTranslate_ = 10000.0f;

};

