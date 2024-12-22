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


class IPlayerState{
public:
	//攻撃に関連するパラメーター
	struct  WorkAttack {
		uint32_t attackParameter = 0;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
		bool strongComboNext = false;
		uint32_t AttackTimer = 0;
		uint32_t nextAttackTimer = 0;
	};
	/*状態で共通する変数群*/
	struct PlayerContext {
		//自機のトランスフォーム
		EulerTransform playerTransform_ = {};
		//移動ベクトル
		Vector3 move_{};
		//移動ベクトルの補正
		float moveSpeed_ = 0.0f;

		//向きベクトル
		Vector3 postureVec_{};
		Vector3 frontVec_{};

		//自機の回転行列
		Matrix4x4 playerRotateMatrix_{};
		//攻撃パラメーター
		WorkAttack workAttack_{};
		//ジャンプの強さ
		float jumpPower_ = 0.0f;

		//落下するかどうか
		bool isDown_ = false;

		//落下関連
		float downSpeed_ = 0.0f;
		//落下ベクトル
		Vector3 downVector_ = {};
		//浮遊で利用するサイン角
		float floatSin_ = 0.0f;
		//浮遊速度
		float floatSpeed_ = 0.0f; 
		//ダッシュのクールタイム
		int dashCoolTime_ = 0;

		//ディゾルブするかどうか
		bool isDissolve_ = false;
		//武器の動きをデバックする
		bool isWeaponDebugFlug_ = false;
	};

	enum class StateName{
		Root,/*通常時*/
		Attack,/*弱攻撃時*/
		StrongAttack,/*強攻撃時*/
		Dash,/*ダッシュ時*/
	};

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	virtual void SetStateManager(PlayerStateManager* stateManager) { stateManager_ = stateManager; }

public:
	IPlayerState(PlayerContext& ctx) : context_(ctx) {}
	virtual ~IPlayerState() = default;

protected:
	PlayerContext& context_;

	//キー入力
	Input* input_ = nullptr;
private:
	//ステートマネージャー
	PlayerStateManager* stateManager_ = nullptr;

	
};

