#pragma once
#include"../../Transform.h"
#include"../../TextureManager.h"
#include"../../math/Matrix.h"
#include"../../Object3D.h"
#include"../../3D/ViewProjection.h"
#include"../../OBB.h"
#include"../../Adjustment_Item.h"
#include"../../math/Quaternion.h"
#include<optional>


class Enemy {
public:
	//調整項目
	void ApplyGlobalVariables();
	//初期化
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//更新処理
	void Update();
	//描画
	void Draw(TextureManager* textureManager, const ViewProjection& viewProjection);
	//Imgui描画
	void DrawImgui();

	//床と当たった時のonCollision
	void onFlootCollision(OBB obb);

	//リスポーン
	void Respawn();

public:
	//Getter
	const Transform& GetTransform() const { return transform_; }

	const Matrix4x4& GetRotateMatrix()const { return rotateMatrix_; };

	const OBB& GetOBB()const { return OBB_; }

	//Setter

	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	void SetIsDead(bool isDead) { isDead_ = isDead; }

private:
	//クラス内関数

private:
	//自機のモデル
	std::unique_ptr<Object3D> model_;
	std::unique_ptr<Object3D> partsModel_;
	std::unique_ptr<Object3D> collisionModel_;


	//自機のSRT
	Transform transform_{};
	Transform partsTransform_{};
	Transform collisionTransform_{};

	//プレイヤーのマトリックス
	Matrix4x4 matrix_{};

	Matrix4x4 scaleMatrix_{};
	Matrix4x4 rotateMatrix_{};
	Matrix4x4 transformMatrix_{};

	Matrix4x4 partsMatrix_{};
	Matrix4x4 collisionMatrix_{};

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

	/// <summary>
	///倍率
	/// </summary>
	float magnification = 1.0f;
	//移動スピード
	const float moveSpeed_ = 0.02f;

	bool isDead_ = false;

};

