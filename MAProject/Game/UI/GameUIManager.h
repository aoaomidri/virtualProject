#pragma once
#include"TextureManager.h"
#include"Sprite.h"
#include"Object3D.h"
#include"Matrix.h"
#include"OBB.h"
#include"Quaternion.h"
#include"FollowCamera.h"
#include"Player.h"

class GameUIManager{
public:
	//初期化
	void Initialize();
	//更新処理
	void Update();
	//描画処理
	void Draw();

private:
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	std::unique_ptr<Sprite> weakComboTex_;
	std::unique_ptr<Sprite> actionTextSprite_;
	std::unique_ptr<Sprite> attackSprite_;

};

