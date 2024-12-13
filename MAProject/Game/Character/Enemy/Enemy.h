#pragma once
#include"Transform.h"
#include"TextureManager.h"
#include"Matrix.h"
#include"Object3D/Object3D.h"
#include"ViewProjection.h"
#include"Shape/OBB.h"
#include"Adjustment_Item.h"
#include"Quaternion.h"
//#include"../../Engine/3D/Object3D/Particle.h"
#include"HitRecord.h"
#include<optional>
#include <algorithm>
#include"Ease/Ease.h"
#include"Sprite.h"
#include "ParticleBase.h"
/*敵キャラクターの更新描画*/

class Enemy {
public:
	//コンストラクタ
	Enemy();

	~Enemy();
	//調整項目
	void ApplyGlobalVariables();
	//初期化
	void Initialize(const Vector3& position);
	//更新処理
	void Update();
	//描画
	void Draw(const ViewProjection& viewProjection);
	//テクスチャの描画
	void TexDraw(const Matrix4x4& viewProjection);
	//パーティクルの描画
	void ParticleDraw(const ViewProjection& viewProjection, const Vector3& color);

	//Imgui描画
	void DrawImgui();

	//被弾時の処理
	void OnCollision();
	//被弾時の処理
	void OnCollisionStrong();

	//ガード中に当たったときの処理
	void OnCollisionGuard();
	//当たりの記憶に追加
	void AddRecord(uint32_t number) { hitRecord_.AddRecord(number); }
	//当たりの記憶をチェックする
	bool RecordCheck(uint32_t number) { return hitRecord_.RecordCheck(number); }


public:
	//Getter
	const EulerTransform& GetTransform() const { return transform_; }

	const Vector3 GetCenterPos()const;

	const Matrix4x4& GetRotateMatrix()const { return rotateMatrix_; };

	const OBB& GetAttackOBB()const { return attackOBB_; }

	const OBB& GetBodyOBB()const { return bodyOBB_; }

	bool GetIsDead() const { return isDead_; }

	bool GetIsNoLife() const { return isNoLife_; }

	uint32_t GetSerialNumber()const { return serialNumber_; }

	bool GetIsOnScreen()const { return isOnScreen_; }

	//Setter

	void SetShininess(const float shininess) {
		bodyObj_->SetShininess(shininess);
		partsObj_->SetShininess(shininess);
	}

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	void SetIsDead(bool isDead) { isDead_ = isDead; }

	void SetTarget(const EulerTransform* target) { target_ = target; }

	void SetTargetMat(const Matrix4x4* target) { targetRotateMat_ = target; }

	void SetTimeScale(const float scale) { timeScale_ = scale; }

	void SetIsOnScreen(const bool flag) { isOnScreen_ = flag; }

	void SetKnockBackType(const HitRecord::KnockbackType type) { type_ = type; }

	static uint32_t GetEnemyDestroyingNum() {
		return enemyDestroyingNumber_;
	}

private:
	//クラス内関数
	void ParticleMove();

private:
	static const int particleNum_ = 20;

	//敵の体力
	int32_t enemyLifeMax_ = 0;

	int enemyLife_ = 0;

	int damege_ = 1;

	int strongDamege_ = 3;

	//自機のモデル
	std::unique_ptr<Object3D> bodyObj_;
	std::unique_ptr<Object3D> partsObj_;

	const std::string enemyTexPath_ = "resources/Model/Enemy/EnemyTex.png";
	const std::string enemyHitTexPath_ = "resources/Model/Enemy/EnemyHitTex.png";
	
	HitRecord hitRecord_;

	std::unique_ptr<ParticleBase> particle_;

	ParticleBase::Emitter emitter_;

	std::unique_ptr<Object3D> collisionObj_;

	//プレイヤーの座標
	// 追従対象
	const EulerTransform* target_ = nullptr;
	//プレイヤーの回転
	const Matrix4x4* targetRotateMat_ = nullptr;

	//自機のSRT
	EulerTransform transform_{};
	EulerTransform partsTransform_{};
	EulerTransform collisionTransform_{};

