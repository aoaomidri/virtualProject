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

	//ファイルから読み込み代入する
	void ApplyGlobalVariables();
	//変数をImguiやファイルに書き出す
	void InitGlobalVariables() const;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	//強攻撃全体の初期化
	void AllStrongAttackInitialize();
	//強行動共通の初期化
	void PreStrongAttackInitialize();
	//強行動共通の初期化
	void PostStrongAttackInitialize();

	//強1攻撃行動初期化
	void StrongAttackInitialize();
	//強2攻撃行動初期化
	void SecondStrongAttackInitialize();
	//強2弱派生攻撃初期化
	void SSAttackWeakVersionInitialize();
	//強2強派生攻撃初期化
	void SSAttackStrongVersionInitialize();
	//強3攻撃行動初期化
	void ThirdStrongAttackInitialize();
	//強4攻撃行動初期化
	void FourthStrongAttackInitialize();
	//強5攻撃行動初期化
	void FifthStrongAttackInitialize();
	//強6攻撃行動初期化
	void SixthStrongAttackInitialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Vector3& cameraRotate)override;

	void DrawImgui();
	//強1攻撃のモーション
	void StrongAttackMotion();
	//強2攻撃のモーション
	void SecondStrongAttackMotion();
	//強2攻撃弱版派生
	void SSAttackWeakVersion();
	//強2攻撃強版派生
	void SSAttackStrongVersion();

	//強3攻撃のモーション
	void ThirdStrongAttackMotion();
	//強4攻撃のモーション
	void FourthStrongAttackMotion();
	//強5攻撃のモーション
	void FifthStrongAttackMotion();
	//強6攻撃のモーション
	void SixthStrongAttackMotion();

	void AppearanceRotate();

	/*地面破壊のテクスチャの配置*/
	void SettingGroundCrushTex();

public:
	//InitGlobalVariablesを行ったかどうか
	static bool isLoad_;

private:
	//追撃を出すかどうか
	bool isNextAttack_ = false;
	//強派生かどうか
	bool isStrongVersion_ = false;
	//初回の攻撃かどうか
	bool isFirstAttack_ = false;
	//最後のきめ技を行うかどうか
	bool isFinishAttack_ = false;
	bool chargeEnd_ = false;

	//強2攻撃での追撃回数の最大値
	const int32_t kStrongSecondAttackCountMax_ = 2;
	//強2攻撃での追撃回数カウント
	int32_t strongSecondAttackCount_ = 0;

	float waitTimeBase_ = 7.0f;
	float waitTime_ = 0;
	float motionDistance_ = 3.0f;
	float easeT_ = 0.0f;
	float easeSecondStrong_ = 0.3f;
	float addEaseT_ = 0.0f;
	float addEaseSpeed_ = 0.04f;
	float addEaseSpeedStrong_ = 0.08f;
	float motionSpeed_ = 1.0f;
	float strongAttackRotateZ_ = 1.57f;
	float strongSixthAttackRotate_ = 1.6f;
	float secondWeapon_Rotate_ = -0.4f;	
	float secondRotateSpeed_ = 2.0f;
	float fifthWeapon_Rotate_ = 0.5f;
	float maxRotateX_ = 6.78f;
	/*当たり関係*/
	float strongAddScale_ = 3.0f;
	float strongSecondAddScale_ = 9.0f;
	//次の攻撃に映るまでの時間
	//初回
	float nextAttackTimerFirst_ = 21.0f;
	//それ以外
	float nextAttackTimer_ = 28.0f;

	const float kAttackParameterCorection_ = 4.0f;
	const float kAttackMagnification_ = 1.5f;
	const float kAttackDivisionMagnification_ = 2.0f;
	const float kGuadeMagnification_ = 5.0f;
	const float kStrongAttackMagnification_ = 3.0f;	
	
	//強2弱派生の制限
	Vector2 strongSecondRotatesMinMax_ = { -0.6f,14.21f };
	//強2強派生の初期回転
	Vector3 strongSecondRotate_ = { 3.14f,0.0f,0.0f };

	Vector3 fifthWeaponCollisionScale_ = { 0.9f,4.5f,0.9f };
	//カウンター時の判定の大きさ
	Vector3 counterScale_{};
	Vector3 collsionScaleGuade_ = { 0.6f,2.0f,0.6f };



	std::array<Vector3, WorkAttack::conboNum_> weaponStrongAttackTransformRotates_ = { {
			{0.0f,0.0f,2.35f},{-0.3f,0.0f,2.8f},{-0.3f,0.0f,0.0f},{-0.3f,0.0f,1.85f},{-0.3f,0.0f,-1.5f},{1.0f,0.0f,2.35f}
			}
	};
	std::array<Vector3, WorkAttack::conboNum_> weaponStrongAttackOffset_ = { {
		{0.0f,-0.5f,1.0f},{0.0f,2.0f,0.0f},{0.0f,2.0f,0.0f},{0.0f,1.5f,0.0f},{0.0f,2.0f,0.0f},{0.0f,0.0f,1.0f}
		}
	};
	std::array<Vector2, WorkAttack::conboNum_> weapon_StrongRotatesMinMax_ = { {
		{4.5f,9.2f},{-0.6f,2.35f},{-0.9f,1.35f},{-0.9f,3.16f},{-0.5f,3.0f},{3.3f,-0.3f}
		}
	};
};

