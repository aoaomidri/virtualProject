#include "GameScene.h"
#include"Matrix.h"

#include <cassert>
void GameScene::TextureLoad() {
	textureManager_->Load("resources/texture/rock.png");
	textureManager_->SetDissolveTex(textureManager_->Load("resources/texture/PostEffect/noise0.png"));
	textureManager_->Load("resources/texture/uvChecker.png");
	textureManager_->Load("resources/texture/Floor.png");
	textureManager_->Load("resources/texture/Road.png");
	//textureManager_->Load("resources/texture/Sky.png");
	textureManager_->Load("resources/Model/Enemy/EnemyTex.png");
	textureManager_->Load("resources/Model/Enemy/EnemyHitTex.png");
	textureManager_->Load("resources/Model/EnemyParts/EnemyParts.png");
	textureManager_->Load("resources/Model/Weapon/Sword.png");
	textureManager_->Load("resources/texture/Magic.png");
	textureManager_->Load("resources/texture/Black.png");
	textureManager_->Load("resources/texture/circle.png");
	textureManager_->Load("resources/texture/monsterBall.png");
	textureManager_->Load("resources/texture/title.png");
	textureManager_->Load("resources/texture/pressA.png");
	textureManager_->Load("resources/texture/Clear.png");
	textureManager_->Load("resources/texture/Whitex64.png");
	textureManager_->Load("resources/texture/STAttack.png");
	textureManager_->Load("resources/texture/dash.png");
	textureManager_->Load("resources/texture/RB.png");
	textureManager_->Load("resources/texture/actionText.png");
	textureManager_->Load("resources/DDS/rostock_laage_airport_4k.dds");
}

void GameScene::SoundLoad(){
	
	enemyHitSE_ = audio_->LoadAudio("SE/enemyHitSE.mp3");

}

void GameScene::SpriteInitialize(){
	uint32_t textureHandle = 0;

	//////ここから修正
	backSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Whitex64.png");
	backSprite_->Initialize(textureHandle);
	backSprite_->isDraw_ = false;


	fadeSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Black.png");
	fadeSprite_->Initialize(textureHandle);

	actionTextSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/actionText.png");
	actionTextSprite_->Initialize(textureHandle);

	attackSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/STAttack.png");
	attackSprite_->Initialize(textureHandle);

	for (size_t i = 0; i < timerTexs_.size(); i++){
		timerTexs_[i] = std::make_unique<Sprite>();
		textureHandle = textureManager_->Load("resources/texture/number/number.png");
		timerTexs_[i]->Initialize(textureHandle);
	}

}

void GameScene::ObjectInitialize() {
	

	skyBox_ = std::make_unique<SkyBox>();
	skyBox_->Initialize("resources/DDS/rostock_laage_airport_4k.dds");

	skyBox_->transform_.scale = { 1000.0f,1000.0f,1000.0f };

	stageObject_->Initialize();

}

void GameScene::Initialize(){
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();

	GameTime::ResetGameTimer();

	textureManager_ = TextureManager::GetInstance();

	postEffect_ = PostEffect::GetInstance();
	postEffect_->SetPostEffect(PostEffect::EffectType::Gray);
	
	TextureLoad();
	SoundLoad();

	

	SpriteInitialize();
	ObjectInitialize();

	floorManager_ = std::make_unique<FloorManager>();
	floorManager_->Initialize();

	//firstFloor_ = LevelLoader::GetInstance()->GetLevelObjectTransform("Cube");

	floorManager_->AddFloor(firstFloor_);

	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->Update();

	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->SetTarget(&player_->GetTransform());
	enemyManager_->SetTargetMat(&player_->GetRotateMatrix());
	enemyManager_->Initialize();

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	//自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTargetMatrix(&player_->GetRotateMatrix());
	followCamera_->SetTarget(&player_->GetTransform());
	
	player_->SetViewProjection(&followCamera_->GetViewProjection());


	backSprite_->position_ = { 640.0f,360.0f };
	backSprite_->anchorPoint_ = { 0.5f,0.5f };
	backSprite_->scale_.x = 1280.0f;
	backSprite_->scale_.y = 720.0f;
	backSprite_->color_ = {0.0f,0.0f,0.0f,0.85f };

	actionTextSprite_->position_ = { 1072.0f,500.0f };
	actionTextSprite_->anchorPoint_ = { 0.5f,0.5f };
	actionTextSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	attackSprite_->position_ = { 1072.0f,650.0f };
	attackSprite_->anchorPoint_ = { 0.5f,0.5f };
	attackSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	fadeSprite_->position_ = { 640.0f,360.0f };
	fadeSprite_->scale_.x = 1280.0f;
	fadeSprite_->scale_.y = 720.0f;
	fadeSprite_->color_ = { 0.0f,0.0f,0.0f,fadeAlpha_ };
	fadeSprite_->anchorPoint_ = { 0.5f,0.5f };

	for (size_t i = 0; i < timerTexs_.size(); i++){
		timerTexs_[i]->scale_ = { 96.0f,96.0f };
		timerTexs_[i]->anchorPoint_ = { 0.5f,0.5f };
		timerTexs_[i]->uvTransform_.scale.x = 0.1f;
		timerTexs_[i]->position_ = { 500.0f,80.0f,0.0f };
	}

	
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	followCamera_->SetLockOn(lockOn_.get());
	player_->SetLockOn(lockOn_.get());
	//player_->Update();

}

