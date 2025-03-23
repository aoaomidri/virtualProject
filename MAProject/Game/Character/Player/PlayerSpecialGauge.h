#pragma once
#include"Vector4.h"
#include"Sprite.h"
#include"Adjustment_Item.h"
#include"ImGuiManager.h"
/*自機キャラの必殺ゲージ*/

class PlayerSpecialGauge{
public:
	//外部ファイルからの読み込み
	void ApplyGlobalVariables();
	//外部ファイルに掃き出し
	void ExportGlobalVariables();

	//初期化処理
	void Initialize();
	//テクスチャの初期化
	void TextureInitialize();
	//更新処理
	void Update();
	//描画処理
	void Draw();

	void DrawImgui();

private:
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	//外部ファイルとやり取りする
	Adjustment_Item* adjustment_item_ = nullptr;
	const char* groupName_ = "PLSpecialGauge";

	//ゲージの最大値
	float gaugeMax_ = 0.0f;
	//現在のゲージ
	float nowGauge_ = 0.0f;
	//攻撃ヒット時の増加量
	float hitAddGauge_ = 0.0f;
	//敵を倒した時のゲージ増加量
	float knockDownAddGauge_ = 0.0f;
	//必殺技使用時の消費量
	float specialAttackConsumption_ = 0.0f;

	//不透明時の色
	Vector4 opaqueColor_ = { 0.0f,0.0f,0.0f,1.0f };
	//半透明時の色
	Vector4 translucentColor_ = { 0.0f,0.0f,0.0f,0.5f };
	//ゲージの外枠のテクスチャ
	std::unique_ptr<Sprite> gaugeFrameTex_;
	//ゲージの中身のテクスチャ
	std::unique_ptr<Sprite> gaugeTex_;

};

