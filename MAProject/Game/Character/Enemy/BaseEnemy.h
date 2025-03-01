#pragma once
#include"Transform.h"
#include"TextureManager.h"
#include"Matrix.h"
#include"Object3D/Object3D.h"
#include"ViewProjection.h"
#include"Shape/OBB.h"
#include"Adjustment_Item.h"
#include"Quaternion.h"
#include"HitRecord.h"
#include<optional>
#include <algorithm>
#include"Ease/Ease.h"
#include"Sprite.h"
#include "ParticleBase.h"
class BaseEnemy{
public:
	//初期化
	virtual void Initialize(const Vector3& position) = 0;
	//更新処理
	virtual void Update() = 0;
	//描画
	virtual void Draw(const ViewProjection& viewProjection) = 0;
	//テクスチャの描画
	virtual void TexDraw(const Matrix4x4& viewProjection) = 0;
	//パーティクルの描画
	virtual void ParticleDraw(const ViewProjection& viewProjection, const Vector3& color) = 0;

	//当たりの記憶に追加
	void AddRecord(uint32_t number) { hitRecord_.AddRecord(number); }
	//当たりの記憶をチェックする
	bool RecordCheck(uint32_t number) { return hitRecord_.RecordCheck(number); }

protected:
	//体力関連
	int32_t enemyLifeMax_ = 0;
	int enemyLife_ = 0;
	//当たりを記録する機構
	HitRecord hitRecord_;

	//自機の移動
	Vector3 move_{};
	//姿勢ベクトル
	Vector3 postureVec_{};
	Vector3 frontVec_{};

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
	int lengthJudgmentFar_ = 90;
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
};