void GameScene::Update(){
	bool frontFlag = false;
	DrawImgui();

	GameTime::InGameUpdate();

	TimeTexUpdate();

	timerTexs_[1]->position_.x = timerTexs_[0]->position_.x + 80.0f;
	timerTexs_[2]->position_.x = timerTexs_[0]->position_.x + 200.0f;
	timerTexs_[3]->position_.x = timerTexs_[0]->position_.x + 280.0f;

	
	
	followCamera_->Update();
	postEffect_->SetMatProjectionInverse(followCamera_->GetProjectionInverse());
		
	followCamera_->SetIsMove(true);		
		
	fadeAlpha_ -= 0.01f;
	if (fadeAlpha_<=0.0f){
			
			
		isFade_ = false;
		fadeAlpha_ = 0.0f;

		frontFlag = player_->GetIsJustAvoid();
		player_->SetTimeScale(GameTime::timeScale_);
		player_->Update();
		lockOn_->Update(enemyManager_->GetEnemies(), followCamera_->GetViewProjection(), input_, followCamera_->GetLockViewingFrustum(), player_->GetIsJustAvoid(), player_->GetSerialNumber());

		if (!player_->GetIsJustAvoid() and frontFlag){
			lockOn_->TargetReset();
		}
		if (enemyManager_->GetEnemyNum() == 0) {
			SceneManager::GetInstance()->ChangeScene(SceneName::Result);
		}
		enemyManager_->SetTimeScale(GameTime::timeScale_);
		enemyManager_->Update();
	}
	/*if (player_->GetIsJustAvoid()){
		
		postEffect_->SetPostEffect(PostEffect::EffectType::Gray);
	}
	else if (postEffect_->GetEffectType() != PostEffect::EffectType::None and player_->GetHitTimer() == 0 ) {
		postEffect_->SetPostEffect(PostEffect::EffectType::None);
	}*/
		

	if (input_->Trigerkey(DIK_C) && input_->Trigerkey(DIK_L)) {

	}
#ifdef _DEBUG

	if (input_->Trigerkey(DIK_R)) {
		SceneManager::GetInstance()->ChangeScene(SceneName::Game);
	}

#endif // _DEBUG	
	

	AllCollision();


	floorManager_->Update();
	fadeSprite_->color_.w = fadeAlpha_;
	

	if (fadeAlpha_>=1.0f){
		fadeAlpha_ = 1.0f;
	}
	else if (fadeAlpha_ <= 0.0f) {
		fadeAlpha_ = 0.0f;
	}
}

void GameScene::Debug(){

}

void GameScene::DrawParticle(){
	textureManager_->PreDrawParticle();

	player_->ParticleDraw(followCamera_->GetViewProjection());
	
	enemyManager_->ParticleDraw(followCamera_.get(), player_->GetTrailColor());
	

	textureManager_->PostDrawParticle();
}

void GameScene::DrawSkin3D(){
	textureManager_->PreDrawSkin3D();
	player_->SkinningDraw(followCamera_->GetViewProjection());

}

void GameScene::Draw3D(){

	/*描画前処理*/
	textureManager_->PreDrawMapping3D();

	///*ここから下に描画処理を書き込む*/

	textureManager_->PreDraw3D();
	player_->Draw(followCamera_->GetViewProjection());

	enemyManager_->Draw(followCamera_.get());

	lockOn_->Draw();
	floorManager_->Draw(followCamera_->GetViewProjection());
	stageObject_->Draw(followCamera_->GetViewProjection());
	
	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw3D();

	//textureManager_->PreDrawSkyBox();

	//skyBox_->Update(followCamera_->GetViewProjection());
	//skyBox_->Draw();

	textureManager_->PreDrawWorld2D();

	player_->TexDraw(followCamera_->GetViewProjection().matViewProjection_);

	enemyManager_->TexDraw(followCamera_.get());
	
}

void GameScene::Draw2D(){
	/*描画前処理*/
	textureManager_->PreDraw2D();
	///*ここから下に描画処理を書き込む*/
	actionTextSprite_->Draw();
	attackSprite_->Draw();
	fadeSprite_->Draw();

	for (size_t i = 0; i < timerTexs_.size(); i++){
		timerTexs_[i]->Draw();
	}
	
		
	//testTexture_->Draw(textureManager_->SendGPUDescriptorHandle(0));
	
	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw2D();
}

void GameScene::Finalize(){
}

void GameScene::AllDraw3D(){
	DrawSkin3D();
	Draw3D();
	DrawParticle();
}

