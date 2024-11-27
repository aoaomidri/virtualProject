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
/*プレイヤーが操作する自機の更新描画*/
//前方宣言
class LockOn;

class Player {
public:
	~Player();

	//調整項目
	void ApplyGlobalVariables();
	//初期化
	void Initialize();
	//更新処理
	void Update();

	void TexDraw(const Matrix4x4& viewProjection);

	//描画
	void Draw(const ViewProjection& viewProjection);

	//描画
	void SkinningDraw(const ViewProjection& viewProjection);

	void ParticleDraw(const ViewProjection& viewProjection);

	//Imgui描画
	void DrawImgui();

	//床と当たった時のonCollision
	void OnFlootCollision(OBB obb);

	//リスポーン
	void Respawn();

public:
	//Getter
	const EulerTransform& GetTransform() const { return playerTransform_; }

	const Vector3& GetTranslate() const { return playerTransform_.translate; }

	const Vector3& GetRotate() const { return playerTransform_.rotate; }

	const Vector3& GetScale() const { return playerTransform_.scale; }

	const Vector3& GetTrailColor() const { return trailRender_->GetTrailColor(); }

	const Matrix4x4& GetRotateMatrix()const { return playerRotateMatrix_; };

	const bool GetIsDown() const { return isDown_; }

	const bool GetIsHitEnemyAttack() const { return isHitEnemyAttack_; }

	const bool GetIsJustAvoid() const { return isJustAvoid_; }

	const OBB& GetOBB()const { return playerOBB_; }

	const OBB& GetWeaponOBB()const { return weaponOBB_; }

	const ParticleBase* GetParticle()const { return particle_.get(); }

	const int GetHitTimer()const { return hitTimer_; }

	void AddRecord(uint32_t number) { hitRecord_.AddRecord(number); }

	bool RecordCheck(uint32_t number) {return hitRecord_.RecordCheck(number); }

	void OnCollisionEnemyAttack();

	//Setter
	void SetCollisionEnemy(bool collisionEnemy) { isCollisionEnemy_ = collisionEnemy; }

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	void SetIsDown(bool isDown);

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

private:
	//クラス内関数

	//通常行動初期化
	void BehaviorRootInitialize();
	
	//ダッシュ行動初期化
	void BehaviorDashInitialize();
	// 通常行動更新
	void BehaviorRootUpdate();
	//攻撃行動更新
	void BehaviorAttackUpdate();
	//強攻撃全体の初期化
	void BehaviorAllStrongAttackInitialize();
	//強攻撃全体の更新
	void BehaviorStrongAttackUpdate();
	//ダッシュ行動更新
	void BehaviorDashUpdate();

public:
	
	struct  WorkAttack{
		uint32_t attackParameter = 0;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
		bool strongComboNext = false;
		uint32_t AttackTimer = 0;
		uint32_t nextAttackTimer = 0;
	};

	WorkAttack workAttack_;

	bool endCombo_ = false;

	bool chargeEnd_ = false;

	Vector3 baseRotate_ = { 0 };

	//コンボの数
	static const int ConboNum = 6;

private:
	//弱1攻撃行動初期化
	void BehaviorAttackInitialize();
	//弱2攻撃行動初期化
	void BehaviorSecondAttackInitialize();
	//弱3攻撃行動初期化
	void BehaviorThirdAttackInitialize();
	//弱4攻撃行動初期化
	void BehaviorFourthAttackInitialize();
	//弱5攻撃行動初期化
	void BehaviorFifthAttackInitialize();
	//弱6攻撃行動初期化
	void BehaviorSixthAttackInitialize();
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
	//強攻撃関連

	//強1攻撃行動初期化
	void BehaviorStrongAttackInitialize();
	//強2攻撃行動初期化
	void BehaviorSecondStrongAttackInitialize();
	//強3攻撃行動初期化
	void BehaviorThirdStrongAttackInitialize();
	//強4攻撃行動初期化
	void BehaviorFourthStrongAttackInitialize();
	//強5攻撃行動初期化
	void BehaviorFifthStrongAttackInitialize();
	//強6攻撃行動初期化
	void BehaviorSixthStrongAttackInitialize();
	//強1攻撃のモーション
	void StrongAttackMotion();
	//強2攻撃のモーション
	void SecondStrongAttackMotion();
	//強3攻撃のモーション
	void ThirdStrongAttackMotion();
	//強4攻撃のモーション
	void FourthStrongAttackMotion();
	//強5攻撃のモーション
	void FifthStrongAttackMotion();
	//強6攻撃のモーション
	void SixthStrongAttackMotion();

	/*地面破壊のテクスチャの配置*/
	void SettingGroundCrushTex();

private:
	//自機のモデル
	std::unique_ptr<Object3D> playerObj_;

	//std::vector<std::unique_ptr<Object3D>> debugSphere_;

	std::vector<std::string> animetionNames_;

	//std::vector<Model::Joint> debugJoints_;

	//武器のモデル
	std::unique_ptr<Object3D> weaponObj_;
	std::unique_ptr<Object3D> weaponCollisionObj_;

	//当たりを見るためのオブジェ
	std::unique_ptr<Object3D> collisionObj_;

	//自機のSRT
	EulerTransform playerTransform_{};
	
	//武器のSRT
	EulerTransform weaponTransform_{};
	EulerTransform weaponCollisionTransform_{};

	

	//プレイヤーのマトリックス
	Matrix4x4 playerMatrix_{};

	//std::vector<Matrix4x4> debugMatrix_;

