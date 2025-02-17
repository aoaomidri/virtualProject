#include "Player.h"
#include"LockOn.h"
#include"ImGuiManager.h"
#include"Ease/Ease.h"
#include"LevelLoader/LevelLoader.h"

Player::~Player(){
}

void Player::ApplyGlobalVariables() {
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";

	downSpeed_ = adjustment_item->GetfloatValue(groupName, "DownSpeed");
	jumpPower_ = adjustment_item->GetfloatValue(groupName, "JumpPower");
	baseAttackPower_ = adjustment_item->GetIntValue(groupName, "AttackPower");
	trailPosData_ = adjustment_item->GetVector2Value(groupName, "TrailPosData");
	hitStop_ = adjustment_item->GetfloatValue(groupName, "HitStop");
	strongHitStop_ = adjustment_item->GetfloatValue(groupName, "StrongHitStop");
	addPostT_ = adjustment_item->GetfloatValue(groupName, "AddPostT");
	scaleValue_ = adjustment_item->GetfloatValue(groupName, "ScaleValue");
}

void Player::Initialize(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "DownSpeed", downSpeed_);
	adjustment_item->AddItem(groupName, "JumpPower", jumpPower_);
	adjustment_item->AddItem(groupName, "AttackPower", baseAttackPower_);
	adjustment_item->AddItem(groupName, "TrailPosData", trailPosData_);
	adjustment_item->AddItem(groupName, "HitStop", hitStop_);
	adjustment_item->AddItem(groupName, "StrongHitStop", strongHitStop_);
	adjustment_item->AddItem(groupName, "AddPostT", addPostT_);
	adjustment_item->AddItem(groupName, "ScaleValue", scaleValue_);
	
	input_ = Input::GetInstance();

	stateManager_ = PlayerStateManager::GetInstance();
	stateManager_->ChangeState(BasePlayerState::StateName::Root);
	stateManager_->InitGlobalVariables();
	stateManager_->InitState();

	playerObj_ = std::make_unique<Object3D>();
	playerObj_->Initialize("PlayerFace");
	playerObj_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());
	playerObj_->SetIsLighting(true);

	weaponObj_ = std::make_unique<Object3D>();
	weaponObj_->Initialize("Weapon");
	weaponObj_->SetIsGetTop(true);	
	weaponObj_->SetIsLighting(false);

	collisionObj_ = std::make_unique<Object3D>();
	collisionObj_->Initialize("box");

	shadow_ = std::make_unique<Sprite>();
	shadow_->Initialize(TextureManager::GetInstance()->Load("resources/texture/shadow.png"));
	shadow_->rotation_.x = 1.57f;
	shadow_->scale_ = { 0.7f,0.7f };
	shadow_->anchorPoint_ = { 0.5f,0.5f };
	shadow_->color_.w = 0.5f;

	groundCrush_ = std::make_unique<Sprite>();
	groundCrush_->Initialize(TextureManager::GetInstance()->Load("resources/texture/groundCrush.png"));
	groundCrush_->position_ = { 20.0f ,1.04f,0.0f };
	groundCrush_->rotation_.x = 1.57f;
	groundCrush_->scale_ = { 3.0f,3.0f };
	groundCrush_->anchorPoint_ = { 0.5f,0.5f };
	groundCrush_->color_.w = 1.0f;
	groundOffsetBase_ = { 0.0f,0.1f,6.0f };
	isStopCrush_ = false;

	weaponCollisionObj_ = std::make_unique<Object3D>();
	weaponCollisionObj_->Initialize("box");

	emitter_.count = 27;
	emitter_.transform = {
		playerObj_->transform_.scale,
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	emitter_.transform.scale /= 5.0f;

	particle_ = std::make_unique<ParticleBase>();
	particle_->Initialize(emitter_, false);
	particle_->SetIsDraw(false);
	particle_->SetBlend(BlendMode::kBlendModeNormal);
	particle_->SetIsBillborad(true);
	particle_->SetIsUpper(true);
	particle_->SetLifeTime(0.4f);
	particle_->SetVelocityRange(Vector2(-10.0f, 10.0f));

	playerTransform_ = stateManager_->GetPlayerTrnaform();
	particleTrans_ = playerTransform_;
	particleTransCenter_ = playerTransform_;

	playerRotateMatrix_ = Matrix::MakeIdentity4x4();
	isDown_ = true;

	trail_ = std::make_unique<TrailEffect>();
	trail_->Initialize(12, "resources/texture/TrailEffect/whiteTrail.png");

	trailRender_ = std::make_unique<TrailRender>();
	trailRender_->Initialize();

	audio_ = Audio::GetInstance();	
	playerHitSE_ = audio_->LoadAudio("SE/playerHitSE.mp3");

}

