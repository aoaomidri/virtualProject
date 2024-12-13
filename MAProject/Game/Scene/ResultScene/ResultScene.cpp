#include "ResultScene.h"
#include <GameTime.h>

void ResultScene::TextureLoad(){
	
	
}

void ResultScene::SoundLoad(){
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

	for (size_t i = 0; i < timerTexs_.size(); i++) {
		timerTexs_[i] = std::make_unique<Sprite>();
		textureHandle = textureManager_->Load("resources/texture/number/number.png");
		timerTexs_[i]->Initialize(textureHandle);
	}

	clearSprite_->position_ = { 640.0f,175.0f };
	clearSprite_->scale_.x = 850.0f;
	clearSprite_->scale_.y = 150.0f;
	clearSprite_->anchorPoint_ = { 0.5f,0.5f };

	pressSprite_->position_ = { 640.0f,600.0f };
	pressSprite_->scale_.x = 600.0f;
	pressSprite_->scale_.y = 136.0f;
	pressSprite_->anchorPoint_ = { 0.5f,0.5f };

	for (size_t i = 0; i < timerTexs_.size(); i++) {
		timerTexs_[i]->scale_ = { 96.0f,96.0f };
		timerTexs_[i]->anchorPoint_ = { 0.5f,0.5f };
		timerTexs_[i]->uvTransform_.scale.x = 0.1f;
		timerTexs_[i]->position_ = { 500.0f,400.0f,0.0f };
	}

	timerTexs_[1]->position_.x = timerTexs_[0]->position_.x + 80.0f;
	timerTexs_[2]->position_.x = timerTexs_[0]->position_.x + 200.0f;
	timerTexs_[3]->position_.x = timerTexs_[0]->position_.x + 280.0f;

	timerTexs_[3]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetSecondsOnes());
	timerTexs_[2]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetSecondsTens());
	timerTexs_[1]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetMinutes());
	timerTexs_[0]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetMinutesTens());
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

	SpriteInitialize();
	ObjectInitialize();

	floorManager_ = std::make_unique<FloorManager>();
	floorManager_->Initialize();

	firstFloor_ = LevelLoader::GetInstance()->GetLevelObjectTransform("Cube");

	floorManager_->AddFloor(firstFloor_);

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetPosition(kFirstCameraPos_);

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	followCamera_->SetLockOn(lockOn_.get());
}

void ResultScene::Finalize(){
}

void ResultScene::Update(){
	followCamera_->Update();
	followCamera_->SetIsMove(false);
	postEffect_->SetMatProjectionInverse(followCamera_->GetProjectionInverse());
	//ボタンを押したらタイトルに
	if (input_->GetPadButtonTriger(Input::GamePad::A) || input_->GetPadButtonTriger(Input::GamePad::B)) {
		SceneManager::GetInstance()->ChangeScene(SceneName::Title);
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

	for (size_t i = 0; i < timerTexs_.size(); i++) {
		timerTexs_[i]->Draw();
	}
	

	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw2D();
}
