#pragma once
#include"BasePlayerState.h"
#include"PlayerStateManager.h"

//前方宣言
class LockOn; 

class PlayerStrongAttack : public BasePlayerState {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>
	PlayerStrongAttack(PlayerContext& ctx) : BasePlayerState(ctx) {}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Vector3& cameraRotate)override;
private:



	std::array<Vector3, WorkAttack::conboNum_> weaponStrongAttackTransformRotates_ = { {
			{0.0f,0.0f,2.35f},{ 1.57f,0.0f,0.0f},{-0.3f,0.0f,0.0f},{-0.3f,0.0f,1.85f},{-0.3f,0.0f,-1.5f},{1.0f,0.0f,2.35f}
			}
	};

	std::array<Vector3, WorkAttack::conboNum_> weaponStrongAttackOffset_ = { {
		{0.0f,0.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,2.0f,0.0f},{0.0f,1.5f,0.0f},{0.0f,2.0f,0.0f},{0.0f,0.0f,1.0f}
		}
	};

	std::array<Vector2, WorkAttack::conboNum_> weapon_StrongRotatesMinMax_ = { {
		{4.5f,9.2f},{3.16f,-0.9f},{-0.9f,1.35f},{-0.9f,3.16f},{-0.5f,3.0f},{3.3f,-0.3f}
		}
	};
};

