#include "GameScene.h"
#include"Matrix.h"

#include <cassert>
void GameScene::TextureLoad() {
	//テクスチャ読み込み
	textureManager_->Load("resources/texture/rock.png");
	textureManager_->SetDissolveTex(textureManager_->Load("resources/texture/PostEffect/noise0.png"));
	textureManager_->Load("resources/texture/uvChecker.png");
	textureManager_->Load("resources/texture/Floor.png");
	textureManager_->Load("resources/texture/Road.png");
	textureManager_->Load("resources/Model/Enemy/EnemyTex.png");
	textureManager_->Load("resources/Model/Enemy/EnemyHitTex.png");
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

void GameScene::SoundLoad(){
	//音源読み込み
	enemyHitSE_ = audio_->LoadAudio("SE/enemyHitSE.mp3");
}

void GameScene::SpriteInitialize(){
	//テクスチャの初期化
	uint32_t textureHandle = 0;

	backSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Whitex64.png");
	backSprite_->Initialize(textureHandle);
	backSprite_->isDraw_ = false;

	for (size_t i = 0; i < timerTexs_.size(); i++){
		timerTexs_[i] = std::make_unique<Sprite>();
		textureHandle = textureManager_->Load("resources/texture/number/number.png");
		timerTexs_[i]->Initialize(textureHandle);
	}

	uiManager_ = std::make_unique<GameUIManager>();
	uiManager_->Initialize();

	backSprite_->position_ = { 640.0f,360.0f };
	backSprite_->anchorPoint_ = { 0.5f,0.5f };
	backSprite_->scale_.x = 1280.0f;
	backSprite_->scale_.y = 720.0f;
	backSprite_->color_ = { 0.0f,0.0f,0.0f,0.85f };

	for (size_t i = 0; i < timerTexs_.size(); i++) {
		timerTexs_[i]->scale_ = { 96.0f,96.0f };
		timerTexs_[i]->anchorPoint_ = { 0.5f,0.5f };
		timerTexs_[i]->uvTransform_.scale.x = 0.1f;
		timerTexs_[i]->position_ = { 500.0f,80.0f,0.0f };
	}
	timerTexs_[1]->position_.x = timerTexs_[0]->position_.x + 80.0f;
	timerTexs_[2]->position_.x = timerTexs_[0]->position_.x + 200.0f;
	timerTexs_[3]->position_.x = timerTexs_[0]->position_.x + 280.0f;

}

void GameScene::ObjectInitialize() {

	stageObject_->Initialize();

}

void GameScene::Initialize(){
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();
	textureManager_ = TextureManager::GetInstance();
	GameTime::ResetGameTimer();
	postEffect_ = PostEffect::GetInstance();
	postEffect_->SetPostEffect(PostEffect::EffectType::Gray);
	
	TextureLoad();
	SoundLoad();

	SpriteInitialize();
	ObjectInitialize();

	floorManager_ = std::make_unique<FloorManager>();
	floorManager_->Initialize();
	floorManager_->AddFloor(firstFloor_);

	player_ = std::make_unique<Player>();
	player_->Initialize();

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
	
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	followCamera_->SetLockOn(lockOn_.get());
	player_->SetLockOn(lockOn_.get());

}

void GameScene::Update(){
	bool frontFlag = false;
	DrawImgui();
	//インゲーム用の時間更新
	if (!uiManager_->GetIsTutorial()) {
		GameTime::InGameUpdate();
	}
	else{
		GameTime::ResetGameTimer();
	}
	//時間のテクスチャの更新
	uiManager_->Update();
	TimeTexUpdate();
	//timeScaleをそれぞれに渡す
	enemyManager_->SetTimeScale(GameTime::timeScale_);
	player_->SetTimeScale(GameTime::timeScale_);

	//カメラの更新
	followCamera_->Update();
	followCamera_->SetIsMove(true);	
	postEffect_->SetMatProjectionInverse(followCamera_->GetProjectionInverse());

	frontFlag = player_->GetIsJustAvoid();
	player_->SetIsSPAttackUse(uiManager_->GetIsUse());
	player_->Update();

	if (uiManager_->GetIsTutorial() == false){
		enemyManager_->SetTutorialFlug(false);
	}
	enemyManager_->Update();
	lockOn_->Update(enemyManager_->GetEnemies(), followCamera_->GetViewProjection(), input_, followCamera_->GetLockViewingFrustum(), player_->GetIsJustAvoid(), player_->GetSerialNumber());
	//当たり判定
	AllCollision();

	floorManager_->Update();
	//フラグが切れた瞬間だけリセットする
	if (!player_->GetIsJustAvoid() and frontFlag){
		lockOn_->TargetReset();
	}
	//現状は倒しきったら遷移
	if (enemyManager_->GetEnemyNum() == 0) {
		SceneManager::GetInstance()->ChangeScene(SceneName::Result);
	}		
#ifdef _DEBUG
	//再読み込み
	if (input_->Trigerkey(DIK_R)) {
		SceneManager::GetInstance()->ChangeScene(SceneName::Game);
	}
#endif // _DEBUG		
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

	textureManager_->PreDrawWorld2D();

	player_->TexDraw(followCamera_->GetViewProjection().matViewProjection_);
	enemyManager_->TexDraw(followCamera_.get());	
}

void GameScene::Draw2D(){
	/*描画前処理*/
	textureManager_->PreDraw2D();
	///*ここから下に描画処理を書き込む*/
	for (size_t i = 0; i < timerTexs_.size(); i++){
		timerTexs_[i]->Draw();
	}
	uiManager_->Draw();
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
	//分と秒を描画するための更新
	timerTexs_[3]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetSecondsOnes());
	timerTexs_[2]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetSecondsTens());
	timerTexs_[1]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetMinutes());
	timerTexs_[0]->uvTransform_.translate.x = (float)(0.1f * GameTime::GetMinutesTens());
}

