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
	//imguiの表示
	void DrawImgui();
	//ヒット時ゲージを規定値増加させる
	void AddSPGauge() {
		nowGauge_ += hitAddGauge_;
	}

public:
	//ゲッター
	const bool GetIsUse() const { return isUse_; }

private:
	//ゲージ自体の更新処理
	void GaugeUpdate();

private:
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;

	//外部ファイルとやり取りする
	Adjustment_Item* adjustment_item_ = nullptr;
	const char* groupName_ = "PLSpecialGauge";

	//ゲージの最大
	float gaugeMax_ = 0.0f;
	//現在のゲージ
	float nowGauge_ = 0.0f;
	//攻撃ヒット時の増加量
	float hitAddGauge_ = 0.0f;
	//必殺技使用時の消費量
	float specialAttackConsumption_ = 0.0f;
	//ゲージの最大の長さ
	float gaugelengthMax_ = 0.0f;
	//現在のゲージの長さ
	float nowGaugeLength_ = 0.0f;
	//発動可能時のボタンUIのα値
	float buttonAlpha_ = 0.8f;
	//必殺が発動できるかどうか
	bool isUse_ = false;
	//xが最小,yが最大
	const Vector2 kGaugeMinMax_ = { -32.0f,252.0f };
	//不透明時の色
	Vector4 opaqueColor_ = { 1.0f,1.0f,0.0f,1.0f };
	//半透明時の色
	Vector4 translucentColor_ = { 1.0f,1.0f,0.0f,0.2f };
	//ゲージの外枠のテクスチャ
	std::unique_ptr<Sprite> gaugeFrameTex_;
	//ゲージの中身のテクスチャ
	std::unique_ptr<Sprite> gaugeTex_;
	//発動ボタンのテクスチャ
	std::unique_ptr<Sprite> buttonTex_;
};

