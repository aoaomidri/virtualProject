#include "GameScene.h"
#include"Matrix.h"
#include <cassert>
void GameScene::TextureLoad() {
	textureManager_->Load("resources/texture/uvChecker.png");//0
	textureManager_->Load("resources/texture/rock.png");
	textureManager_->Load("resources/texture/Floor.png");
	textureManager_->Load("resources/texture/Road.png");
	textureManager_->Load("resources/texture/Sky.png");
	textureManager_->Load("resources/Model/Enemy/EnemyTex.png");
	textureManager_->Load("resources/Model/EnemyParts/EnemyParts.png");
	textureManager_->Load("resources/Model/Weapon/Sword.png");
	textureManager_->Load("resources/texture/Magic.png");
	textureManager_->Load("resources/texture/Black.png");
	textureManager_->Load("resources/texture/circle.png");//10
	textureManager_->Load("resources/texture/monsterBall.png");
	textureManager_->Load("resources/texture/title.png");
	textureManager_->Load("resources/texture/pressA.png");
	textureManager_->Load("resources/texture/Clear.png");
	textureManager_->Load("resources/texture/Whitex64.png");//15
	textureManager_->Load("resources/Model/skyDome/skyDome.png");
	textureManager_->Load("resources/texture/STAttack.png");
	textureManager_->Load("resources/texture/dash.png");
	textureManager_->Load("resources/texture/RB.png");
	textureManager_->Load("resources/texture/actionText.png");//20
	textureManager_->Load("resources/texture/LockOn.png");//21
	textureManager_->Load("resources/texture/Whitex64.png");
	textureManager_->Load("resources/Model/terrain/grass.png");
}

void GameScene::SoundLoad(){
	/*titleBGM = audio_->SoundLoadWave("honobono.wav");
	gameBGM = audio_->SoundLoadWave("Game3.wav");*/
	titleBGM = audio_->SoundLoadWave("honobono.wav");
}

void GameScene::SpriteInitialize(){
	titleSprite_ = std::make_unique<Sprite>();
	titleSprite_->Initialize(12);

	pressSprite_ = std::make_unique<Sprite>();
	pressSprite_->Initialize(13);

	clearSprite_ = std::make_unique<Sprite>();
	clearSprite_->Initialize(14);

	fadeSprite_ = std::make_unique<Sprite>();
	fadeSprite_->Initialize(15);

	/*actionTextSprite_ = std::make_unique<Sprite>(textureManager_.get());
	actionTextSprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), 20);

	attackSprite_ = std::make_unique<Sprite>(textureManager_.get());
	attackSprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), 17);*/
}

void GameScene::ObjectInitialize(){
	directionalLight_.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight_.direction = { 0.0f,1.0f,0.0f };
	directionalLight_.intensity = 1.0f;

	pointLight_.color = { 1.0f,1.0f,1.0f,1.0f };
	pointLight_.position = { 0.0f,10.0f,0.0f };
	pointLight_.intensity = 1.0f;
	pointLight_.radius = 50.0f;
	pointLight_.decay = 1.0f;

	TestModel_ = Model::LoadObjFile("sphere");
	TestGroundModel_ = Model::LoadObjFile("terrain");
	skyDomeModel_ = Model::LoadObjFile("skyDome");

	TestObj_ = std::make_unique<Object3D>();
	TestObj_->Initialize(TestModel_.get());
	TestGroundObj_ = std::make_unique<Object3D>();
	TestGroundObj_->Initialize(TestGroundModel_.get());
	pointLightObj_ = std::make_unique<Object3D>();
	pointLightObj_->Initialize(skyDomeModel_.get());

	skyDomeObj_ = std::make_unique<Object3D>();
	skyDomeObj_->Initialize(skyDomeModel_.get());

	TestObj_->SetDirectionalLight(&directionalLight_);
	TestGroundObj_->SetDirectionalLight(&directionalLight_);
	pointLightObj_->SetDirectionalLight(&directionalLight_);
	skyDomeObj_->SetDirectionalLight(&directionalLight_);

	TestObj_->SetPointLight(&pointLight_);
	TestGroundObj_->SetPointLight(&pointLight_);
	pointLightObj_->SetPointLight(&pointLight_);
	skyDomeObj_->SetPointLight(&pointLight_);

	TestObj_->shininess_ = 10.0f;
	TestGroundObj_->shininess_ = 50.0f;

	testTransform_.scale = { 1.0f,1.0f,1.0f };
	testTransform_.translate.y = 2.0f;
	testGroundTransform_.scale = { 1.0f,1.0f,1.0f };

	pointLightTransform_.scale = { 0.005f,0.005f,0.005f };
}

