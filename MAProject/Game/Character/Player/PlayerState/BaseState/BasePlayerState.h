#pragma once
#include"Transform.h"
#include"TextureManager.h"
#include"Matrix.h"
#include"Object3D.h"
#include"SkinAnimObject3D.h"
#include"ViewProjection.h"
#include"Input.h"
#include"OBB.h"
#include"Sprite.h"
#include"Adjustment_Item.h"
#include"Quaternion.h"
#include"HitRecord.h"
#include"ParticleBase.h"
#include<optional>
#include"Effect/TrailRender.h"
#include"Audio.h"
#include"GameTime.h"

/*プレイヤーの状態の基本設定*/
//前方宣言
class PlayerStateManager;

//前方宣言
class LockOn;

class BasePlayerState{
public:
	//攻撃に関連するパラメーター
	struct  WorkAttack {
		bool comboNext_ = false;
		bool strongComboNext_ = false;
		bool trailResetFlug_ = false;
		bool hitRecordRestFlug_ = false;
		bool isShakeDown_ = false;
		bool isStrongHitStop_ = false;
		//攻撃が終わったかどうか
		bool isEndAttack_ = false;
		int32_t comboIndex_ = 0;
		int32_t inComboPhase_ = 0;
		float attackParameter_ = 0;
		float AttackTimer_ = 0;
		float nextAttackTimer_ = 0;
		//コンボの数
		static const int conboNum_ = 6;
		//攻撃ののけぞりの種類
		HitRecord::KnockbackType type_;
	};
	//ダッシュに関するパラメーター
	struct WorkDash {
		//ダッシュしてるかどうか
		bool isDash_ = false;
		//ダッシュ用の媒介変数
		float dashParameter_ = 0;
	};	
	//回避に関連するパラメーター
	struct WorkAvoidAttack {
		//応刀受付時間のベース
		int justAvoidAttackTimerBase_ = 60;
		//応刀受付時間
		int justAvoidAttackTimer_ = 0;
		//突進攻撃のタイマー
		float avoidTimer_ = 0.0f;
		//突進攻撃のタイマーの最大値
		float avoidTimerBase_ = 0.5f;
		//突進のスピード
		float avoidSpeed_ = 1.0f;
		//突進攻撃の判定のタイマー
		float avoidHitTimer_ = 0.0f;
		//突進攻撃の判定のタイマーの最大値
		float avoidHitTimerBase_ = 0.05f;

		//派生用フラグ
		bool isChangeEndAttack_ = false;
	};
	//武器に関するパラメーター
	struct WeaponParameter {
		//武器のSRT
		EulerTransform weaponTransform_{};
		EulerTransform weaponCollisionTransform_{};

		//加算する武器の回転
		float weapon_Rotate_ = 0.0f;

		//武器の高さ補正
		Vector3 addPosition_ = {};
		Vector3 justAvoidWeaponRotate_ = { 1.57f,0.0f,0.0f };

		const Vector3 kWeaponScale_ = { 0.15f,0.15f ,0.15f };
		const Vector3 kWeaponCollisionBase_ = { 0.9f,3.0f,0.9f };

		//武器のマトリックス
		Matrix4x4 weaponMatrix_{};
		Matrix4x4 weaponScaleMatrix_{};
		Matrix4x4 weaponCollisionMatrix_{};

		//武器のOBB
		OBB weaponOBB_{};

		Vector3 weapon_offset_{};
		Vector3 weapon_offset_Base_ = { 0.0f,4.0f, 0.0f };
		Vector3 weapon_offset_RootBase_ = { 0.4f,-1.8f,1.0f };

		const float kMoveWeapon_ = 0.1f;
		const float kMoveWeaponShakeDown_ = 0.2f;
		const float kMaxRotate_ = 2.0f;
		const float kMaxRotateY_ = -1.55f;
		const float kMinRotate_ = -0.6f;
		const float kFloatHeight_ = 0.1f;
		const float kWeapon_offset_ = 2.0f;
		//ディゾルブ関係
		float weaponThreshold_ = 0.0f;

	};
	
