#pragma once
#include"Transform.h"
#include"TextureManager.h"
#include"math/Matrix.h"
#include"Object3D.h"
#include"3D/ViewProjection.h"
#include"OBB.h"
#include"Adjustment_Item.h"

class Particle{
public:
	~Particle();

	//初期化
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//更新処理
	void Update();
	//描画
	void Draw(TextureManager* textureManager, const ViewProjection& viewProjection);
public:
	Object3D* model_ = nullptr;

	Transform transform_{};

	Matrix4x4 matrix_{};

	Vector3 moveSpeed_ = { 0.02f,0.0f,0.0f };

	bool isMove_;

	bool isDelete_;

};

