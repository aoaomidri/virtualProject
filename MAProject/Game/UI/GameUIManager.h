#pragma once
#include"TextureManager.h"
#include"Sprite.h"
#include"Matrix.h"
#include<array>
#include"Quaternion.h"
#include"FollowCamera.h"
#include"Player.h"

class GameUIManager{
public:
	//外部ファイルからの読み込み
	void ApplyGlobalVariables();
	//外部ファイルに掃き出し
	void ExportGlobalVariables();

	//初期化
	void Initialize();
	//テクスチャを呼び出したり等の初期化
	void TextureInitialize();
	//更新処理
	void Update();
	//チュートリアルのUI更新
	void TutorialUpdate();

	//描画処理
	void Draw();

private:
	//外部ファイルとやり取りする
	Adjustment_Item* adjustment_item_ = nullptr;
	const char* groupName_ = "UI";

	static const int comboMax_ = 6;
	int titorialLevel_ = 1;
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	//プレイヤーの状態
	PlayerStateManager* plStateManager_ = nullptr;
	//コンボ描画用
	std::unique_ptr<Sprite> weakComboTex_;
	std::unique_ptr<Sprite> strong2ComboTex_;

	std::unique_ptr<Sprite> actionTextSprite_;
	std::unique_ptr<Sprite> attackSprite_;

	std::array<std::unique_ptr<Sprite>, comboMax_> checkMarkTex_;
	std::array<Vector2, comboMax_> checkPos_;
	float checkScale_;
};