void Player::Update(){
	ApplyGlobalVariables();
	if (stateManager_->GetIsGuardHit()){
		trailPosData_ = trailPosDataGuard_;
	}
	//武器のディゾルブ関連
	weaponObj_->SetDissolve(weaponThreshold_);
	weaponObj_->SetTrailPos(trailPosData_);

	if (stateManager_->GetIsDissolve()){
		weaponThreshold_ += addThresholdSpeed_ * timeScale_;
		if (weaponThreshold_ > 1.0f) {
			//完全に消えたら
			weaponThreshold_ = 1.0f;
			trail_->Reset();
			stateManager_->SetIsDissolve(false);
		}
	}
	else {
		//徐々に元に戻す
		weaponThreshold_ -= minusThresholdSpeed_;
		if (weaponThreshold_ < 0.0f) {			
			weaponThreshold_ = 0.0f;
		}		
	}

	if (stateManager_->GetStateName() == PlayerStateManager::StateName::Attack or stateManager_->GetStateName() == PlayerStateManager::StateName::StrongAttack) {
		weaponThreshold_ = 0.0f;
	}
	//カウンター時間経過処理
	if (counterTime_ > counterTimeBase_){
		stateManager_->SetIsGuardHit(false);
	}
	if (stateManager_->GetIsGuardHit()){
		counterTime_ += timeScale_;
	}	
	//ジャスト回避時間経過処理
	if (stateManager_->GetJustAvoidTimer() > 0) {
		postT_ = 0.9f;		
	}
	else if (stateManager_->GetJustAvoidTimer() <= 0) {
		postT_ -= addPostT_;
		isJustAvoid_ = false;
	}
	//値を制限
	if (postT_ > 1.0f){
		postT_ = 1.0f;
	}
	else if (postT_ < 0.0f){
		postT_ = 0.0f;
	}

	postBlend_ = Ease::Easing(Ease::EaseName::EaseInBack, 0.0f, 1.0f, postT_);
	PostEffect::GetInstance()->SetPostBlend(postBlend_);

	//被弾無敵の経過処理
	if (hitTimer_ != 0) {
		hitTimer_--;
	}
	else if (hitTimer_ <= 0) {
		isHitEnemyAttack_ = false;
	}
	
	/*状態によっての処理をここに記述する*/
	//playerStateManagerなど
	stateManager_->SetIsJustAvoid(isJustAvoid_);
	stateManager_->SetLockOnPos(lockOn_->GetTargetPosition());
	stateManager_->Update(viewProjection_->rotation_);

	//Stateによって変更が生じた値を代入する
	type_ = stateManager_->GetKnockbackType();

	playerTransform_.translate.x = stateManager_->GetPlayerTrnaform().translate.x;
	playerTransform_.translate.z = stateManager_->GetPlayerTrnaform().translate.z;
	playerAppearanceTransform_ = stateManager_->GetPlayerAppearanceTrnaform();
	weaponTransform_ = stateManager_->GetWeaponTrnaform();
	weaponCollisionTransform_ = stateManager_->GetWeaponCollisionTrnaform();
	playerRotateMatrix_ = stateManager_->GetPlayerRotateMatrix();
	//当たりの記憶を削除する
	if (stateManager_->GetHitRecordResetFlug()){
		hitRecord_.Clear();
		stateManager_->SetHitRecordResetFlug(false);
	}
	//trailの頂点情報を削除する
	if (stateManager_->GetTrailResetFlug()) {
		trail_->Reset();
		stateManager_->SetTrailResetFlug(false);
	}

	if (stateManager_->GetIsJump() && !isDown_) {
		downVector_.y = jumpPower_;
	}

	/*落下処理*/
	if (isDown_) {
		downVector_.y += downSpeed_ * timeScale_;
	}	
	playerTransform_.translate.y += downVector_.y * timeScale_;
	//落下処理による修正後の値を代入
	stateManager_->SetPlayerTranslateY(playerTransform_.translate.y);

	//影の処理
	shadow_->position_ = playerTransform_.translate;
	shadow_->position_.y = shadowHeight_;
	shadow_->scale_.x = shadowScaleBase_ + (shadowScaleCulcBase_ - playerTransform_.translate.y);
	shadow_->scale_.y = shadowScaleBase_ + (shadowScaleCulcBase_ - playerTransform_.translate.y);

	groundCrush_->color_.w = stateManager_->GetGroundCrushTexAlpha();
	//地面のヒビの処理
	if (not stateManager_->GetIsStopCrush()) {
		groundCrush_->position_ = playerTransform_.translate + Matrix::TransformNormal(groundOffsetBase_, playerRotateMatrix_);
		groundCrush_->position_.y = crushPosBase_;
	}
	//0以下なら固定する
	if (shadow_->scale_.x < 0.0f) {
		shadow_->scale_.x = 0.0f;
		shadow_->scale_.y = 0.0f;
	}

	/*行列やobbの更新処理*/
	PlayerCalculation();

	//トレイルの更新処理
	if (stateManager_->GetStateName() == PlayerStateManager::StateName::Attack or stateManager_->GetStateName() == PlayerStateManager::StateName::StrongAttack) {
		Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
		weaponObj_->SetMatrix(weaponMatrix_);
		weaponObj_->UniqueUpdate();
		if (timeScale_ != 0.0f) {
			trail_->SetPos(weaponObj_->GetTopAndTailVerTex().head, weaponObj_->GetTopAndTailVerTex().tail);
		}
	}

	if (timeScale_ != 0.0f){
		trail_->Update();
	}
	
}