	/*どの状態でも共通する変数群*/
	struct PlayerContext {
		//自機のトランスフォーム
		EulerTransform playerTransform_ = {};
		//自機の見た目のトランスフォーム
		EulerTransform appearanceTransform_ = {};

		//移動限界 xが+側、yが-側
		Vector2 limitPos_{ 70.0f,-70.0f };

		//移動ベクトル
		Vector3 move_{};
		//向きベクトル
		Vector3 postureVec_{};
		Vector3 frontVec_{};
		//カメラの回転
		Vector3 cameraRotate_{};
		//ロックオン対象の座標
		Vector3 lockOnPos_{};
		//落下ベクトル
		Vector3 downVector_{};

		//自機の回転行列
		Matrix4x4 playerRotateMatrix_{};

		//攻撃パラメーター
		WorkAttack workAttack_{};
		//ダッシュパラメーター
		WorkDash workDash_{};
		//ジャスト回避パラメーター
		WorkAvoidAttack workAvoidAttack_{};
		//武器のパラメーター
		WeaponParameter weaponParameter_{};

		//ダッシュのクールタイム
		int dashCoolTime_ = 0;

		//ジャンプの強さ
		float jumpPower_ = 0.0f;
		//移動ベクトルの補正
		float moveSpeed_ = 0.0f;
		//落下関連
		float downSpeed_ = 0.0f;
		//浮遊で利用するサイン角
		float floatSin_ = 0.0f;
		//浮遊速度
		float floatSpeed_ = 0.0f; 
		float moveLimitMinimum_ = 0.0005f;
		float moveCorrection_ = 3.0f;
		//ヒビテクスチャの透明度
		float groundCrushTexAlpha_ = 1.0f;

		//落下するかどうか
		bool isDown_ = false;
		//ダッシュしてるかどうか
		bool isDash_ = false;
		//ジャンプするかどうか
		bool isJump_ = false;
		//攻撃のジャンプするかどうか
		bool isAttackJump_ = false;
		//攻撃に回避を合わせたか(ジャスト回避をしているか)
		bool isJustAvoid_ = false;
		//回避反撃を行うかどうか
		bool isAvoidAttack_ = false;
		//敵と衝突しているか
		bool isCollisionEnemy_ = false;
		//ディゾルブするかどうか
		bool isDissolve_ = false;
		//武器の動きをデバックする
		bool isWeaponDebugFlug_ = false;
		//地面テクスチャを固定するかどうか
		bool isStopCrush_ = false;
		//トレイルを描画するかどうか
		bool isTrail_ = false;
		//強攻撃一段目で守っているかどうか
		bool isGuard_ = false;
		//強攻撃中に攻撃を受けたかどうか
		bool isGuardHit_ = false;

		std::unique_ptr<TrailEffect> trail_;
	};

	enum class StateName{
		Root,/*通常時*/
		Attack,/*弱攻撃時*/
		StrongAttack,/*強攻撃時*/
		Dash,/*ダッシュ時*/
		kJustAvoid,/*ジャスト回避中*/
	};

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update(const Vector3& cameraRotate) = 0;

	virtual void SetStateManager(PlayerStateManager* stateManager) { stateManager_ = stateManager; }

public:
	BasePlayerState(PlayerContext& ctx) : context_(ctx) {}
	virtual ~BasePlayerState() = default;

protected:
	PlayerContext& context_;
	//音
	Audio* audio_ = nullptr;
	//キー入力
	Input* input_ = nullptr;
	//ロックオン
	const LockOn* lockOn_ = nullptr;

	float seVolume_ = 0.5f;

	//回避の時の音
	uint32_t avoidSE_{};
	//武器を振るときの音
	uint32_t attackMotionSE_{};
	//被弾時の音
	uint32_t playerHitSE_{};
	//ステートマネージャー
	PlayerStateManager* stateManager_ = nullptr;
private:

	
};