void GameScene::AllCollision(){
	//床の当たり判定
	PlayerFloorCollision(player_.get(), floorManager_.get());
	//敵との当たり判定
	
	//プレイヤーがノックバック中であれば被弾しないように
	if (player_->GetNowState() == BasePlayerState::StateName::kLeaningBack) {
		return;
	}

	for (const auto& enemy : enemyManager_->GetEnemies()) {
		PlayerEnemyCollision(player_.get(), enemy.get());
	}
	//ボスとの当たり判定
	//チュートリアル時はスキップ
	if (enemyManager_->GetIsTutorial()){
		return;
	}

	const auto& enemy = enemyManager_->GetBossEnemy();
	PlayerBossCollision(player_.get(), enemy);
		
	
}

void GameScene::PlayerEnemyCollision(Player* player, Enemy* enemy){	

	uint32_t serialNumber = enemy->GetSerialNumber();
	//敵がカメラに写っているか
	if (IsCollisionOBBViewFrustum(enemy->GetBodyOBB(), followCamera_->GetLockViewingFrustum())) {
		enemy->SetIsOnScreen(true);
	}
	else {
		enemy->SetIsOnScreen(false);
	}
	//敵が映っていなかったらこの敵は処理しない
	if (!enemy->GetIsOnScreen()) {
		return;
	}

	//ガード判定と敵の攻撃判定との処理
	if (player->GetIsGuard()) {
		if (IsCollisionOBBOBB(player->GetWeaponOBB(), enemy->GetAttackOBB())) {
			if (player->RecordCheck(serialNumber)) {
				return;
			}
			player->OnCollisionEnemyAttack();
			//ノックバックの種類を指定
			enemy->SetKnockBackType(HitRecord::KnockbackType::Guard);
			//ヒット音の再生
			audio_->PlayAudio(enemyHitSE_, seVolume_, false);
			//当たったときの処理
			enemy->OnCollisionGuard();
		}
	}
	//武器の判定と敵の体の判定との処理
	if (IsCollisionOBBOBB(player->GetWeaponOBB(), enemy->GetBodyOBB())) {
		if (player->RecordCheck(serialNumber)) {
			return;
		}
		if (player->GetIsGuard()) {

		}
		else {
			//ノックバックの種類を指定
			enemy->SetKnockBackType(player->GetKnockbackType());
			//接触履歴に登録
			player->AddRecord(serialNumber);
			//ヒット音の再生
			audio_->PlayAudio(enemyHitSE_, seVolume_, false);
			//ヒットストップ
			GameTime::StopTime(player->GetHitStop());
			//チュートリアル以外ではゲージ増加を適応
			if (!enemyManager_->GetIsTutorial()) {
				//ヒット時に必殺ゲージを伸ばす
				uiManager_->AddSPGauge();
			}
			//当たったときの処理
			if (player->ChackStrongBack()) {
				followCamera_->StartShake(playerAttackShake_.x, playerAttackShake_.y);
				enemy->OnCollisionStrong();
			}
			else {
				enemy->OnCollision();
			}
		}
	}

	if (player->GetNowState() == BasePlayerState::StateName::SpecialAttack) {
		return;
	}

	//プレイヤー自身と敵の体の判定
	if (IsCollisionOBBOBB(player->GetOBB(), enemy->GetBodyOBB())) {
		player->SetCollisionEnemy(true);
		return;
	}
	else {
		player->SetCollisionEnemy(false);
	}
	//回避と敵の攻撃との判定
	if (player->GetIsDash()) {
		if (IsCollisionOBBOBB(player->GetJustAvoidOBB(), enemy->GetAttackOBB())) {

			player->OnCollisionEnemyAttackAvoid(serialNumber);
		}
	}
	else {
		if (!player->GetIsGuard()) {
			//ガードをしていなかったら敵の攻撃とプレイヤーとの判定
			if (IsCollisionOBBOBB(player->GetOBB(), enemy->GetAttackOBB())) {
				GameTime::AddGameTime();
				followCamera_->StartShake(enemyAttackShake_.x, enemyAttackShake_.y);
				player->OnCollisionEnemyAttack();
			}
		}
	}


}

