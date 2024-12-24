#pragma once
#include"BasePlayerState.h"
#include"PlayerStateManager.h"

//前方宣言
class LockOn;

class PlayerAttack : public BasePlayerState {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ctx"></param>
	PlayerAttack(PlayerContext& ctx) : BasePlayerState(ctx) {}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	//弱行動共通の初期化
	void PreAttackInitialize();
	//弱行動共通の初期化
	void PostAttackInitialize();

	//弱1攻撃行動初期化
	void AttackInitialize();
	//弱2攻撃行動初期化
	void SecondAttackInitialize();
	//弱3攻撃行動初期化
	void ThirdAttackInitialize();
	//弱4攻撃行動初期化
	void FourthAttackInitialize();
	//弱5攻撃行動初期化
	void FifthAttackInitialize();
	//弱6攻撃行動初期化
	void SixthAttackInitialize();
	

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Vector3& cameraRotate)override;

	//弱1攻撃のモーション
	void AttackMotion();
	//弱2攻撃のモーション
	void SecondAttackMotion();
	//弱3攻撃のモーション
	void ThirdAttackMotion();
	//弱4攻撃のモーション
	void FourthAttackMotion();
	//弱5攻撃のモーション
	void FifthAttackMotion();
	//弱6攻撃のモーション
	void SixthAttackMotion();


private:
	bool isShakeDown_ = false;
	//攻撃が終わったかどうか
	bool isEndAttack_ = false;
	//トレイルを描画するかどうか

	float waitTimeBase_ = 7.0f;
	float waitTime_ = 0;
	float motionDistance_ = 3.0f;
	float easeT_ = 0.0f;
	float easeSecondStrong_ = 0.3f;
	float addEaseT_ = 0.0f;
	float addEaseSpeed_ = 0.04f;
	float addEaseSpeedStrong_ = 0.08f;
	float motionSpeed_ = 1.0f;
	//次の攻撃に映るまでの時間
	//初回
	float nextAttackTimerFirst_ = 21.0f;
	//それ以外
	float nextAttackTimer_ = 28.0f;

	const float kAttackParameterCorection_ = 4.0f;
	const float kAttackMagnification_ = 1.5f;
	const float kAttackDivisionMagnification_ = 2.0f;

	std::array<float, WorkAttack::conboNum_> weapon_Rotates_ = {
		-0.5f,-0.4f,-0.4f,0.0f,-0.4f,0.0f
	};

	std::array<Vector2, WorkAttack::conboNum_> weapon_RotatesMinMax_ = { {
		{2.4f,-1.0f},{3.16f,-0.9f},{3.16f,-0.9f},{9.44f,0.0f},{3.16f,-0.9f},{1.65f,-0.6f}
		}
	};

	std::array<Vector3, WorkAttack::conboNum_> weaponAttackTransformRotates_ = { {
		{0.0f,0.0f,-0.5f},{-0.3f,0.0f,2.0f},{-0.3f,0.0f,-1.7f},{0.0f,0.0f,-2.2f},{-0.3f,0.0f,2.5f},{-0.3f,0.0f,-0.0f}
		}
	};

};