	Matrix4x4 playerScaleMatrix_{};
	Matrix4x4 playerRotateMatrix_{};
	Matrix4x4 playerTransformMatrix_{};

	Matrix4x4 playerOBBTransformMatrix_{};
	Matrix4x4 playerOBBScaleMatrix_{};
	Matrix4x4 playerOBBMatrix_{};

	//武器のマトリックス
	Matrix4x4 weaponMatrix_{};
	Matrix4x4 weaponScaleMatrix_{};
	Matrix4x4 weaponCollisionMatrix_{};

	//スケールを無視したマトリックス
	Matrix4x4 playerMoveMatrix_{};

	//自機の移動
	Vector3 move_{};

	//ジャンプ
	float jumpPower_ = 0.2f;

	//落下関連
	float downSpeed_ = -0.01f;
	Vector3 downVector_ = { 0 };

	//姿勢ベクトル
	Vector3 postureVec_{};
	Vector3 frontVec_{};

	const ViewProjection* viewProjection_ = nullptr;

	//自機のOBB
	OBB playerOBB_{};

	//武器のOBB
	OBB weaponOBB_{};

	//プレイヤーの基礎攻撃力
	int baseAttackPower_ = 3;

	//移動スピード
	float moveSpeed_ = 0.1f;

	//ダッシュ時のスピード倍率
	float dashSpeed_ = 10.0f;
	//ダッシュのクールタイムのベース
	int dashCoolTimeBase_ = 20;
	//被弾時の無敵時間
	int hitTimer_ = 0;
	//代入用ダッシュのクールタイム
	int dashCoolTime_ = 0;

	struct WorkDash {
		//ダッシュ用の媒介変数
		uint32_t dashParameter_ = 0;
	};

	WorkDash workDash_;

	//応刀受付時間のベース
	int justAvoidAttackTimerBase_ = 60;
	//応刀受付時間
	int justAvoidAttackTimer_ = 0;

	//////*攻撃に関連するもの*///////
	//武器の回転
	float weapon_Rotate_ = 0.0f;
	float arm_Rotate_ = -3.15f;
	//武器開店に関連する変数
	Vector3 weapon_offset_{};
	Vector3 weapon_offset_Base_ = { 0.0f,4.0f, 0.0f };

	const float kMoveWeapon_ = 0.1f;
	const float kMoveWeaponShakeDown_ = 0.2f;
	const float kMaxRotate_ = 2.0f;
	const float kMaxRotateY_ = -1.55f;
	const float kMinRotate_ = -0.6f;
	const float kFloatHeight_ = 0.1f;
	//強2攻撃での追撃回数の最大値
	const int32_t kStrongSecondAttackCountMax_ = 2;
	//強2攻撃での追撃回数カウント
	int32_t strongSecondAttackCount_ = 0;

	int32_t waitTimeBase_ = 7;
	int32_t waitTime_ = 0;

	//強攻撃強化時間
	int32_t counterTimeBase_ = 30;
	int32_t counterTime_ = 0;

	//武器の高さ補正
	Vector3 addPosition_ = {};

	float floatSin_ = 0.0f;

	float floatSpeed_ = (float)(std::numbers::pi / 45.0f);

	float motionDistance_ = 3.0f;

	float easeT_ = 0.0f;

	float addEaseT_ = 0.0f;

	float motionSpeed_ = 1.0f;

	float shiness_ = 0.0f;

	//ディゾルブ関係
	float weaponThreshold_ = 0.0f;
	//取得する頂点座標を調整する
	Vector2 trailPosData_ = { 0.0f,3.0f };

	bool isDissolve_ = false;
	//武器を振り下ろしかどうか
	bool isShakeDown_ = false;
	//
	bool isEndAttack_ = false;
	//トレイルを描画するかどうか
	bool isTrail_ = false;
	//追撃を出すかどうか
	bool isNextAttack_ = false;


	/*強5攻撃のみ*/
	//初回の攻撃かどうか
	bool isFirstAttack_ = false;
	//最後のきめ技を行うかどうか
	bool isFinishAttack_ = false;
	////////
	//落下するかどうか
	bool isDown_ = false;
	//武器の振りに対して調整をしたいかどうか
	bool isWeaponDebugFlug_ = false;
	//ダッシュしてるかどうか
	bool isDash_ = false;
	//強攻撃一段目で守っているかどうか
	bool isGuard_ = false;
	//強攻撃中に攻撃を受けたかどうか
	bool isGuardHit_ = false;

	/*振る舞い系*/
	enum class Behavior {
		kRoot,			//通常状態
		kAttack,		//攻撃中
		kStrongAttack,  //強攻撃中
		kDash,			//ダッシュ中

	};

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = Behavior::kRoot;

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

	HitRecord hitRecord_;

	std::unique_ptr<ParticleBase> particle_;

	ParticleBase::Emitter emitter_;

	std::unique_ptr<TrailEffect> trail_;

	std::unique_ptr<TrailRender> trailRender_;

	size_t leftHandNumber_;

	size_t rightHandNumber_;

	EulerTransform particleTrans_;

	EulerTransform particleTransCenter_;

	std::unique_ptr<Sprite> shadow_;

	float shadowScaleBase_ = 0.9f;

	std::unique_ptr<Sprite> groundCrush_;

	Vector3 groundOffsetBase_;
	//破壊跡を固定するかどうか
	bool isStopCrush_ = false;

	float crushScaleBase_ = 0.9f;

	//Obbの補正値
	Vector3 obbPoint_ = { -0.02f,0.0f,-0.05f };

	//Obbの補正値
	Vector3 obbAddScale_ = { -0.5f,0.5f,0.0f };
};

