#pragma once
#include"../../Transform.h"
#include"../../TextureManager.h"
#include"../../math/Matrix.h"
#include"../../Object3D.h"
#include"../../Sprite.h"
#include"../../Input.h"
#include"../../OBB.h"
#include"../../Adjustment_Item.h"
#include<optional>

class Player {
public:
	//初期化
	void Initislize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//更新処理
	void Update(Input* input);
	//描画
	void Draw(TextureManager* textureManager);
	//Imgui描画
	void DrawImgui();

	//キーごとにコマンドを作る
	void MoveRight();
	void MoveLeft();

	//リスポーン
	void Respawn();

public:
	//Getter
	const Transform& GetTransform() const { return playerTransform_; }

	const Vector3& GetTranslate() const { return playerTransform_.translate; }

	const Vector3& GetRotate() const { return playerTransform_.rotate; }

	const Vector3& GetScale() const { return playerTransform_.scale; }

	//Setter
private:
	//クラス内関数

private:
	std::unique_ptr<Sprite> playerSprite_;

	//自機のSRT
	Transform playerTransform_{};

	//プレイヤーのマトリックス
	Matrix4x4 playerMatrix_{};

	//自機の移動
	Vector2 move_{};

	//移動スピード
	const float moveSpeed_ = 10.0f;

};

