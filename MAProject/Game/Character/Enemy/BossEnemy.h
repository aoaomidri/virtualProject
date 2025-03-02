#pragma once
#include"BaseEnemy.h"

class BossEnemy: public BaseEnemy{
public:
	//調整項目
	void ApplyGlobalVariables();
	//初期化
	void Initialize(const Vector3& position) override;
	//更新処理
	void Update()override;
	//描画
	void Draw(const ViewProjection& viewProjection)override;
	//テクスチャの描画
	void TexDraw(const Matrix4x4& viewProjection)override;
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

private:
	//オブジェ内部の値をセット
	void ObjStateSet();
	//部品の計算処理
	void PartsCalculation();

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
	std::unique_ptr<Object3D> partsObjRight_;
	std::unique_ptr<Object3D> partsObjLeft_;

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
	EulerTransform partsRightTransform_{};
	EulerTransform partsLeftTransform_{};
	EulerTransform collisionTransform_{};
	//プレイヤーのマトリックス
	Matrix4x4 matrix_{};
	Matrix4x4 scaleMatrix_{};
	Matrix4x4 rotateMatrix_{};
	Matrix4x4 transformMatrix_{};
	Matrix4x4 collisionMatrix_{};
	Matrix4x4 partsMatrix_{};
	Matrix4x4 partsRightMatrix_{};
	Matrix4x4 partsLeftMatrix_{};
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

	float partsRotateRootPos_ = 2.5f;
	float partsRotateAttackPos_ = 6.5f;
	float partsRotateAttackRotateZ_ = 1.57f;

	float partsLRPos_ = 2.5f;
	float partsLRRotateZ_ = 0.0f;
	//パーツ関連
	Vector3 parts_offset_BaseRight_ = { partsLRPos_, 0.0f, 0.0f };
	Vector3 parts_offsetRight_ = { 0.0f, 0.0f, 0.0f };
	//パーツ関連
	Vector3 parts_offset_BaseLeft_ = { -partsLRPos_, 0.0f, 0.0f };
	Vector3 parts_offsetLeft_ = { 0.0f, 0.0f, 0.0f };

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
	float nearPlayer_ = 10.0f + (0.2f * serialNumber_);
	//プレイヤーが遠くにいるときに溜まる
	int farTime_ = 0;
	//遠い条件
	float farPlayer_ = 30.0f;
	//距離に応じた反応の時間制限
	int lengthJudgment_ = 180;
	int lengthJudgmentFar_ = 90;
	int freeTime_ = 0;
	int	freeTimeMax_ = 40;
	const float kTranslateHeight_ = 4.0f;
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
		kRotate,		//その場回転攻撃
		kShot,			//左右のとげを射出する
		kTackle,		//突進攻撃.OK
		kNone,			//何もしない
	};
	AttackBehavior ATBehavior_ = AttackBehavior::kNone;
	std::optional<AttackBehavior> ATBehaviorRequest_ = std::nullopt;
	//攻撃行動初期化
	void BehaviorAttackInitialize();
	//攻撃の予約
	void NextAttack(const AttackBehavior attack);
	//形態ごとにとる行動
	void AttackMotion();
	//突進攻撃初期化
	void AttackBehaviorTackleInitialize();
	//突進攻撃
	void Tackle();
	//回転攻撃初期化
	void AttackBehaviorRotateAttackInitialize();
	//回転攻撃
	void RotateAttack();
	//遠距離攻撃初期化
	void AttackBehaviorShotInitialize();
	//遠距離攻撃
	void PartsShot();
private:
	//攻撃関係の変数群
	//間の待ちの時間
	uint32_t attackDistance_ = 0;

	//待ちに入るための
	bool isAttackEnd_ = false;
	//移動のコンテナがすべて埋まったかどうか
	bool isMaxContext_ = false;

	const uint32_t distanceTime_ = 20;
	/*突進攻撃*/
	//突進の向き決めの時間
	uint32_t directionTimeBase_ = 90;
	//突進の向き決めの時間
	uint32_t directionTime_ = 0;

	//突進攻撃の範囲
	float collisionScale_ = 1.3f;
	float bossScale_ = 1.7f;

	//突進への移行時間
	uint32_t attackTransitionTimeBase_ = 30;
	uint32_t attackTransitionTime_ = 0;
	const int32_t kDashTime_ = 30;
	/*回転突進攻撃*/
	//攻撃の飛距離
	float attackLength_ = 20.0f;
	//回転数
	float rotateNum_ = 1.0f;
	//回転角の最大値
	float rotateAttackMax_ = 31.4f;
	//補完
	float rotateAttackEaseT_ = 0;
	//補完速度
	float rotateSpeed_ = 0.01f;
	//遅くする位置
	float slowPoint_ = 0.8f;
	float slow_ = 0.5f;
	//代入する変数
	float attackRotate_ = 0;
	Ease ease_{};
private:
	bool isAttack_ = false;

	Vector4 enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };

	float colorSpeed_ = 0.02f;
	float colorLimit_ = 0.2f;
	//ディゾルブ関係
	float threshold_ = 0.0f;
	float thresholdSpeed_ = 0.0075f;

	//シリアルナンバー
	uint32_t serialNumber_ = 0;
};

