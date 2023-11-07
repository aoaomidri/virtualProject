#pragma once
#include"Transform.h"
#include"TextureManager.h"
#include"math/Matrix.h"
#include"Object3D.h"
#include"Input.h"
#include"OBB.h"
class Player{
public:
	//初期化
	void Initislize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//更新処理
	void Update(Input* input);
	//描画
	void Draw(TextureManager* textureManager);
	//Imgui描画
	void DrawImgui();

public:
	//Getter
	const Vector3& GetTranslate() const { return playerTransform_.translate; }

	const Vector3& GetRotate() const { return playerTransform_.rotate; }

	const Vector3& GetScale() const { return playerTransform_.scale; }

	const OBB& GetOBB()const { return playerOBB_; }

	//Setter

	void SetCameraTransform(const Transform& cameraTransform) { cameraTransform_ = cameraTransform; }

	void SetIsDown(bool isDown) { isDown_ = isDown; }
private:
	//自機のモデル
	std::unique_ptr<Object3D> playerModel_;

	//自機のSRT
	Transform playerTransform_{};

	//プレイヤーのマトリックス
	Matrix4x4 playerMatrix_{};

	//スケールを無視したマトリックス
	Matrix4x4 playerMoveMatrix_{};

	//自機の移動
	Vector3 move_{};

	Transform cameraTransform_{};

	//自機のOBB
	OBB playerOBB_{};

	//移動スピード
	const float moveSpeed_ = 0.1f;

	//落下するかどうか
	bool isDown_ = false;

};

