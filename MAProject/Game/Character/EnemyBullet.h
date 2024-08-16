#pragma once
#include"Object3D/Object3D.h"
#include"Transform.h"
#include"OBB.h"

class EnemyBullet{
public:
	void Initialize(const Vector3& position, const Vector3& velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

	Vector3 GetWorldPosition();

	// 衝突したら呼び出す関数
	void OnCollision();

	float radius = 1.0f;
private:
    std::unique_ptr<Object3D> obj_;

    OBB obb_;

	float size_ = 0.8f;

	Vector3 scale_ = { size_, size_, size_ };

	Vector3 velocity_;
	// 寿命
	static const int32_t kLifeTime_ = 180;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime_;
	// デスフラグ
	bool isDead_ = false;
};

