#include "TitleScene.h"


void TitleScene::TextureLoad() {
	textureManager_->Load("resources/texture/rock.png");
	textureManager_->SetDissolveTex(textureManager_->Load("resources/texture/PostEffect/noise0.png"));
	textureManager_->Load("resources/texture/uvChecker.png");
	textureManager_->Load("resources/texture/Floor.png");
	textureManager_->Load("resources/texture/Road.png");
	textureManager_->Load("resources/Model/Enemy/EnemyTex.png");
	textureManager_->Load("resources/Model/EnemyParts/EnemyParts.png");
	textureManager_->Load("resources/Model/Weapon/Sword.png");
	textureManager_->Load("resources/texture/Magic.png");
	textureManager_->Load("resources/texture/Black.png");
	textureManager_->Load("resources/texture/circle.png");
	textureManager_->Load("resources/texture/monsterBall.png");
	textureManager_->Load("resources/texture/Text/title.png");
	textureManager_->Load("resources/texture/Text/pressA.png");
	textureManager_->Load("resources/texture/Text/Clear.png");
	textureManager_->Load("resources/texture/Whitex64.png");
	textureManager_->Load("resources/texture/Text/STAttack.png");
	textureManager_->Load("resources/texture/Text/dash.png");
	textureManager_->Load("resources/texture/Text/RB.png");
	textureManager_->Load("resources/texture/Text/actionText.png");
	textureManager_->Load("resources/DDS/rostock_laage_airport_4k.dds");
}

void TitleScene::SoundLoad() {
	titleBGM_ = audio_->LoadAudio("BGM/Game3.mp3");
	gameBGM_ = audio_->LoadAudio("BGM/Result3.mp3");
}

void TitleScene::SpriteInitialize() {
	uint32_t textureHandle = 0;

	titleSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Text/title.png");
	titleSprite_->Initialize(textureHandle);

	startSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Text/start.png");
	startSprite_->Initialize(textureHandle);

	comboSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Text/combo.png");
	comboSprite_->Initialize(textureHandle);

	selectSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/select.png");
	selectSprite_->Initialize(textureHandle);

	controlSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/combo/control.png");
	controlSprite_->Initialize(textureHandle);
	controlSprite_->isDraw_ = false;
	//////ここから修正
	backSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Whitex64.png");
	backSprite_->Initialize(textureHandle);
	backSprite_->isDraw_ = false;

	fadeSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Black.png");
	fadeSprite_->Initialize(textureHandle);

	titleSprite_->position_ = { 640.0f,175.0f };
	titleSprite_->scale_.x = 850.0f;
	titleSprite_->scale_.y = 150.0f;
	titleSprite_->anchorPoint_ = { 0.5f,0.5f };
	titleSprite_->color_ = { 0.529f, 0.808f, 0.922f,1.0f };

	startSprite_->position_ = { 640.0f,390.0f };
	startSprite_->anchorPoint_ = { 0.5f,0.5f };
	startSprite_->color_ = { 0.3f,0.3f,0.3f,1.0f };
	startSprite_->isDraw_ = true;

	selectSprite_->position_ = { 380.0f,390.0f };
	selectSprite_->rotation_.z = -1.57f;
	selectSprite_->anchorPoint_ = { 0.5f,0.5f };
	selectSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };
	selectSprite_->isDraw_ = true;

	comboSprite_->position_ = { 640.0f,550.0f };
	comboSprite_->anchorPoint_ = { 0.5f,0.5f };
	comboSprite_->color_ = { 0.3f,0.3f,0.3f,1.0f };
	comboSprite_->isDraw_ = true;

	controlSprite_->position_ = { 640.0f,360.0f };
	controlSprite_->anchorPoint_ = { 0.5f,0.5f };
	controlSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	backSprite_->position_ = { 640.0f,360.0f };
	backSprite_->anchorPoint_ = { 0.5f,0.5f };
	backSprite_->scale_.x = 1280.0f;
	backSprite_->scale_.y = 720.0f;
	backSprite_->color_ = { 0.0f,0.0f,0.0f,0.85f };

	fadeSprite_->position_ = { 640.0f,360.0f };
	fadeSprite_->scale_.x = 1280.0f;
	fadeSprite_->scale_.y = 720.0f;
	fadeSprite_->color_ = { 0.0f,0.0f,0.0f,fadeAlpha_ };
	fadeSprite_->anchorPoint_ = { 0.5f,0.5f };
}

void TitleScene::ObjectInitialize() {
	skyBox_ = std::make_unique<SkyBox>();
	skyBox_->Initialize("resources/DDS/rostock_laage_airport_4k.dds");
	skyBox_->transform_.scale = { 1000.0f,1000.0f,1000.0f };
	stageObject_->Initialize();
}

