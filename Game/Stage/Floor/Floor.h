#pragma once
#include"../../../Engine/3D/Transform.h"
#include"../../../Engine/2D/TextureManager.h"
#include"../../../math/Matrix.h"
#include"../../../Engine/3D/Object3D/Object3D.h"
#include"../../../Engine/3D/ViewProjection.h"
#include"../../../Engine/3D/Shape/OBB.h"
#include"../../Item/Adjustment_Item.h"

class Floor{
public:
	//初期化
	void Initialize(Transform transform);
	//更新処理
	void Update();
	//描画
	void Draw(Object3D* object, Model* model, TextureManager* textureManager, const ViewProjection& viewProjection);
	//Imgui描画
	void DrawImgui();

public:
	//Getter
	const Transform& GetTransform() const { return floorTransform_; }

	bool GetIsMove() { return isMove_; }

	bool GetIsDelete() { return isDelete_; }

	const OBB& GetOBB()const { return floorOBB_; }

private:
	Object3D* floorModel_ = nullptr;

	Transform floorTransform_{};

	OBB floorOBB_;

	Matrix4x4 floorMatrix_{};

	Vector3 moveSpeed_ = { 0.02f,0.0f,0.0f };

	float moveMax = 0.0f;

	float Magnification = 1.0f;

	bool isMove_;

	bool isDelete_;
};

