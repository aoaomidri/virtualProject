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

	void TrailDraw();
	//Imgui描画
	void DrawImgui();

	//床と当たった時のonCollision
	void onFlootCollision(OBB obb);

	//リスポーン
	void Respawn();

public:
	//Getter
	const EulerTransform& GetTransform() const { return playerTransform_; }

	const Vector3& GetTranslate() const { return playerTransform_.translate; }

	const Vector3& GetRotate() const { return playerTransform_.rotate; }

	const Vector3& GetScale() const { return playerTransform_.scale; }

	const Matrix4x4& GetRotateMatrix()const { return playerRotateMatrix_; };

	bool GetIsDown() { return isDown_; }

	const OBB& GetOBB()const { return playerOBB_; }

	const OBB& GetWeaponOBB()const { return weaponOBB_; }

	const ParticleBase* GetParticle()const { return particle_.get(); }

	const int GetHitTimer()const { return hitTimer_; }

	void AddRecord(uint32_t number) { hitRecord_.AddRecord(number); }

	bool RecordCheck(uint32_t number) {return hitRecord_.RecordCheck(number); }

	bool OnCollisionEnemy() { return isCollisionEnemy_; }

	//Setter
	void SetCollisionEnemy(bool collisionEnemy) { isCollisionEnemy_ = collisionEnemy; }

	void SetHitTimer();

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

	void BehaviorAllStrongAttackInitialize();

	void BehaviorStrongAttackUpdate(Input* input);
	//ダッシュ行動更新
	void BehaviorDashUpdate();

public:
	/*攻撃に関連する項目*/
	//攻撃用定数
	struct ConstAttack {
		//振りかぶりの時間<frame>
		uint32_t anticipatationTime;
		//ための時間
		uint32_t chargeTime;
		//攻撃ふりの時間
		uint32_t swingTime;
		//硬直時間
		uint32_t recoveryTime;
		//振りかぶりの移動の速さ
		float anticipatationSpeed;
		//ための移動速度
		float chargeSpeed;
		//攻撃振りの移動速度
		float swingSpeed;
	};
	struct  WorkAttack{
		uint32_t attackParameter_ = 0;
		int32_t comboIndex_ = 0;
		int32_t inComboPhase_ = 0;
		bool comboNext_ = false;
		bool strongComboNext_ = false;
		uint32_t AttackTimer_ = 0;
		uint32_t nextAttackTimer = 0;
	};

	WorkAttack workAttack_;

	bool endCombo_ = false;

	bool chargeEnd_ = false;

	Vector3 baseRotate_ = { 0 };

	//コンボの数
	static const int ConboNum = 5;

	static const std::array<ConstAttack, ConboNum>kConstAttacks_;
private:
	//攻撃行動初期化
	void BehaviorAttackInitialize();
	//攻撃行動初期化
	void BehaviorSecondAttackInitialize();
	//攻撃行動
	void BehaviorThirdAttackInitialize();
	//攻撃行動初期化
	void BehaviorFourthAttackInitialize();
	//攻撃行動初期化
	void BehaviorFifthAttackInitialize();
	//攻撃のモーション
	void AttackMotion();

	void secondAttackMotion();

	void thirdAttackMotion();

	void fourthAttackMotion();

	void fifthAttackMotion();

private:
	//強攻撃関連

	//攻撃行動初期化
	void BehaviorStrongAttackInitialize();
	//攻撃行動初期化
	void BehaviorSecondStrongAttackInitialize();
	//攻撃行動
	void BehaviorThirdStrongAttackInitialize();
	//攻撃行動初期化
	void BehaviorFourthStrongAttackInitialize();
	//攻撃行動初期化
	void BehaviorFifthStrongAttackInitialize();
	//攻撃のモーション
	void StrongAttackMotion(Input* input);

	void secondStrongAttackMotion();

	void thirdStrongAttackMotion();

	void fourthStrongAttackMotion();

	void fifthStrongAttackMotion();

private:
	//自機のモデル
	SkinAnimObject3D* playerSkinAnimObj_ = nullptr;

	std::vector<std::unique_ptr<Object3D>> debugSphere_;

	std::vector<std::string> animetionNames_;

	std::vector<Model::Joint> debugJoints_;

	//武器のモデル
	std::unique_ptr<Object3D> weaponObj_;
	std::unique_ptr<Object3D> weaponCollisionObj_;
	std::unique_ptr<Object3D> weaponTopObj_;
	std::unique_ptr<Object3D> weaponTailObj_;

	//自機のSRT
	EulerTransform playerTransform_{};
	
	//武器のSRT
	EulerTransform weaponTransform_{};
	EulerTransform weaponCollisionTransform_{};

	//プレイヤーのマトリックス
	Matrix4x4 playerMatrix_{};

	std::vector<Matrix4x4> debugMatrix_;

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
	float jumpPower = 0.2f;

	//落下関連
	float downSpeed = -0.01f;
	Vector3 downVector = { 0 };

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
	float kDashSpeed = 10.0f;
	//ダッシュのクールタイム
	int kDashCoolTime = 20;

	int hitTimer_ = 0;

	int dashCoolTime;

	struct WorkDash {
		//ダッシュ用の媒介変数
		uint32_t dashParameter_ = 0;
	};

	WorkDash workDash_;

	/*武器に関連するもの*/
	//武器の回転
	float weapon_Rotate = 0.0f;
	float arm_Rotate = -3.15f;
	//武器開店に関連する変数
	Vector3 Weapon_offset;
	Vector3 Weapon_offset_Base = { 0.0f,4.0f, 0.0f };

	const float moveWeapon = 0.1f;
	const float moveWeaponShakeDown = 0.2f;
	const float MaxRotate = 1.55f;
	const float MaxRotateY = -1.55f;
	const float MinRotate = -0.6f;

	int WaitTimeBase = 10;
	int WaitTime = 0;

	float motionDistance_ = 3.0f;

	float easeT_ = 0.0f;

	float motionSpeed_ = 1.0f;

	float shiness_ = 0.0f;

	bool isShakeDown = false;

	bool isEndAttack_ = false;

	//落下するかどうか
	bool isDown_ = false;

	bool isDash_ = false;

	/*振る舞い系*/
	enum class Behavior {
		kRoot,			//通常状態
		kAttack,		//攻撃中
		kStrongAttack,  //強攻撃中
		kDash,			//ダッシュ中

	};

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

private:
	bool isCollisionEnemy_ = false;

private:
	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//入力
	Input* input_ = nullptr;

	HitRecord hitRecord_;

	std::unique_ptr<ParticleBase> particle_;

	//手から出す予定のパーティクル
	std::unique_ptr<ParticleBase> particleHands_;

	/*std::unique_ptr<TrailEffect> trail_;

	std::unique_ptr<TrailRender> trailRender_;*/

	size_t leftHandNumber_;

	size_t rightHandNumber_;

	EulerTransform particleTrans_;

	std::unique_ptr<Sprite> shadow_;

	float shadowScaleBase_ = 0.9f;

	//Obbの補正値
	Vector3 obbPoint_ = { -0.02f,0.75f,-0.05f };

	//Obbの補正値
	Vector3 obbAddScale_ = { -0.65f,-0.2f,-0.65f };

	std::unique_ptr<Object3D> test_;

	EulerTransform testTrans_;
};