	//プレイヤーのマトリックス
	Matrix4x4 matrix_{};

	Matrix4x4 scaleMatrix_{};
	Matrix4x4 rotateMatrix_{};
	Matrix4x4 transformMatrix_{};

	Matrix4x4 collisionMatrix_{};

	Matrix4x4 partsMatrix_{};

	Matrix4x4 dashRotateMatrix_{};

	//スケールを無視したマトリックス
	Matrix4x4 moveMatrix_{};

	Matrix4x4 playerMat_{};

	//自機の移動
	Vector3 move_{};

	//姿勢ベクトル
	Vector3 postureVec_{};
	Vector3 frontVec_{};

	//パーツ関連
	Vector3 parts_offset_Base_ = { 0.0f, 1.5f, 0.0f };
	Vector3 parts_offset_ = { 0.0f, 1.5f, 0.0f };
	const Vector3 kPartsScaleBase_ = { 0.9f,0.9f,0.9f };
	const float kPartsRotateZ_ = 1.57f;

	//カメラの情報
	const ViewProjection* viewProjection_ = nullptr;

	//攻撃のOBB
	OBB attackOBB_{};

	//自機のOBB
	OBB bodyOBB_{};

	const Vector3 addOBBSize_ = { 3.0f,2.0f,3.0f };

	Vector3 worldPos_ = {};

	Vector3 deadMoveBase_ = { 0,0.02f,0.1f };
	Vector3 deadMove_ = { 0,0.02f,0.1f };
	float deadMoveSpeed_ = 0.5f;
	float deadRotateSpeed_ = 0.3f;

	//移動限界 xが+側、yが-側
	Vector2 limitPos_{ 70.0f,-70.0f };

	float enemyLimitPos_ = 15.0f;
	//座標補正
	float positionCoordinate_ = 69.9f;

	float deadYAngle_ = 0;

	//プレイヤーとの距離を保存する用の変数
	float playerLength_ = 0;

	//プレイヤーが近くにいるときに溜まる
	int nearTime_ = 0;
	//近い条件
	float nearPlayer_ = 10.0f;

	//プレイヤーが遠くにいるときに溜まる
	int farTime_ = 0;
	//遠い条件
	float farPlayer_ = 30.0f;

	//距離に応じた反応の時間制限
	int lengthJudgment_ = 180;

	int freeTime_ = 0;
	int	freeTimeMax_ = 40;

	const float kTranslateHeight_ = 2.5f;

	/// <summary>
	///倍率
	/// </summary>
	float magnification_ = 1.0f;
	float runMagnification_ = 6.0f;

	//移動スピード
	float moveSpeed_ = 0.03f;
	//ダッシュの時間
	float dashSpeed_ = 3.0f;
	//バックステップの時間
	float backSpeed_ = -2.0f;
	//ヒットバックの速度
	float hitBackSpeed_ = 1.0f;
	//強ヒットバックの速度
	float fewHitBackSpeed_ = 0.015f;
	//強ヒットバックの速度
	float strongHitBackSpeed_ = 1.0f;
	//ゲームの時間
	float timeScale_ = 0.0f;
	//パーティクルの速度
	float particleSpeed_ = 0.0f;	

	float rotateEaseT_ = 0.0f;

	const float addRotateEaseT_ = (1.0f / 30.0f);
	//ダッシュの時間
	int dashTimer_ = 0;

	//くらった時のジャンプ
	float jumpPower_ = 0.4f;

	//落下関連
	float downSpeed_ = -0.02f;
	Vector3 downVector_ = { 0 };

	bool isDead_ = false;
	//近接攻撃中かどうか
	bool isNearAttack_ = false;
	//体力がなくなったかどうか
	bool isNoLife_ = false;
	//画面内にいるかどうか
	bool isOnScreen_ = false;
	//パーティクルを発生させるかどうか
	bool isParticle_ = false;	

	HitRecord::KnockbackType type_;

	//ノックバックイージング
	Vector3 knockBackEaseStart_{};

	//左から攻撃を受けたときのノックバック
	Vector3 hitEaseStartLeft_ = { -0.3f,0.0f,-0.3f };

