#pragma once

#include"MAFramework.h"
//ゲーム全体を管理するクラス
class WholeGame : public MAFramework{
public:
	//メンバ関数

	//初期化
	void Initialize()override;

	//終了
	void Finalize()override;

	//毎フレーム更新
	void Update()override;

	//描画
	void Draw()override;

	//ゲーム全体にかかわるImgui
	void DrawImgui();

private:	
	//Imgui関連
	std::unique_ptr<ImGuiManager> imguiManager_;	
	//ゲームシーン
	std::unique_ptr<GameScene> gameScene_;
	//グローバル変数関連
	Adjustment_Item* adjustment_item = nullptr;

	TextureManager* textureManager_ = nullptr;
		
};