void GameScene::PlayerBossCollision(Player* player, BossEnemy* enemy){
	uint32_t serialNumber = enemy->GetSerialNumber();

	//自機キャラがのけぞり中であればこれ以上判定しない
	if (player->GetNowState() == BasePlayerState::StateName::kLeaningBack) {
		return;
	}

	//敵がカメラに写っているか
	if (IsCollisionOBBViewFrustum(enemy->GetBodyOBB(), followCamera_->GetLockViewingFrustum())) {
		enemy->SetIsOnScreen(true);
	}
	else {
		enemy->SetIsOnScreen(false);
	}
	//敵が映っていなかったらこの後は処理しない
	if (!enemy->GetIsOnScreen()) {

	}
	else {
		//ガード判定と敵の攻撃判定との処理
		if (player->GetIsGuard()) {
			if (IsCollisionOBBOBB(player->GetWeaponOBB(), enemy->GetAttackOBB())) {
				if (player->RecordCheck(serialNumber)) {
					return;
				}
				player->OnCollisionEnemyAttack();
				//ノックバックの種類を指定
				enemy->SetKnockBackType(HitRecord::KnockbackType::Guard);
				//ヒット音の再生
				audio_->PlayAudio(enemyHitSE_, seVolume_, false);
				//当たったときの処理
				enemy->OnCollisionGuard();
			}
		}
		//武器の判定と敵の体の判定との処理
		if (IsCollisionOBBOBB(player->GetWeaponOBB(), enemy->GetBodyOBB())) {
			if (player->RecordCheck(serialNumber)) {
				return;
			}
			if (player->GetIsGuard()) {

			}
			else {
				//ノックバックの種類を指定
				enemy->SetKnockBackType(player->GetKnockbackType());
				//接触履歴に登録
				player->AddRecord(serialNumber);
				//ヒット音の再生
				audio_->PlayAudio(enemyHitSE_, seVolume_, false);
				//ヒットストップ
				GameTime::StopTime(player->GetHitStop());
				//ヒット時に必殺ゲージを伸ばす
				uiManager_->AddSPGauge();
				//当たったときの処理
				if (player->ChackStrongBack()) {
					followCamera_->StartShake(playerAttackShake_.x, playerAttackShake_.y);
					enemy->OnCollisionStrong();
				}
				else {
					enemy->OnCollision();
				}
			}
		}

		if (player->GetNowState() == BasePlayerState::StateName::SpecialAttack) {
			return;
		}

		//プレイヤー自身と敵の体の判定
		if (IsCollisionOBBOBB(player->GetOBB(), enemy->GetBodyOBB())) {
			player->SetCollisionEnemy(true);
			return;
		}
		else {
			player->SetCollisionEnemy(false);
		}
		//回避と敵の攻撃との判定
		if (player->GetIsDash()) {
			if (IsCollisionOBBOBB(player->GetJustAvoidOBB(), enemy->GetAttackOBB())) {
				player->OnCollisionEnemyAttackAvoid(serialNumber);
			}
		}
		else {
			if (!player->GetIsGuard()) {
				//ガードをしていなかったら敵の攻撃とプレイヤーとの判定
				if (IsCollisionOBBOBB(player->GetOBB(), enemy->GetAttackOBB())) {
					GameTime::AddGameTime();
					followCamera_->StartShake(enemyAttackShake_.x, enemyAttackShake_.y);
					player->OnCollisionEnemyAttack();
				}
			}
		}
	}

}

void GameScene::PlayerFloorCollision(Player* player, const FloorManager* floorManager){
	for (const auto& floor : floorManager->GetFloors()) {
		if (IsCollisionOBBOBB(floor->GetOBB(), player->GetOBB())) {
			chackCollision = 1;
			player->OnFlootCollision(floor->GetOBB());
			player->SetIsDown(false);
			break;
		}
		else {
			chackCollision = 0;
			player->SetIsDown(true);
		}
	}
	
}