void GameScene::Initialize(){
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	TextureLoad();
	SoundLoad();
	//audio_->SoundPlayWave(titleBGM, 0.5f);
	///audio_->SoundPlayWave(gameBGM, 0.5f);
	


	audio_->SoundPlayWave(titleBGM, 0.2f);

	SpriteInitialize();
	ObjectInitialize();


	//textureManager_->MakeInstancingShaderResourceView(player_->GetParticle()->GetInstancingResource());

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	//自キャラのワールドトランスフォームを追従カメラにセット
	
	titleSprite_->position_ = { 640.0f,175.0f };
	titleSprite_->scale_.x = 850.0f;
	titleSprite_->scale_.y = 150.0f;
	titleSprite_->anchorPoint_ = { 0.5f,0.5f };
	titleSprite_->color_ = { 0.0f,0.0f,0.0f,1.0f };

	pressSprite_->position_ = { 640.0f,500.0f };
	pressSprite_->scale_.x = 600.0f;
	pressSprite_->scale_.y = 136.0f;
	pressSprite_->anchorPoint_ = { 0.5f,0.5f };
	pressSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	/*actionTextSprite_->position_ = { 1072.0f,500.0f };
	actionTextSprite_->anchorPoint_ = { 0.5f,0.5f };
	actionTextSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	attackSprite_->position_ = { 1072.0f,650.0f };
	attackSprite_->anchorPoint_ = { 0.5f,0.5f };
	attackSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };*/

	clearSprite_->position_ = { 640.0f,175.0f };
	clearSprite_->scale_.x = 850.0f;
	clearSprite_->scale_.y = 150.0f;
	clearSprite_->anchorPoint_ = { 0.5f,0.5f };

	fadeSprite_->position_ = { 640.0f,360.0f };
	fadeSprite_->scale_.x = 1280.0f;
	fadeSprite_->scale_.y = 720.0f;
	fadeSprite_->color_ = { 0.0f,0.0f,0.0f,fadeAlpha_ };
	fadeSprite_->anchorPoint_ = { 0.5f,0.5f };

	sceneNum_ = SceneName::TITLE;

	followCamera_->SetTarget(&testTransform_);

	
	//lockOn_ = std::make_unique<LockOn>();
	//lockOn_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), textureManager_.get());

	//followCamera_->SetLockOn(lockOn_.get());
	//player_->SetLockOn(lockOn_.get());
	////audio_->SoundPlayWave(soundData1);
}

void GameScene::Update(){
	DrawImgui();
	testMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(testTransform_);
	testGroundMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(testGroundTransform_);

	pointLightMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(pointLightTransform_.scale, pointLightTransform_.rotate, pointLight_.position);;

	switch (sceneNum_){
	case SceneName::TITLE:
		followCamera_->SetIsMove(true);
		followCamera_->Update();
		
		

		/*titleSprite_->Update();
		pressSprite_->Update();
		fadeSprite_->Update();*/
		break;
	case SceneName::GAME:
		/*if (input_->Trigerkey(DIK_1)){
			sceneNum_ = SceneName::CLEAR;
		}*/
		followCamera_->SetIsMove(true);
		
		fadeAlpha_ -= 0.01f;
		if (fadeAlpha_<=0.0f){

		}
		

		
		//AllCollision();
		////particle_->Update(player_->GetTransform(), followCamera_->GetViewProjection());

		//floorManager_->Update();
		break;
	case SceneName::CLEAR:
		followCamera_->SetIsMove(false);
		//audio_->PauseWave(gameBGM);
		
		if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A)) {
			sceneNum_ = SceneName::TITLE;
		}
		
		fadeAlpha_ = 0.0f;
		isFade_ = false;

		break;
	default:
		assert(0);
	}
	fadeSprite_->color_.w = fadeAlpha_;
	skyDomeTrnasform_.rotate.y += 0.001f;
	skyDomeMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(skyDomeTrnasform_);

	if (fadeAlpha_>=1.0f){
		fadeAlpha_ = 1.0f;
	}
	else if (fadeAlpha_ <= 0.0f) {
		fadeAlpha_ = 0.0f;
	}
}

void GameScene::AudioDataUnLoad(){
	
	
}

void GameScene::DrawParticle(){
	textureManager_->PreDrawParticle();

	switch (sceneNum_) {
	case SceneName::TITLE:
		break;
	case SceneName::GAME:
		//player_->ParticleDraw(textureManager_.get(), followCamera_->GetViewProjection());
		break;
	case SceneName::CLEAR:

		break;
	default:
		assert(0);
	}

	textureManager_->PostDrawParticle();
}

