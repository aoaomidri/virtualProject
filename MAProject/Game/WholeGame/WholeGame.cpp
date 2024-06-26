#include "WholeGame.h"

void WholeGame::Initialize(){
	MAFramework::Initialize();

	imguiManager_ = std::make_unique<ImGuiManager>();
	imguiManager_->Initialize();

	adjustment_item = Adjustment_Item::GetInstance();
	//グローバル変数の読み込み
	adjustment_item->LoadFiles();
	textureManager_ = TextureManager::GetInstance();

	directionalLight_ = DirectionalLight::GetInstance();
	directionalLight_->Initialize();

	directionalData_.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalData_.direction = { 0.0f,-1.0f,0.0f };
	directionalData_.intensity = { 1.0f };

	levelLoader_ = LevelLoader::GetInstance();
	levelLoader_->LoadLevelData();

	gameScene_ = new GameScene();
	gameScene_->Initialize();


	vignettingData_ = {
		.scale = 16.0f,
		.pow = 0.8f,
	};
	
}

void WholeGame::Finalize(){	
	imguiManager_->Finalize();

	gameScene_->AudioDataUnLoad();

	gameScene_->Finalize();

	delete gameScene_;
	
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
	
	dxCom_->PreDrawCopy();
	dxCom_->PreDrawSwapChain();
	textureManager_->DrawCopy();
	//2D描画
	gameScene_->Draw2D();
	imguiManager_->Draw();
	dxCom_->PostDraw();
}

void WholeGame::DrawImgui(){
#ifdef _DEBUG


	ImGui::Begin("ポストエフェクト");
	if (ImGui::Button("None")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::None);
	}
	if (ImGui::Button("Gray")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::Gray);
	}
	if (ImGui::Button("Sepia")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::Sepia);
	}
	if (ImGui::Button("Inverse")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::Inverse);
	}
	if (ImGui::Button("OutLine")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::OutLine);
	}
	if (ImGui::Button("RadialBlur")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::RadialBlur);
	}
	if (ImGui::Button("Smoothing3x3")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::Smoothing3x3);
	}
	if (ImGui::Button("Smoothing5x5")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::Smoothing5x5);
	}
	if (ImGui::Button("Smoothing9x9")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::Smoothing9x9);
	}
	if (ImGui::Button("NormalVignetting")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::NormalVignetting);
	}
	if (ImGui::Button("GrayVignetting")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::GrayVignetting);
	}
	if (ImGui::Button("SepiaVignetting")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::SepiaVignetting);
	}
	if (ImGui::Button("VignettingGrayScale")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::VignettingGrayScale);
	}
	if (ImGui::Button("VignettingSepiaScale")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::VignettingSepiaScale);
	}
	ImGui::End();
	ImGui::Begin("FPS");
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Vignettingの情報");
	ImGui::DragFloat("Scale", &vignettingData_.scale, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("Pow", &vignettingData_.pow, 0.01f, 0.0f, 5.0f);
	ImGui::End();
	postEffect_->SetVignettingData(vignettingData_);

	ImGui::Begin("DirectionalLightの情報");
	ImGui::ColorEdit4("ライトの色", &directionalData_.color.x);
	ImGui::DragFloat3("ライトの向き", &directionalData_.direction.x, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat("ライトの輝き", &directionalData_.intensity, 0.01f, 0.0f, 1.0f);
	ImGui::End();

#endif // _DEBUG

	directionalLight_->SetLightData(directionalData_);

}