void Player::TexDraw(const Matrix4x4& viewProjection){
	shadow_->Draw(viewProjection);
	if (stateManager_->GetIsStopCrush()) {
		groundCrush_->Draw(viewProjection);
	}
	trailRender_->SetIsDraw(stateManager_->GetIsDrawTrail());
	trailRender_->Draw(trail_.get(), viewProjection);
}

void Player::Draw(const ViewProjection& viewProjection){
	playerObj_->SetMatrix(playerMatrix_);
	playerObj_->Update(viewProjection);
	playerObj_->Draw();
	if (!stateManager_->GetIsDash()){
		weaponObj_->SetMatrix(weaponMatrix_);
		weaponObj_->SetShininess(shiness_);
		weaponObj_->Update(viewProjection);
		weaponObj_->Draw();

	}

#ifdef _DEBUG
	

#endif
		
}

void Player::SkinningDraw(const ViewProjection& viewProjection){
	/*昔は使っていたが現状はスキニングモデルを使っていないので放置*/
	viewProjection;
	
}

void Player::ParticleDraw(const ViewProjection& viewProjection){
	EulerTransform newTrans = playerTransform_;

	particle_->SetOneColor(trailRender_->GetTrailColor());
	particle_->Update(newTrans, viewProjection);	
	particle_->Draw();

}