	//左から攻撃を受けたときのノックバック
	Vector3 hitEaseStartRight_ = { 0.3f,0.0f,-0.3f };

	//左から攻撃を受けたときのノックバック
	Vector3 hitEaseStartCenter_ = { -0.3f,0.0f,0.0f };

	Vector3 hitEaseStartStrong_ = { -1.57f,0.0f,0.0f };

private:
	/*振る舞い系*/
	enum class Behavior {
		kPreliminalyAction, //予備動作
		kAttack,			//攻撃
		kSelectAttack,		//確定行動攻撃
		kRoot,				//様子見中
		kRun,				//走り
		kFree,				//遊びの時間
		kDead,				//やられた
		kLeaningBack		//のけぞり
	};


	Behavior behavior_ = Behavior::kRoot;



	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	std::unique_ptr<Sprite> shadow_;

	float shadowScaleBase_ = 0.9f;

	const float kShadowHeightBase_ = 1.11f;

private:
	//行動全体を制御する
	void MotionUpdate();

	//攻撃行動初期化
	void BehaviorRootInitialize();
	//様子見状態の行動
	void RootMotion();
	//攻撃行動初期化
	void BehaviorDeadInitialize();

	//死んだときのモーション
	void DeadMotion();

	//前進行動初期化
	void BehaviorRunInitialize();
	//ダッシュモーション
	void EnemyRun();

	//遊び行動初期化
	void BehaviorFreeInitialize();
	//遊び
	void Free();


	//予備動作初期化
	void BehaviorPreliminalyActionInitialize();
	//予備動作
	void PreliminalyAction();

	//のけぞり動作初期化
	void BehaviorLeaningBackInitialize();
	//のけぞり動作
	void LeaningBack();


private:
	//攻撃関係の関数群
	
	enum class AttackBehavior {
		kTackle,		//突進攻撃.OK
		kNone,			//何もしない
	};

	AttackBehavior ATBehavior_ = AttackBehavior::kNone;

	std::optional<AttackBehavior> ATBehaviorRequest_ = std::nullopt;

	//攻撃行動初期化
	void BehaviorAttackInitialize();

	//形態ごとにとる行動
	void AttackMotion();

	//突進攻撃初期化
	void AttackBehaviorTackleInitialize();
	//突進攻撃
	void Tackle();


private:
	//攻撃関係の変数群

	/*三連切り*/
	//三連切りの移動座標用
	std::array<Vector3, 3> posContainer_;

	//間の待ちの時間
	uint32_t attackDistance_ = 0;

	//待ちに入るための
	bool isAttackEnd_ = false;

	//移動のコンテナがすべて埋まったかどうか
	bool isMaxContext_ = false;

	//現在の攻撃回数
	uint32_t attackCount_ = 0;

	const uint32_t distanceTime_ = 20;

	/*突進攻撃*/
	//突進の向き決めの時間
	uint32_t directionTimeBase_ = 90;
	//突進の向き決めの時間
	uint32_t directionTime_ = 0;

	//突進攻撃の範囲
	float collisionScale_ = 1.3f;

	//突進への移行時間
	uint32_t attackTransitionTimeBase_ = 30;

	uint32_t attackTransitionTime_ = 0;

	const int32_t kDashTime_ = 30;

	/*回転突進攻撃*/

	//攻撃の飛距離
	float attackLength_ = 20.0f;
	//攻撃の目標地点
	Vector3 attackSetPos_{};

	Vector3 attackBasePos_{};

	float easeT_ = 0;

	float attackRotate_ = 0;

	Ease ease_{};


	Vector3 slashAngle_{};

private:
	Vector4 enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };

	float colorSpeed_ = 0.02f;

	float colorLimit_ = 0.2f;

	//ディゾルブ関係
	float threshold_ = 0.0f;
	
	float thresholdSpeed_ = 0.0075f;

	//シリアルナンバー
	uint32_t serialNumber_ = 0;
	//次のシリアルナンバー
	static uint32_t nextSerialNumber_;

	//敵全体での撃破数
	static uint32_t enemyDestroyingNumber_;
};

