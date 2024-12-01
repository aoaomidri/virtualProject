#include "WholeGame.h"
#include"TitleScene/TitleScene.h"

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

	sceneFactory_ = std::make_unique<SceneFactory>();

	sceneManager_ = SceneManager::GetInstance();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene(AbstractSceneFactory::SceneName::Result);


	vignettingData_ = {
		.scale = 16.0f,
		.pow = 0.8f,
	};

	postBlend_ = 1.0f;
	
}

void WholeGame::Finalize(){	
	imguiManager_->Finalize();

	sceneManager_->Finalize();	
	
	MAFramework::Finalize();
}

void WholeGame::Update(){
	MAFramework::Update();

	imguiManager_->Begin();
	//ゲームの処理	

	adjustment_item->Update();

	GameTime::Update();

	sceneManager_->Update();
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
	sceneManager_->Draw3D();
	
	dxCom_->PreDrawCopy();
	dxCom_->PreDrawSwapChain();
	textureManager_->DrawCopy();
	//2D描画
	sceneManager_->Draw2D();
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
	if (ImGui::Button("Dissolve")) {
		postEffect_->SetPostEffect(PostEffect::EffectType::Dissolve);
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
	
	ImGui::End();
	ImGui::Begin("FPS");
	ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("ゲーム内の時間");
	ImGui::SliderFloat("時間の速さ", &GameTime::timeScale_, 0.0f, 1.0f, "%.1f");
	if (ImGui::Button("時間を止める")){
		GameTime::StopTime(1.0f);
	}
	if (ImGui::Button("時間を遅くする")) {
		GameTime::SlowDownTime(1.0f, 0.5f);
	}

	ImGui::End();

	ImGui::Begin("ポストエフェクトの情報");

	ImGui::Text("Vignettingの情報");
	ImGui::DragFloat("Scale", &vignettingData_.scale, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("Pow", &vignettingData_.pow, 0.01f, 0.0f, 5.0f);
	postEffect_->SetVignettingData(vignettingData_);

	ImGui::Text("しきい値の設定");
	ImGui::DragFloat("しきい値", &threshold, 0.001f, 0.0f, 1.0f);
	postEffect_->SetThreshold(threshold);

	ImGui::Text("HSVの値");
	ImGui::DragFloat("hue", &hsv_.hue, 0.001f, -1.0f, 1.0f);
	ImGui::DragFloat("saturate", &hsv_.saturation, 0.001f, -1.0f, 1.0f);
	ImGui::DragFloat("value", &hsv_.value, 0.001f, -1.0f, 1.0f);
	postEffect_->SetHSVData(hsv_);

	ImGui::Text("ポストエフェクトの補正値");
	ImGui::SliderFloat("blendFactor", &postBlend_, 0.0f, 1.0f, "%.2f");
	//postEffect_->SetPostBlend(postBlend_);

	ImGui::End();

	

	ImGui::Begin("DirectionalLightの情報");
	ImGui::ColorEdit4("ライトの色", &directionalData_.color.x);
	ImGui::DragFloat3("ライトの向き", &directionalData_.direction.x, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat("ライトの輝き", &directionalData_.intensity, 0.01f, 0.0f, 1.0f);
	ImGui::End();

#endif // _DEBUG

	directionalLight_->SetLightData(directionalData_);

}