void TitleScene::TitleArrowMove(){
	if (fadeAlpha_ <= 0.0f) {
		if ((input_->GetPadLStick().y > 0.8f || input_->GetPadLStick().y < -0.8f) && isStickUpDown_ == false) {
			isStickUpDown_ = true;
			if (isStart_ == false) {
				isStart_ = true;
			}
			else {
				isStart_ = false;
			}
		}
		else if (input_->GetPadLStick().y == 0.0f) {
			isStickUpDown_ = false;
		}
		//上下ボタンでアローの移動
		if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_DPAD_UP) || input_->GetPadButtonTriger(XINPUT_GAMEPAD_DPAD_DOWN)) {
			if (isStart_ == false) {
				isStart_ = true;
			}
			else {
				isStart_ = false;
			}
			controlSprite_->isDraw_ = false;
			backSprite_->isDraw_ = false;
		}
		if (isStart_ == false) {
			selectSprite_->position_.y = selectPosY_.y;
		}
		else {
			selectSprite_->position_.y = selectPosY_.x;
		}
		//説明画面から抜ける
		if (controlSprite_->isDraw_ == true) {
			if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A) || input_->GetPadButtonTriger(XINPUT_GAMEPAD_B)) {
				controlSprite_->isDraw_ = false;
				backSprite_->isDraw_ = false;
			}
		}

		if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A) && isStart_ == true) {
			isFade_ = true;
		}
		else if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A) && isStart_ == false) {
			controlSprite_->isDraw_ = true;
			backSprite_->isDraw_ = true;
		}
	}
}

void TitleScene::Initialize() {
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	postEffect_ = PostEffect::GetInstance();
	postEffect_->SetPostEffect(PostEffect::EffectType::None);

	TextureLoad();
	SoundLoad();

	audio_->PlayAudio(titleBGM_, 0.1f, true);
	audio_->SetVolume(titleBGM_, 0.1f);

	SpriteInitialize();
	ObjectInitialize();

	floorManager_ = std::make_unique<FloorManager>();
	floorManager_->Initialize();
	firstFloor_ = LevelLoader::GetInstance()->GetLevelObjectTransform("Cube");
	floorManager_->AddFloor(firstFloor_);

	weaponObj_ = std::make_unique<Object3D>();
	weaponObj_->Initialize("Weapon");
	weaponObj_->SetIsGetTop(true);
	weaponObj_->SetIsLighting(false);

	weaponTransform_.translate = { 0.0,7.0f,10.0f };
	weaponTransform_.scale= { 0.3f,0.3f,0.3f };
	weaponTransform_.rotate= { 0.0,0.0f,0.0f };

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetPosition(Vector3(0.0f, 10.0f, 0.0f));

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	followCamera_->SetLockOn(lockOn_.get());
}

void TitleScene::Update() {	
	Debug();
	//カメラの更新
	followCamera_->Update();
	followCamera_->SetIsMove(false);
	postEffect_->SetMatProjectionInverse(followCamera_->GetProjectionInverse());

	TitleArrowMove();

	if (isFade_) {
		fadeAlpha_ += fadeSpeed_;
	}
	else {
		fadeAlpha_ -= fadeSpeed_;
	}
	if (fadeAlpha_ >= 1.0f && isFade_) {
		SceneManager::GetInstance()->ChangeScene(SceneName::Game);
		
	}
	
	weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_);

	floorManager_->Update();
	fadeSprite_->color_.w = fadeAlpha_;

	weaponTransform_.rotate.y += 0.05f;

	if (fadeAlpha_ >= 1.0f) {
		fadeAlpha_ = 1.0f;
	}
	else if (fadeAlpha_ <= 0.0f) {
		fadeAlpha_ = 0.0f;
	}
}

void TitleScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("武器");
	ImGui::DragFloat3("座標", &weaponTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("回転", &weaponTransform_.rotate.x, 0.1f);
	ImGui::DragFloat3("大きさ", &weaponTransform_.scale.x, 0.1f);
	ImGui::End();
#endif
}

void TitleScene::DrawParticle() {
	textureManager_->PreDrawParticle();

	textureManager_->PostDrawParticle();
}

void TitleScene::DrawSkin3D() {
	textureManager_->PreDrawSkin3D();
}

void TitleScene::Draw3D() {
	/*描画前処理*/
	textureManager_->PreDrawMapping3D();
	floorManager_->Draw(followCamera_->GetViewProjection());
	textureManager_->PreDraw3D();
	stageObject_->Draw(followCamera_->GetViewProjection());
	weaponObj_->SetMatrix(weaponMatrix_);
	weaponObj_->Update(followCamera_->GetViewProjection());
	weaponObj_->Draw();

	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw3D();

	textureManager_->PreDrawSkyBox();

	textureManager_->PreDrawWorld2D();

}

void TitleScene::Draw2D() {
	/*描画前処理*/
	textureManager_->PreDraw2D();
	///*ここから下に描画処理を書き込む*/
	titleSprite_->Draw();
	startSprite_->Draw();
	selectSprite_->Draw();
	comboSprite_->Draw();
	backSprite_->Draw();
	controlSprite_->Draw();
	fadeSprite_->Draw();

	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw2D();
}

void TitleScene::Finalize() {
}

void TitleScene::AllDraw3D() {
	DrawSkin3D();
	Draw3D();
	DrawParticle();
}

void TitleScene::AllDraw2D() {
	Draw2D();
}