void GameScene::AllDraw2D(){
	Draw2D();
}

void GameScene::DrawImgui(){
#ifdef _DEBUG
	ImGui::Begin("テクスチャ");
	ImGui::DragFloat3("position", &timerTexs_[0]->position_.x, 0.1f);
	ImGui::DragFloat3("uvTrans", &timerTexs_[0]->uvTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("uvScale", &timerTexs_[0]->uvTransform_.scale.x, 0.1f);
	ImGui::DragFloat3("uvRotate", &timerTexs_[0]->uvTransform_.rotate.x, 0.1f);
	ImGui::End();


	player_->DrawImgui();
	followCamera_->DrawImgui();
	lockOn_->DrawImgui();
	floorManager_->DrawImgui();

	enemyManager_->DrawImgui();

#endif // _DEBUG	
}

void GameScene::TimeTexUpdate(){
	timerTexs_[3]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetSecondsOnes());
	timerTexs_[2]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetSecondsTens());
	timerTexs_[1]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetMinutes());
	timerTexs_[0]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetMinutesTens());
}



void GameScene::AllCollision(){
	for (const auto& floor : floorManager_->GetFloors()) {
		if (IsCollisionOBBOBB(floor->GetOBB(), player_->GetOBB())) {
			chackCollision = 1;
			player_->OnFlootCollision(floor->GetOBB());
			player_->SetIsDown(false);
			break;
		}
		else {
			chackCollision = 0;
			player_->SetIsDown(true);
		}
	}

	for (const auto& enemy : enemyManager_->GetEnemies()) {
		uint32_t serialNumber = enemy->GetSerialNumber();
		if (IsCollisionOBBViewFrustum(enemy->GetBodyOBB(),followCamera_->GetLockViewingFrustum())){
			enemy->SetIsOnScreen(true);
		}
		else {
			enemy->SetIsOnScreen(false);
		}

		if (!enemy->GetIsOnScreen()) {
			continue;
		} 

		if (player_->GetIsGuard()){
			if (IsCollisionOBBOBB(player_->GetWeaponOBB(), enemy->GetAttackOBB())) {
				
				if (player_->RecordCheck(serialNumber)) {
					return;
				}

				
				player_->OnCollisionEnemyAttack();
				//ノックバックの種類を指定
				enemy->SetKnockBackType(HitRecord::KnockbackType::Guard);
				//ヒット音の再生
				audio_->PlayAudio(enemyHitSE_, 0.3f, false);
				//当たったときの処理
				enemy->OnCollisionGuard();
				
			}
		}
		
		if (IsCollisionOBBOBB(player_->GetWeaponOBB(), enemy->GetBodyOBB())) {
			if (player_->RecordCheck(serialNumber)) {
				return;
			}
			if (player_->GetIsGuard()) {
				
			}
			else {
				//ノックバックの種類を指定
				enemy->SetKnockBackType(player_->GetKnockbackType());
				//接触履歴に登録
				player_->AddRecord(serialNumber);
				//ヒット音の再生
				audio_->PlayAudio(enemyHitSE_, 0.3f, false);
				//ヒットストップ
				GameTime::StopTime(player_->GetHitStop());
				//当たったときの処理
				if (player_->ChackStrongBack()){
					followCamera_->StartShake(1.0f, 3.0f);
					enemy->OnCollisionStrong();
				}
				else {
					enemy->OnCollision();
				}
				
				
			}
		}
		

		if (IsCollisionOBBOBB(player_->GetOBB(), enemy->GetBodyOBB())) {
			player_->SetCollisionEnemy(true);
			break;
		}
		else {
			player_->SetCollisionEnemy(false);
		}

		if (player_->GetIsDash()){
			if (IsCollisionOBBOBB(player_->GetJustAvoidOBB(), enemy->GetAttackOBB())) {
				
				player_->OnCollisionEnemyAttackAvoid(serialNumber);
			}
		}
		else {
			if (!player_->GetIsGuard()) {
				if (IsCollisionOBBOBB(player_->GetOBB(), enemy->GetAttackOBB())) {
					GameTime::AddGameTime();
					followCamera_->StartShake(0.5f, 1.5f);
					player_->OnCollisionEnemyAttack();

				}
			}
		}
		
	}

}

void GameScene::FilesSave(const std::vector<std::string>& stages) {
	floorManager_->SaveFile(stages);
	std::string message = std::format("{}.json created.", "all");
	MessageBoxA(nullptr, message.c_str(), "StagesObject", 0);
}

void GameScene::FilesOverWrite(const std::string& stage) {
	floorManager_->FileOverWrite(stage);
	std::string message = std::format("{}.json OverWrite.", "all");
	MessageBoxA(nullptr, message.c_str(), "StagesObject", 0);
}

void GameScene::FilesLoad(const std::string& stage) {
	floorManager_->LoadFiles(stage);
	std::string message = std::format("{}.json loaded.", "all");
	MessageBoxA(nullptr, message.c_str(), "StagesObject", 0);
}


