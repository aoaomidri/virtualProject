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
#include"EnemyBullet.h"
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

	void TexDraw(const Matrix4x4& viewProjection);

	void ParticleDraw(const ViewProjection& viewProjection, const Vector3& color);

	//Imgui描画
	void DrawImgui();

	//床と当たった時のonCollision
	void onFlootCollision(OBB obb);

	//リスポーン
	void Respawn(const Vector3& position);

	void OnCollision();

	void AddRecord(uint32_t number) { hitRecord_.AddRecord(number); }

	bool RecordCheck(uint32_t number) { return hitRecord_.RecordCheck(number); }


public:
	//Getter
	const EulerTransform& GetTransform() const { return transform_; }

	const Vector3 GetCenterPos()const;

	const Matrix4x4& GetRotateMatrix()const { return rotateMatrix_; };

	const OBB& GetOBB()const { return OBB_; }

	const OBB& GetAttackOBB()const { return attackOBB_; }

	const OBB& GetBodyOBB()const { return bodyOBB_; }

	bool GetIsDead() const { return isDead_; }

	bool GetIsNoLife() const { return isNoLife_; }

	uint32_t GetSerialNumber()const { return serialNumber_; }

	//Setter

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	void SetIsDead(bool isDead) { isDead_ = isDead; }

	void SetTarget(const EulerTransform* target) { target_ = target; }

	void SetTargetMat(const Matrix4x4* target) { targetRotateMat_ = target; }

	static uint32_t GetEnemyDestroyingNum() {
		return enemyDestroyingNumber_;
	}

private:
	//クラス内関数
	void ParticleMove();

private:
	static const int particleNum_ = 20;

	//自機のモデル
	std::unique_ptr<Object3D> bodyObj_;
	std::unique_ptr<Object3D> partsObj_;
	

	//自機のモデル
	//Object3D* boxObj_;
	
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

	//自機の移動
	Vector3 move_{};

	//姿勢ベクトル
	Vector3 postureVec_{};
	Vector3 frontVec_{};

	const ViewProjection* viewProjection_ = nullptr;

	//自機のOBB
	OBB OBB_{};

	OBB attackOBB_{};

	//自機のOBB
	OBB bodyOBB_{};

	Vector3 worldPos = {};

	Vector3 deadMove_ = { 0,0.02f,0.1f };

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


	/// <summary>
	///倍率
	/// </summary>
	float magnification = 1.0f;
	//移動スピード
	float moveSpeed_ = 0.03f;

	float dashSpeed_ = 3.0f;

	float backSpeed_ = -2.0f;

	float hitBackSpeed_ = 1.0f;

	Vector3 hitEaseStart_ = { -0.3f,0.0f,-0.3f };

	float rotateEaseT_ = 0.0f;

	const float addRotateEaseT_ = (1.0f / 30.0f);

	int dashTimer_ = 0;

	bool isDead_ = false;
	//近接攻撃中かどうか
	bool isNearAttack_ = false;

	bool isNoLife_ = false;

	int32_t enemyLifeMax_ = 3;

	int enemyLife_ = 10;

	bool isParticle_ = false;

	float particleSpeed_;

	

private:
	/*振る舞い系*/
	enum class Behavior {
		kPreliminalyAction, //予備動作
		kAttack,			//攻撃
		kSelectAttack,		//確定行動攻撃
		kRoot,				//様子見中
		kBack,				//後ろにジャンプ
		kDash,				//ダッシュ
		kRun,				//走り
		kFree,				//遊びの時間
		kDead,				//やられた
		kLeaningBack		//のけぞり
	};


	Behavior behavior_ = Behavior::kRoot;



	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	std::unique_ptr<Sprite> shadow_;

	float shadowScaleBase_ = 0.9f;

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
	//後退行動初期化
	void BehaviorBackInitialize();
	//後ろに下がるモーション
	void BackStep();
	//前進行動初期化
	void BehaviorDashInitialize();
	//ダッシュモーション
	void Dash();

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
	/*.OK = 実装済み*/
	enum class AttackBehavior {
		kTriple,		//三連撃.OK
		kCharge,		//近づいて溜めて一撃(X字で剣を振る)
		kBeam,			//ビーム攻撃(保留)
		kTackle,		//突進攻撃.OK
		kRotateAttack,	//回転して追尾しながらの攻撃
		kChargeStrong,	//溜めて剣を大きくして攻撃(縦振り)
		kXAttack,		//その場でX字を書いて攻撃(斬撃は飛ぶ)
		kNone,			//何もしない
	};

	AttackBehavior ATBehavior_ = AttackBehavior::kNone;

	std::optional<AttackBehavior> ATBehaviorRequest_ = std::nullopt;

	//攻撃行動初期化
	void BehaviorAttackInitialize();

	//形態ごとにとる行動
	void AttackMotion();

	//三連攻撃初期化
	void AttackBehaviorTripleInitialize();
	//三連攻撃の更新処理
	void TripleAttack();

	//突進攻撃初期化
	void AttackBehaviorTackleInitialize();
	//突進攻撃
	void Tackle();

	//回転突進攻撃初期化
	void AttackBehaviorRotateAttackInitialize();
	//回転突進攻撃
	void RotateAttack();

	//飛ぶ斬撃の初期化
	void AttackBehaviorDoubleSlashInitialize();
	//二つの斬撃を飛ばす
	void DoubleSlash();

private:
	//攻撃関係の変数群

	/*三連切り*/
	//三連切りの移動座標用
	std::array<Vector3, 3> posContainer_;

	//間の待ちの時間
	uint32_t attackDistance_;

	//待ちに入るための
	bool isAttackEnd_ = false;

	//移動のコンテナがすべて埋まったかどうか
	bool isMaxContext_;

	//現在の攻撃回数
	uint32_t attackCount_;

	const uint32_t distanceTime_ = 20;

	/*突進攻撃*/
	//突進の向き決めの時間
	uint32_t directionTime_;

	//突進への移行時間
	uint32_t attackTransitionTime_;

	/*回転突進攻撃*/

	//攻撃の飛距離
	float attackLength_ = 20.0f;
	//攻撃の目標地点
	Vector3 attackSetPos_{};

	Vector3 attackBasePos_{};

	float easeT_;

	float attackRotate_;

	Ease ease_;


	/*飛ぶ斬撃みたいなの*/

	std::list<std::unique_ptr< EnemyBullet>> bullets_;

	float bulletSpeed_;

	Vector3 slashAngle_{};

private:
	Vector4 enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };

	//ディゾルブ関係
	float threshold_ = 0.0f;
	
	//シリアルナンバー
	uint32_t serialNumber_ = 0;
	//次のシリアルナンバー
	static uint32_t nextSerialNumber_;

	//敵全体での撃破数
	static uint32_t enemyDestroyingNumber_;
};

