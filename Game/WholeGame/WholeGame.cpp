#include "WholeGame.h"

void WholeGame::Initialize(){
	MAFramework::Initialize();

	imguiManager_ = std::make_unique<ImGuiManager>();
	imguiManager_->Initialize();

	adjustment_item = Adjustment_Item::GetInstance();
	//グローバル変数の読み込み
	adjustment_item->LoadFiles();	

	gameScene_ = std::make_unique<GameScene>();
	gameScene_->Initialize();
}

void WholeGame::Finalize(){	
	imguiManager_->Finalize();

	gameScene_->AudioDataUnLoad();

	gameScene_->Finalize();
	
	MAFramework::Finalize();
}

void WholeGame::Update(){
	MAFramework::Update();

	imguiManager_->Begin();
	//ゲームの処理	

	//adjustment_item->Update();

	gameScene_->Update();

	imguiManager_->End();

	if (input_->Trigerkey(DIK_ESCAPE)) {
		endRequst_ = true;
	}
#ifdef _DEBUG
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_BACK)) {
		endRequst_ = true;
	}

#endif // DEBUG_
}

void WholeGame::Draw(){
	dxCom_->PreDraw();
	//3D描画
	gameScene_->Draw3D();
	gameScene_->DrawParticle();
	//2D描画
	gameScene_->Draw2D();
	imguiManager_->Draw();
	dxCom_->PostDraw();
}
