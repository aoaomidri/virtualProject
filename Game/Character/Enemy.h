#pragma once
#include"../../Engine/3D/Transform.h"
#include"../../Engine/2D/TextureManager.h"
#include"../../math/Matrix.h"
#include"../../Engine/3D/Object3D/Object3D.h"
#include"../../Engine/3D/ViewProjection.h"
#include"../../Engine/3D/Shape/OBB.h"
#include"../Item/Adjustment_Item.h"
#include"../../math/Quaternion.h"
//#include"../../Engine/3D/Object3D/Particle.h"
#include<optional>


class Enemy {
public:
	//調整項目
	void ApplyGlobalVariables();
	//初期化
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const Vector3& position);
	//更新処理
	void Update();
	//描画
	void Draw(TextureManager* textureManager, const ViewProjection& viewProjection);
	//Imgui描画
	void DrawImgui();

	//床と当たった時のonCollision
	void onFlootCollision(OBB obb);

	//リスポーン
	void Respawn(const Vector3& position);

	void OnCollision();

public:
	//Getter
	const Transform& GetTransform() const { return transform_; }

	const Vector3 GetCenterPos()const;

	const Matrix4x4& GetRotateMatrix()const { return rotateMatrix_; };

	const OBB& GetOBB()const { return OBB_; }

	bool GetIsDead() { return isDead_; }

	//Setter

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	void SetIsDead(bool isDead) { isDead_ = isDead; }

private:
	//クラス内関数
	void ParticleMove();

private:
	static const int particleNum_ = 20;

	//自機のモデル
	std::unique_ptr<Object3D> model_;
	std::unique_ptr<Object3D> partsModel_;
	//std::unique_ptr<Particle>particleModel_[particleNum_];
	//std::unique_ptr<Object3D> collisionModel_;


	//自機のSRT
	Transform transform_{};
	Transform partsTransform_{};
	Transform particleTransform_[particleNum_]{};

	//プレイヤーのマトリックス
	Matrix4x4 matrix_{};

	Matrix4x4 scaleMatrix_{};
	Matrix4x4 rotateMatrix_{};
	Matrix4x4 transformMatrix_{};

	Matrix4x4 partsMatrix_{};
	Matrix4x4 particleMatrix_[particleNum_]{};

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

	Vector3 worldPos = {};

	/// <summary>
	///倍率
	/// </summary>
	float magnification = 1.0f;
	//移動スピード
	const float moveSpeed_ = 0.02f;

	bool isDead_ = false;

	const int invincibleTimeBase_ = 35;

	int invincibleTime_ = 0;

	int enemyLife_ = 3;

	Vector3 particleVec_[particleNum_];

	bool isParticle_ = false;

	float particleSpeed_;
};