void Player::DrawImgui(){
#ifdef _DEBUG
	ImGui::Begin("プレイヤーのステータス");
	ImGui::SliderFloat("ジャンプ力", &jumpPower_, 0.0f, 1.0f, "%.3f");
	if (ImGui::Button("ジャンプ")){
		downVector_.y = jumpPower_;
	}
	ImGui::DragFloat3("DownVector", &downVector_.x, 0.01f);
	ImGui::Text("スティックの縦 = %.4f", input_->GetPadLStick().y);
	ImGui::Text("スティックの横 = %.4f", input_->GetPadLStick().x);
	ImGui::DragFloat3("OBB座標", &obbPoint_.x, 0.01f);
	ImGui::DragFloat3("OBB大きさ", &obbAddScale_.x, 0.01f);
	ImGui::DragFloat3("自機の座標", &playerTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("自機の見た目上の回転", &playerAppearanceTransform_.rotate.x, 0.1f);
	ImGui::Text("ロックオンしている敵 = %d", enemyNumber_);
	ImGui::Text("おちているかどうか = %d", isDown_);
	ImGui::Text("ジャスト回避中か = %d", isJustAvoid_);
	ImGui::Text("敵に当たっているか = %d", isCollisionEnemy_);
	ImGui::Text("敵に攻撃に当たっているか = %d", isHitEnemyAttack_);
	ImGui::DragFloat3("武器の回転", &weaponTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("武器攻撃判定の回転", &weaponCollisionTransform_.rotate.x, 0.1f);	
	ImGui::DragFloat("武器の反射", &shiness_, 0.01f, 0.0f, 100.0f);
	ImGui::Checkbox("武器のしきい値", &isDissolve_);
	ImGui::DragFloat2("武器のトレイル表示座標", &trailPosData_.x, 0.01f);
	ImGui::End();

	ImGui::Begin("ひび割れテクスチャ");
	ImGui::DragFloat3("座標", &shadow_->position_.x, 0.01f);
	ImGui::DragFloat2("大きさ", &shadow_->scale_.x, 0.01f, 0.0f, 10.0f);
	ImGui::End();

	playerObj_->DrawImgui("プレイヤー");

	trail_->DrawImgui("トレイル");
	//particle_->DrawImgui("プレイヤーパーティクル");
#endif
}

void Player::OnFlootCollision(OBB obb){
	playerTransform_.translate.y = playerOBB_.size.y + obb.size.y;
	downVector_ = { 0.0f,0.0f,0.0f };
	isDown_ = false;
	stateManager_->JumpFlugReset();
}

void Player::PlayerCalculation(){
	playerScaleMatrix_ = Matrix::MakeScaleMatrix(playerTransform_.scale);
	playerTranslateMatrix_ = Matrix::MakeTranslateMatrix(playerTransform_.translate);

	//プレイヤーのobbの更新
	playerOBB_.center = playerTransform_.translate + obbPoint_;
	playerOBB_.size = playerTransform_.scale + obbAddScale_;
	SetOridentatios(playerOBB_, playerRotateMatrix_);
	//回避用のobbの更新
	justAvoidOBB_ = playerOBB_;
	justAvoidObbScale_ = { scaleValue_,scaleValue_,scaleValue_ };
	justAvoidOBB_.size = playerOBB_.size + justAvoidObbScale_;
	//武器のobbの更新
	weaponOBB_.center = weaponCollisionTransform_.translate;
	weaponOBB_.size = weaponCollisionTransform_.scale;
	Matrix4x4 weaponRotateMatrix = Matrix::MakeRotateMatrix(weaponCollisionTransform_.rotate);
	SetOridentatios(weaponOBB_, weaponRotateMatrix);
	//当たり描画用の行列更新
	playerOBBScaleMatrix_.MakeScaleMatrix(playerOBB_.size);
	playerOBBTranslateMatrix_.MakeTranslateMatrix(playerOBB_.center);
	playerOBBMatrix_ = Matrix::MakeAffineMatrix(playerOBBScaleMatrix_, playerRotateMatrix_, playerOBBTranslateMatrix_);
	/*通常時かそれ以外かで武器の行列の処理を変更*/
	if (stateManager_->GetStateName() != BasePlayerState::StateName::Root) {
		Matrix4x4 weaponRotateVec = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
		if (!stateManager_->GetIsDissolve()) {
			weaponRotateVec *= (playerRotateMatrix_);
			weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_.scale, weaponRotateVec, weaponCollisionTransform_.translate);
		}
	}
	else {
		Matrix4x4 weaponRotateVec = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
		if (!stateManager_->GetIsDissolve()) {
			weaponRotateVec *= (playerRotateMatrix_);
			weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_.scale, weaponRotateVec, weaponTransform_.translate);
		}
	}
	weaponCollisionMatrix_ = Matrix::MakeAffineMatrix(weaponCollisionTransform_.scale, weaponCollisionTransform_.rotate, weaponCollisionTransform_.translate);
	//プレイヤー行列更新
	Matrix4x4 appearanceRotateMat{};
	appearanceRotateMat = Matrix::MakeRotateMatrix(playerAppearanceTransform_.rotate);
	playerMatrix_ = Matrix::MakeAffineMatrix(playerScaleMatrix_, (appearanceRotateMat * playerRotateMatrix_), playerTranslateMatrix_);
}

const float Player::GetHitStop() {
	if (stateManager_->GetStateName() == BasePlayerState::StateName::StrongAttack) {
		if (stateManager_->GetComboIndex() != 5) {
			return strongHitStop_;
		}
		else {
			return hitStop_;
		}
	}

	if (stateManager_->GetIsHitStopFlug()) {
		return strongHitStop_;
	}
	else {
		return hitStop_;
	}
}

void Player::OnCollisionEnemyAttack(){	
	//既に被弾していたら処理を飛ばす
	if (isHitEnemyAttack_)
		return;
	//カウンター判定
	if (stateManager_->GetIsGuard()){
		stateManager_->SetIsGuardHit(true);
		counterTime_ = 0;
	}
	else {	
		if (!isJustAvoid_) {
			//普通に被弾
			audio_->PlayAudio(playerHitSE_, seVolume_, false);
			isHitEnemyAttack_ = true;
			hitTimer_ = hitTimerBase_;
		}		
	}	
}

void Player::OnCollisionEnemyAttackAvoid(const uint32_t serialNumber){
	isJustAvoid_ = true;
	stateManager_->SetJustAvoidTimer();
	enemyNumber_ = serialNumber;
	GameTime::SlowDownTime(justAvoidSlowTime_, slowTimeScale_);
}

void Player::SetIsDown(bool isDown){
	isDown_ = isDown;
}

