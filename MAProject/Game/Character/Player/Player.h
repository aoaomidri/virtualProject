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
#include"PlayerStateManager.h"
#include"GameTime.h"
/*プレイヤーが操作する自機の更新描画*/
/*現在全処理をまとめてしまっているのでステートパターンによる実装を目指し作成中です*/
//前方宣言
class LockOn;

class Player {
public:
	using KnockbackType = HitRecord::KnockbackType;

	~Player();

	//調整項目
	void ApplyGlobalVariables();
	//初期化
	void Initialize();
	//更新処理
	void Update();
	//テクスチャ描画
	void TexDraw(const Matrix4x4& viewProjection);

	//描画
	void Draw(const ViewProjection& viewProjection);

	//描画
	void SkinningDraw(const ViewProjection& viewProjection);
	//パーティクル描画
	void ParticleDraw(const ViewProjection& viewProjection);

	//Imgui描画
	void DrawImgui();

	//床と当たった時のonCollision
	void OnFlootCollision(OBB obb);

	//ヒットバックのタイプが強いものの場合
	bool ChackStrongBack() const {
		if (type_ == HitRecord::Center || type_ == HitRecord::Strong){
			return true;
		}
		else {
			return false;
		}
	}

private:
	//Matrix等の計算処理
	void PlayerCalculation();

public:
	//Getter
	const EulerTransform& GetTransform() const { return playerTransform_; }

	const Vector3& GetTranslate() const { return playerTransform_.translate;}
	const Vector3& GetRotate() const { return playerTransform_.rotate; }
	const Vector3& GetScale() const { return playerTransform_.scale; }
	const Vector3& GetTrailColor() const { return trailRender_->GetTrailColor(); }

	const Matrix4x4& GetRotateMatrix()const { return playerRotateMatrix_; };

	const bool GetIsDown() const { return isDown_; }
	const bool GetIsHitEnemyAttack() const { return isHitEnemyAttack_; }
	const bool GetIsJustAvoid() const { return isJustAvoid_; }
	const bool GetIsGuard()const { return stateManager_->GetIsGuard(); }
	const bool GetIsDash() const { return stateManager_->GetIsDash(); }

	const OBB& GetOBB()const { return playerOBB_; }
	const OBB& GetWeaponOBB()const { return weaponOBB_; }
	const OBB& GetJustAvoidOBB()const { return justAvoidOBB_; }

	const ParticleBase* GetParticle()const { return particle_.get(); }

	const int GetHitTimer()const { return hitTimer_; }

	const uint32_t GetSerialNumber()const { return enemyNumber_; }

	const HitRecord::KnockbackType GetKnockbackType()const { return type_; }

	const float GetHitStop();

	void AddRecord(uint32_t number) { hitRecord_.AddRecord(number); }

	bool RecordCheck(uint32_t number) {return hitRecord_.RecordCheck(number); }
	//非回避中の当たりの処理
	void OnCollisionEnemyAttack();
	//回避中の当たりの処理
	void OnCollisionEnemyAttackAvoid(const uint32_t serialNumber);

	//Setter
	void SetCollisionEnemy(bool collisionEnemy) { isCollisionEnemy_ = collisionEnemy; stateManager_->SetIsCollisionEnemy(collisionEnemy); }

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	void SetIsDown(bool isDown);

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	void SetTimeScale(const float scale) { timeScale_ = scale; }

private:
	//状態を管理
	PlayerStateManager* stateManager_ = nullptr;

	//自機のモデル
	std::unique_ptr<Object3D> playerObj_;
	//武器のモデル
	std::unique_ptr<Object3D> weaponObj_;
	std::unique_ptr<Object3D> weaponCollisionObj_;
	//当たりを見るためのオブジェ
	std::unique_ptr<Object3D> collisionObj_;

