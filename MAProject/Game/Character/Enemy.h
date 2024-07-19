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
#include<optional>


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
	//Imgui描画
	void DrawImgui();

	//床と当たった時のonCollision
	void onFlootCollision(OBB obb);

	//リスポーン
	void Respawn(const Vector3& position);

	void OnCollision();

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

private:
	//クラス内関数
	void ParticleMove();

private:
	static const int particleNum_ = 20;

	//自機のモデル
	Object3D* bodyObj_;
	std::unique_ptr<Object3D> partsObj_;
	std::unique_ptr<Object3D>particleObj_[particleNum_];

	//自機のモデル
	//Object3D* boxObj_;
	
	std::unique_ptr<Object3D> collisionObj_;

	//プレイヤーの座標
	// 追従対象
	const EulerTransform* target_ = nullptr;

	//自機のSRT
	EulerTransform transform_{};
	EulerTransform partsTransform_{};
	EulerTransform particleTransform_[particleNum_]{};
	EulerTransform collisionTransform_{};

	//プレイヤーのマトリックス
	Matrix4x4 matrix_{};

	Matrix4x4 scaleMatrix_{};
	Matrix4x4 rotateMatrix_{};
	Matrix4x4 transformMatrix_{};

	Matrix4x4 collisionMatrix_{};

	Matrix4x4 partsMatrix_{};
	Matrix4x4 particleMatrix_[particleNum_]{};

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

	int dashTimer_ = 0;

	bool isDead_ = false;

	bool isNoLife_ = false;

	int32_t enemyLifeMax_ = 10;

	int enemyLife_ = 10;

	Vector3 particleVec_[particleNum_];

	bool isParticle_ = false;

	float particleSpeed_;

	

private:
	/*振る舞い系*/
	enum class Behavior {
		kAttack,		//攻撃
		kRoot,			//様子見中
		kBack,			//後ろにジャンプ
		kDash,			//ダッシュ
		kRun,			//走り
		kFree,			//遊びの時間
		kDead			//やられた
	};

	enum class AttackBehavior {
		kTriple,		//三連撃
		kCharge,		//近づいて溜めて一撃(X字で剣を振る)
		kBeam,			//ビーム攻撃
		kRotateAttack,	//回転して追尾しながらの攻撃
		kChargeStrong,	//溜めて剣を大きくして攻撃(縦振り)
		kXAttack,		//その場でX字を書いて攻撃(斬撃は飛ぶ)
	};

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

private:
	//行動全体を制御する
	void MotionUpdate();

	//攻撃行動初期化
	void BehaviorAttackInitialize();
	
	//攻撃行動初期化
	void BehaviorRootInitialize();
	//攻撃行動初期化
	void BehaviorDeadInitialize();
	//形態ごとにとる行動
	void AttackMotion();

	//様子見状態の行動
	void RootMotion();
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

	//死んだときのモーション
	void DeadMotion();
	

private:
	//ディゾルブ関係
	float threshold_ = 0.0f;
	
	//シリアルナンバー
	uint32_t serialNumber_ = 0;
	//次のシリアルナンバー
	static uint32_t nextSerialNumber_;
};

