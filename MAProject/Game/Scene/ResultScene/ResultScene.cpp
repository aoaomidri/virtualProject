#include "ResultScene.h"

void ResultScene::TextureLoad(){
	textureManager_->Load("resources/texture/uvChecker.png");
	textureManager_->Load("resources/texture/Floor.png");
	textureManager_->Load("resources/texture/Road.png");
	
}

void ResultScene::SoundLoad(){
	resultBGM_ = audio_->LoadAudio("BGM/Game3.mp3");
}

void ResultScene::SpriteInitialize(){
	uint32_t textureHandle = 0;
	//クリア
	clearSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Clear.png");
	clearSprite_->Initialize(textureHandle);

	pressSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Press.png");
	pressSprite_->Initialize(textureHandle);

	clearSprite_->position_ = { 640.0f,175.0f };
	clearSprite_->scale_.x = 850.0f;
	clearSprite_->scale_.y = 150.0f;
	clearSprite_->anchorPoint_ = { 0.5f,0.5f };

	pressSprite_->position_ = { 640.0f,500.0f };
	pressSprite_->scale_.x = 600.0f;
	pressSprite_->scale_.y = 136.0f;
	pressSprite_->anchorPoint_ = { 0.5f,0.5f };
}

void ResultScene::ObjectInitialize(){
	stageObject_->Initialize();
}

void ResultScene::Initialize(){
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	postEffect_ = PostEffect::GetInstance();
	postEffect_->SetPostEffect(PostEffect::EffectType::None);

	TextureLoad();
	SoundLoad();
	audio_->PlayAudio(resultBGM_, 0.1f, true);

	SpriteInitialize();
	ObjectInitialize();

	floorManager_ = std::make_unique<FloorManager>();
	floorManager_->Initialize();

	firstFloor_ = LevelLoader::GetInstance()->GetLevelObjectTransform("Cube");

	floorManager_->AddFloor(firstFloor_, false);

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetPosition(Vector3(0.0f, 20.0f, 0.0f));	

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	followCamera_->SetLockOn(lockOn_.get());
}

void ResultScene::Finalize(){

}

void ResultScene::Update(){

	followCamera_->Update();
	postEffect_->SetMatProjectionInverse(followCamera_->GetProjectionInverse());
	//audio_->PauseWave(gameBGM);
	followCamera_->SetIsMove(false);
	
	if (input_->GetPadButtonTriger(Input::GamePad::A) || input_->GetPadButtonTriger(Input::GamePad::B)) {
		SceneManager::GetInstance()->ChangeScene(SceneName::Title);
		audio_->StopWave(resultBGM_);

	}
	else {
		if (input_->GetPadButtonTriger(Input::GamePad::X) || input_->GetPadButtonTriger(Input::GamePad::Y)) {
			
			audio_->StopWave(resultBGM_);
		}
	}

	floorManager_->Update();
	
}

void ResultScene::AllDraw3D(){
	DrawSkin3D();
	Draw3D();
	DrawParticle();
}

void ResultScene::AllDraw2D(){
	Draw2D();
}

void ResultScene::Debug(){

}

void ResultScene::DrawImgui(){
#ifdef _DEBUG
	ImGui::Begin("テクスチャ");
	ImGui::DragFloat2("クリアテクスチャ", &clearSprite_->position_.x, 1.0f);
	ImGui::DragFloat2("プレステクスチャ", &pressSprite_->position_.x, 1.0f);
	ImGui::End();
#endif // _DEBUG	
}

void ResultScene::DrawParticle(){

}

void ResultScene::DrawSkin3D(){

}

void ResultScene::Draw3D(){
	/*描画前処理*/
	textureManager_->PreDrawMapping3D();


	floorManager_->Draw(followCamera_->GetViewProjection());

	textureManager_->PreDraw3D();
	stageObject_->Draw(followCamera_->GetViewProjection());

	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw3D();

	//textureManager_->PreDrawSkyBox();

	//skyBox_->Update(followCamera_->GetViewProjection());
	//skyBox_->Draw();

	textureManager_->PreDrawWorld2D();
}

void ResultScene::Draw2D(){
	/*描画前処理*/
	textureManager_->PreDraw2D();
	///*ここから下に描画処理を書き込む*/
	clearSprite_->Draw();
	pressSprite_->Draw();
	

	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw2D();
}
