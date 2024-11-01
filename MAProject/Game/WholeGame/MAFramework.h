#pragma once
#include"RandomMaker.h"
#include"ImGuiManager.h"
#include"Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include"GameScene/GameScene.h"
#include"Adjustment_Item.h"
#include"PostEffect.h"
#include"TextureManager.h"
/*ゲーム外のシステムを管理*/
class MAFramework{
public:
	virtual ~MAFramework() = default;
public:
	//初期化
	virtual void Initialize();

	//終了
	virtual void Finalize();

	//毎フレーム更新
	virtual void Update();

	//描画
	virtual void Draw() = 0;

	//システムの終了をチェック
	virtual bool IsEndRequst() { return endRequst_; }

	//実行関数
	void Run();

	bool endRequst_ = false;
public:
	//ウィンドウ関連
	WinApp* window_ = nullptr;
	//キー入力関連
	Input* input_ = nullptr;
	//DirectX12関連
	DirectXCommon* dxCom_ = nullptr;	
	//ランダム生成器
	RandomMaker* randomMaker_ = nullptr;
	//音楽関連
	Audio* audio_ = nullptr;
	//テクスチャ関連
	TextureManager* texManager_ = nullptr;
	//ポストエフェクト
	PostEffect* postEffect_ = nullptr;

private:
	MSG msg{};
};