	//自機のSRT
	EulerTransform playerTransform_{};
	EulerTransform playerAppearanceTransform_{};
	//武器のSRT
	EulerTransform weaponTransform_{};
	EulerTransform weaponCollisionTransform_{};
	//プレイヤーのマトリックス
	Matrix4x4 playerMatrix_{};
	Matrix4x4 playerScaleMatrix_{};
	Matrix4x4 playerRotateMatrix_{};
	Matrix4x4 playerTranslateMatrix_{};
	Matrix4x4 playerOBBTranslateMatrix_{};
	Matrix4x4 playerOBBScaleMatrix_{};
	Matrix4x4 playerOBBMatrix_{};
	//武器のマトリックス
	Matrix4x4 weaponMatrix_{};
	Matrix4x4 weaponScaleMatrix_{};
	Matrix4x4 weaponCollisionMatrix_{};
	//スケールを無視したマトリックス
	Matrix4x4 playerMoveMatrix_{};

	//落下関連
	float downSpeed_ = -0.01f;
	Vector3 downVector_ = { 0 };

	const ViewProjection* viewProjection_ = nullptr;

	//自機のOBB
	OBB playerOBB_{};
	//ジャスト回避用のOBB
	OBB justAvoidOBB_{};
	//武器のOBB
	OBB weaponOBB_{};
	//プレイヤーの基礎攻撃力
	int baseAttackPower_ = 3;
	//被弾時の無敵時間
	int hitTimer_ = 0;
	int hitTimerBase_ = 30;

	float timeScale_ = 0.0f;
	//ジャスト回避時のエフェクトの補正値
	float postBlend_ = 0.0f;
	//上記の補正値を補正するための値
	float postT_ = 0.0f;
	//補正値の加算量
	float addPostT_ = 0.0f;
	float justAvoidSlowTime_ = 10.0f;
	float slowTimeScale_ = 0.2f;

	//////*攻撃に関連するもの*///////
	//ヒットストップの時間
	float hitStop_ = 0.0f;
	float strongHitStop_ = 0.0f;

	uint32_t enemyNumber_ = 100;

	//強攻撃強化時間
	float counterTimeBase_ = 60.0f;
	float counterTime_ = 0;

	//カウンター時の判定の大きさ
	float shiness_ = 0.0f;

	//ディゾルブ関係
	float weaponThreshold_ = 0.0f;

	float addThresholdSpeed_ = 0.03f;
	float minusThresholdSpeed_ = 0.01f;
	//取得する頂点座標を調整する
	Vector2 trailPosData_ = { 0.0f,3.0f };

	//取得する頂点座標を調整する
	Vector2 trailPosDataGuard_ = { 7.0f,0.5f };

	bool isDissolve_ = false;
	//トレイルを描画するかどうか
	bool isTrail_ = false;
	//追撃を出すかどうか
	bool isNextAttack_ = false;
	////////
	//落下するかどうか
	bool isDown_ = false;

private:
	//敵と衝突しているか
	bool isCollisionEnemy_ = false;
	//敵の攻撃に当たったかどうか
	bool isHitEnemyAttack_ = false;
	//攻撃に回避を合わせたか(ジャスト回避をしているか)
	bool isJustAvoid_ = false;

private:
	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//入力
	Input* input_ = nullptr;

	//音関係
	Audio* audio_ = nullptr;

	float seVolume_ = 0.5f;

	//被弾時の音
	uint32_t playerHitSE_{};

	HitRecord hitRecord_;

	HitRecord::KnockbackType type_;

	std::unique_ptr<ParticleBase> particle_;

	ParticleBase::Emitter emitter_;

	std::unique_ptr<TrailEffect> trail_;

	std::unique_ptr<TrailRender> trailRender_;

	EulerTransform particleTrans_;

	EulerTransform particleTransCenter_;

	std::unique_ptr<Sprite> shadow_;

	float shadowScaleBase_ = 0.9f;

	float shadowScaleCulcBase_ = 2.5f;

	float shadowHeight_ = 1.11f;

	std::unique_ptr<Sprite> groundCrush_;

	float crushColorMinus_ = 0.02f;

	float crushPosBase_ = 1.1f;

	Vector3 groundOffsetBase_;
	//破壊跡を固定するかどうか
	bool isStopCrush_ = false;

	float crushScaleBase_ = 0.9f;

	//Obbの補正値
	Vector3 obbPoint_ = { -0.02f,0.0f,-0.05f };

	//Obbの補正値
	Vector3 obbAddScale_ = { -0.5f,0.5f,0.0f };

	//ジャスト回避用のobbの補正値
	Vector3 justAvoidObbScale_{};

	//大きさ補正の値
	float scaleValue_ = 0.0f;
};