void GameScene::Draw3D(){
	/*描画前処理*/
	textureManager_->PreDraw3D();
	/*skyDomeObj_->Update(skyDomeMatrix_, followCamera_->GetViewProjection());
	skyDomeObj_->Draw();*/

	/*ここから下に描画処理を書き込む*/
	switch (sceneNum_) {
	case SceneName::TITLE:
		TestObj_->Update(testMatrix_, followCamera_->GetViewProjection());
		TestObj_->Draw();

		TestGroundObj_->Update(testGroundMatrix_, followCamera_->GetViewProjection());
		TestGroundObj_->Draw();

		pointLightObj_->Update(pointLightMatrix_, followCamera_->GetViewProjection());

		pointLightObj_->Draw();
		
		break;
	case SceneName::GAME:
		
		break;
	case SceneName::CLEAR:
		
		break;
	default:
		assert(0);
	}
	
	
	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw3D();
}

void GameScene::Draw2D(){
	/*描画前処理*/
	textureManager_->PreDraw2D();
	///*ここから下に描画処理を書き込む*/
	switch (sceneNum_) {
	case SceneName::TITLE:
		
		break;
	case SceneName::GAME:
		
		break;
	case SceneName::CLEAR:
	
		break;
	default:
		assert(0);
	}
	//testTexture_->Draw(textureManager_->SendGPUDescriptorHandle(0));
	
	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw2D();
}

void GameScene::Finalize(){
	textureManager_->Finalize();
	
}

void GameScene::DrawImgui(){
#ifdef _DEBUG
	switch (sceneNum_) {
	case SceneName::TITLE:
	/*ImGui::Begin("BGM関連");
	if (ImGui::Button("音源の復活")){
		gameBGM = audio_->SoundPlayWave(gameBGM, 0.5f);
	}
	if (ImGui::Button("最初から再生")) {
		audio_->RePlayWave(gameBGM);
	}
	if (ImGui::Button("完全に停止")) {
		audio_->StopWave(gameBGM);
	}
	if (ImGui::Button("一時停止")){
		audio_->PauseWave(gameBGM);
	}
	if (ImGui::Button("停止解除")) {
		audio_->ResumeWave(gameBGM);
	}
	ImGui::End();*/

	ImGui::Begin("テストオブジェ");
	ImGui::DragFloat3("大きさ", &testTransform_.scale.x, 0.1f);
	ImGui::DragFloat3("回転", &testTransform_.rotate.x, 0.1f);
	ImGui::DragFloat3("座標", &testTransform_.translate.x, 0.1f);

	ImGui::End();
	TestObj_->DrawImgui("sphere");
	TestGroundObj_->DrawImgui("Ground");

	followCamera_->DrawImgui();
		break;
	case SceneName::GAME:
		
		break;
	case SceneName::CLEAR:
		break;
	default:
		assert(0);
	}
	
	//ImGui::Begin("スプライト");
	////ImGui::DragFloat2("action : ポジション", &attackSprite_->position_.x, 1.0f);
	////ImGui::DragFloat2("action : 大きさ", &attackSprite_->scale_.x, 1.0f);
	////ImGui::DragFloat4("action : 色", &attackSprite_->color_.x, 0.01f, 0.0f, 1.0f);
	////ImGui::DragFloat2("press : ポジション", &actionTextSprite_->position_.x, 1.0f);
	////ImGui::DragFloat2("press : 大きさ", &actionTextSprite_->scale_.x, 1.0f);
	////ImGui::DragFloat4("press : 色", &actionTextSprite_->color_.x, 0.01f, 0.0f, 1.0f);
	//ImGui::DragFloat2("fade : ポジション", &fadeSprite_->position_.x, 1.0f);
	//ImGui::DragFloat2("fade : 大きさ", &fadeSprite_->scale_.x, 1.0f);
	//ImGui::DragFloat4("fade : 色", &fadeSprite_->color_.x, 0.01f, 0.0f, 1.0f);
	//ImGui::End();

	ImGui::Begin("ライト");
	ImGui::DragFloat4("ライトの色", &directionalLight_.color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("ライトの向き", &directionalLight_.direction.x, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat("ライトの輝き", &directionalLight_.intensity, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("ポイントライトの色", &pointLight_.color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("ポイントライトの位置", &pointLight_.position.x, 0.01f, -100.0f, 100.0f);
	ImGui::DragFloat("ポイントライトの輝き", &pointLight_.intensity, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("ポイントライトの届く距離", &pointLight_.radius, 0.1f, 0.0f, 200.0f);
	ImGui::DragFloat("ポイントライトの減衰率", &pointLight_.decay, 0.01f, 0.0f, 100.0f);
	ImGui::End();

	//particle_->DrawImgui("ステージパーティクル");
	


#endif // _DEBUG	
}
