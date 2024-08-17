#pragma once
#include"Object3D/Object3D.h"
#include"Transform.h"
#include"OBB.h"

class EnemyBullet{
public:
	void Initialize(const EulerTransform& trans, const Vector3& velocity);

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

	Matrix4x4 transMat_;

	Matrix4x4 rotateYMat_;

	Matrix4x4 rotateZMat_;

	Matrix4x4 rotateZInvMat_;

	Matrix4x4 rotateMat_;

	Matrix4x4 scaleMat_;

	Matrix4x4 numMat_;

	Vector3 velocity_;
	// 寿命
	static const int32_t kLifeTime_ = 300;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime_;
	// デスフラグ
	bool isDead_ = false;
};

