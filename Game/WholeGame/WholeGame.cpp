#include "WholeGame.h"

void WholeGame::Initialize(){
	MAFramework::Initialize();

	imguiManager_ = std::make_unique<ImGuiManager>();
	imguiManager_->Initialize();

	adjustment_item = Adjustment_Item::GetInstance();
	//グローバル変数の読み込み
	adjustment_item->LoadFiles();	

	textureManager_ = TextureManager::GetInstance();

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

	adjustment_item->Update();

	gameScene_->Update();
	DrawImgui();

	imguiManager_->End();

	if (input_->Trigerkey(DIK_ESCAPE)) {
		endRequst_ = true;
	}
#ifdef _DEBUG
	if (input_->GetPadButtonTriger(Input::GamePad::BACK)) {
		endRequst_ = true;
	}

#endif // DEBUG_
}

void WholeGame::Draw(){
	dxCom_->PreDrawRenderTexture();
	//3D描画
	gameScene_->DrawSkin3D();
	gameScene_->Draw3D();
	gameScene_->DrawParticle();
	//2D描画
	gameScene_->Draw2D();
	
	dxCom_->PreDrawCopy();
	dxCom_->PreDrawSwapChain();
	textureManager_->DrawCopy();
	imguiManager_->Draw();
	dxCom_->PostDraw();
}

void WholeGame::DrawImgui(){
	ImGui::Begin("ポストエフェクト");
	if (ImGui::Button("None")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::None);
	}
	if (ImGui::Button("Gray")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::Gray);
	}
	if (ImGui::Button("Sepia")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::Sepia);
	}
	if (ImGui::Button("Inverse")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::Inverse);
	}
	if (ImGui::Button("Smoothing3x3")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::Smoothing3x3);
	}
	if (ImGui::Button("Smoothing5x5")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::Smoothing5x5);
	}
	if (ImGui::Button("Smoothing9x9")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::Smoothing9x9);
	}
	if (ImGui::Button("NormalVignetting")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::NormalVignetting);
	}
	if (ImGui::Button("GrayVignetting")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::GrayVignetting);
	}
	if (ImGui::Button("SepiaVignetting")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::SepiaVignetting);
	}
	if (ImGui::Button("VignettingGrayScale")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::VignettingGrayScale);
	}
	if (ImGui::Button("VignettingSepiaScale")) {
		textureManager_->SetPostEffect(TextureManager::PostEffect::VignettingSepiaScale);
	}
	ImGui::End();
	ImGui::Begin("FPS");
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();
}
