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

	dashSpeed_ = adjustment_item->GetfloatValue(groupName, "DashSpeed");
	dashCoolTimeBase_ = adjustment_item->GetIntValue(groupName, "DashCoolTime");
	jumpPower_ = adjustment_item->GetfloatValue(groupName, "JumpPower");
	downSpeed_ = adjustment_item->GetfloatValue(groupName, "DownSpeed");
	moveSpeed_ = adjustment_item->GetfloatValue(groupName, "MoveSpeed");
	baseAttackPower_ = adjustment_item->GetIntValue(groupName, "AttackPower");
	motionSpeed_ = adjustment_item->GetfloatValue(groupName, "MotionSpeed");
	motionDistance_ = adjustment_item->GetfloatValue(groupName, "MotionDistance");
	trailPosData_ = adjustment_item->GetVector2Value(groupName, "TrailPosData");
	hitStop_ = adjustment_item->GetfloatValue(groupName, "HitStop");
	strongHitStop_ = adjustment_item->GetfloatValue(groupName, "StrongHitStop");
	addPostT_ = adjustment_item->GetfloatValue(groupName, "AddPostT");
	justAvoidT_ = adjustment_item->GetfloatValue(groupName, "JustAvoidT");
	scaleValue_ = adjustment_item->GetfloatValue(groupName, "ScaleValue");
}

void Player::Initialize(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "DashCoolTime", dashCoolTime_);
	adjustment_item->AddItem(groupName, "DashSpeed", dashSpeed_);
	adjustment_item->AddItem(groupName, "JumpPower", jumpPower_);
	adjustment_item->AddItem(groupName, "DownSpeed", downSpeed_);
	adjustment_item->AddItem(groupName, "MoveSpeed", moveSpeed_);
	adjustment_item->AddItem(groupName, "AttackPower", baseAttackPower_);
	adjustment_item->AddItem(groupName, "MotionSpeed", motionSpeed_);
	adjustment_item->AddItem(groupName, "MotionDistance", motionDistance_);
	adjustment_item->AddItem(groupName, "TrailPosData", trailPosData_);
	adjustment_item->AddItem(groupName, "HitStop", hitStop_);
	adjustment_item->AddItem(groupName, "StrongHitStop", strongHitStop_);
	adjustment_item->AddItem(groupName, "AddPostT", addPostT_);
	adjustment_item->AddItem(groupName, "JustAvoidT", justAvoidT_);
	adjustment_item->AddItem(groupName, "ScaleValue", scaleValue_);

	
	
	input_ = Input::GetInstance();

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

	playerTransform_ = playerObj_->transform_;
	playerTransform_.translate.y = 5.0f;
	playerTransform_.scale.z = 0.7f;
	playerTransform_.scale.y = 0.5f;
	particleTrans_ = playerTransform_;
	particleTransCenter_ = playerTransform_;

	weaponTransform_.scale = kWeaponScale_;
	weaponCollisionTransform_.scale = kWeaponCollisionBase_;

	dashCoolTime_ = dashCoolTimeBase_;

	playerRotateMatrix_ = Matrix::MakeIdentity4x4();

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

	isDown_ = true;

	addPosition_ = { 0.0f };

	trail_ = std::make_unique<TrailEffect>();
	trail_->Initialize(12, "resources/texture/TrailEffect/whiteTrail.png");

	trailRender_ = std::make_unique<TrailRender>();
	trailRender_->Initialize();

	audio_ = Audio::GetInstance();
	
	avoidSE_ = audio_->LoadAudio("SE/avoidSE.mp3");
	attackMotionSE_ = audio_->LoadAudio("SE/attackMotionSE.mp3");
	playerHitSE_ = audio_->LoadAudio("SE/playerHitSE.mp3");

}

void Player::Update(){
	ApplyGlobalVariables();

	if (isGuardHit_){
		trailPosData_ = trailPosDataGuard_;
	}

	//武器のディゾルブ関連
	weaponObj_->SetDissolve(weaponThreshold_);
	weaponObj_->SetTrailPos(trailPosData_);


	if (isDissolve_){
		weaponThreshold_ += addThresholdSpeed_ * timeScale_;
		if (weaponThreshold_ > 1.0f) {
			//完全に消えたら
			weaponThreshold_ = 1.0f;
			addPosition_.y = 0.0f;
			trail_->Reset();
			isDissolve_ = false;
		}
	}
	else {
		//徐々に元に戻す
		weaponThreshold_ -= minusThresholdSpeed_;
		if (weaponThreshold_ < 0.0f) {			
			weaponThreshold_ = 0.0f;
		}		
	}

	//カウンター時間経過処理
	if (counterTime_ > counterTimeBase_){
		isGuardHit_ = false;
	}

	if (isGuardHit_){
		counterTime_ += timeScale_;
	}
	
	//ジャスト回避時間経過処理
	if (justAvoidAttackTimer_ > 0) {
		if (behavior_ != Behavior::kJustAvoid){
			justAvoidAttackTimer_--;
		}
		postT_ = 0.9f;
		
	}
	else if (justAvoidAttackTimer_ <= 0) {
		
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
	
	/*リクエストに応じて変更*/
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			PreBehaviorAttackInitialize();
			BehaviorAttackInitialize();
			PostBehaviorAttackInitialize();
			break;
		case Behavior::kStrongAttack:
			BehaviorAllStrongAttackInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;		
		case Behavior::kJustAvoid:
			BehaviorJustAvoidInitialize();
			break;
		}

	}
	// 振る舞いリクエストをリセット
	behaviorRequest_ = std::nullopt;
	//値によって処理を変更
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kStrongAttack:
		BehaviorStrongAttackUpdate();
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	case Behavior::kJustAvoid:
		BehaviorJustAvoidUpdate();
		break;
	}
	//影の処理
	shadow_->position_ = playerTransform_.translate;
	shadow_->position_.y = shadowHeight_;

	shadow_->scale_.x = shadowScaleBase_ + (shadowScaleCulcBase_ - playerTransform_.translate.y);
	shadow_->scale_.y = shadowScaleBase_ + (shadowScaleCulcBase_ - playerTransform_.translate.y);

	//地面のヒビの処理
	if (isStopCrush_){
		groundCrush_->color_.w -= crushColorMinus_;

		if (groundCrush_->color_.w <= 0.0f) {
			isStopCrush_ = false;
		}
	}
	else {
		groundCrush_->color_.w = 0.0f;

		groundCrush_->position_ = playerTransform_.translate + Matrix::TransformNormal(groundOffsetBase_, playerRotateMatrix_);
		groundCrush_->position_.y = crushPosBase_;
	}
	//0以下なら固定する
	if (shadow_->scale_.x < 0.0f) {
		shadow_->scale_.x = 0.0f;
		shadow_->scale_.y = 0.0f;
	}

	/*行列やobbの更新処理*/
	playerScaleMatrix_ = Matrix::MakeScaleMatrix(playerTransform_.scale);
	playerTransformMatrix_ = Matrix::MakeTranslateMatrix(playerTransform_.translate);

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
	//プレイヤー行列更新
	playerMatrix_ = Matrix::MakeAffineMatrix(playerScaleMatrix_, playerRotateMatrix_, playerTransformMatrix_);
	//当たり描画用の行列更新
	playerOBBScaleMatrix_.MakeScaleMatrix(playerOBB_.size);
	playerOBBTransformMatrix_.MakeTranslateMatrix(playerOBB_.center);
	playerOBBMatrix_ = Matrix::MakeAffineMatrix(playerOBBScaleMatrix_, playerRotateMatrix_, playerOBBTransformMatrix_);
	/*通常時かそれ以外かで武器の行列の処理を変更*/
	if (behavior_ != Behavior::kRoot) {
		Matrix4x4 weaponRotateVec = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
		if (!isDissolve_){
			weaponRotateVec *= (playerRotateMatrix_);
			weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_.scale, weaponRotateVec, weaponCollisionTransform_.translate);
		}
	}
	else {
		Matrix4x4 weaponRotateVec = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
		if (!isDissolve_) {
			weaponRotateVec *= (playerRotateMatrix_);
			weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_.scale, weaponRotateVec, weaponTransform_.translate);
		}
	}
	weaponCollisionMatrix_= Matrix::MakeAffineMatrix(weaponCollisionTransform_.scale, weaponCollisionTransform_.rotate, weaponCollisionTransform_.translate);
	//トレイルの更新処理
	if (timeScale_ != 0.0f){
		trail_->Update();
	}
	
}

void Player::TexDraw(const Matrix4x4& viewProjection){
	shadow_->Draw(viewProjection);
	if (isStopCrush_) {
		groundCrush_->Draw(viewProjection);
	}
	trailRender_->Draw(trail_.get(), viewProjection);
}

void Player::Draw(const ViewProjection& viewProjection){
	playerObj_->SetMatrix(playerMatrix_);
	playerObj_->Update(viewProjection);
	playerObj_->Draw();

	weaponObj_->SetMatrix(weaponMatrix_);
	weaponObj_->SetShininess(shiness_);
	weaponObj_->Update(viewProjection);
	weaponObj_->Draw();

#ifdef _DEBUG
	/*collisionObj_->SetMatrix(playerOBBMatrix_);
	collisionObj_->Update(viewProjection);
	collisionObj_->Draw();*/

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
	ImGui::DragFloat("easeT", &easeT_, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("移動限界", &limitPos_.x, 0.01f);
	limitPos_.y = -limitPos_.x;
	ImGui::Text("スティックの縦 = %.4f", input_->GetPadLStick().y);
	ImGui::Text("スティックの横 = %.4f", input_->GetPadLStick().x);
	ImGui::DragFloat3("OBB座標", &obbPoint_.x, 0.01f);
	ImGui::DragFloat3("OBB大きさ", &obbAddScale_.x, 0.01f);
	ImGui::DragFloat3("自機の座標", &playerTransform_.translate.x, 0.1f);
	ImGui::Text("ロックオンしている敵 = %d", enemyNumber_);
	ImGui::Text("おちているかどうか = %d", isDown_);
	ImGui::Text("ジャスト回避中か = %d", isJustAvoid_);
	ImGui::Text("ダッシュのクールタイム = %d", dashCoolTime_);
	ImGui::Text("攻撃時間 = %d", workAttack_.AttackTimer);
	ImGui::Text("今のコンボ段階 = %d", workAttack_.comboIndex);
	ImGui::Text("敵に当たっているか = %d", isCollisionEnemy_);
	ImGui::Text("敵に攻撃に当たっているか = %d", isHitEnemyAttack_);
	ImGui::DragFloat("武器判定の回転", &weapon_Rotate_, 0.01f);
	ImGui::DragFloat3("武器の回転", &weaponTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("武器攻撃判定の回転", &weaponCollisionTransform_.rotate.x, 0.1f);	
	ImGui::DragFloat3("オフセットのベース", &weapon_offset_Base_.x, 0.1f);
	ImGui::DragFloat3("オフセット", &weapon_offset_.x, 0.1f);
	ImGui::DragFloat("武器の反射", &shiness_, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat3("武器の高さ補正", &addPosition_.x, 0.01f);
	ImGui::Checkbox("武器のしきい値", &isDissolve_);
	ImGui::Checkbox("ガード中かどうか", &isGuard_);
	ImGui::Checkbox("ガード中に攻撃受けたかどうか", &isGuardHit_);
	ImGui::Checkbox("武器の振りの調整", &isWeaponDebugFlug_);
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
}

void Player::OnCollisionEnemyAttack(){
	
	//既に被弾していたら処理を飛ばす
	if (isHitEnemyAttack_)
		return;
	//カウンター判定
	if (isGuard_){
		isGuardHit_ = true;
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
	justAvoidAttackTimer_ = justAvoidAttackTimerBase_;
	enemyNumber_ = serialNumber;
	GameTime::SlowDownTime(justAvoidSlowTime_, slowTimeScale_);
}

void Player::SetIsDown(bool isDown){
	
	isDown_ = isDown;
}

void Player::BehaviorRootInitialize(){
	isDash_ = false;
	GameTime::ReverseTimeChange();
	move_ = { 0.0f,0.0f,0.0f };
	workAttack_.comboIndex = 0;
	
	weaponCollisionTransform_.translate.y = kWeaponRootTranslate_;
}

void Player::BehaviorRootUpdate(){
	frontVec_ = postureVec_;
	/*自機の移動*/	
	move_.z = input_->GetPadLStick().y * moveSpeed_;
	if (abs(move_.z) < moveLimitMinimum_) {
		move_.z = 0;
	}	
	move_.x = input_->GetPadLStick().x * moveSpeed_;
	if (abs(move_.x) < moveLimitMinimum_) {
		move_.x = 0;
	}
	//カメラによってmoveの値を補正
	if (viewProjection_){

		Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(viewProjection_->rotation_);
		move_ = Matrix::TransformNormal(move_, newRotateMatrix);
		move_.y = 0.0f;
	}
	
	move_ = Vector3::Mutiply(Vector3::Normalize(move_), moveSpeed_ * moveCorrection_);
	move_.y = 0.0f;
	
	if (move_.x != 0.0f || move_.z != 0.0f) {
		//移動ベクトルがあれば
		postureVec_ = move_;
		
		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::Multiply(playerRotateMatrix_, directionTodirection_);
		
	}
	else if(lockOn_&&lockOn_->ExistTarget()){
		//対象がいれば注目する
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPos - playerTransform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		postureVec_ = sub;

		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::Multiply(playerRotateMatrix_, directionTodirection_);
	}
	//Aを押してジャンプ
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A) && !isDown_) {
		downVector_.y += jumpPower_ * timeScale_;
		
	}
	//次のフレームを仮に計算し制御
	Vector3 NextPos = playerTransform_.translate + move_;

	if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y){
		move_.x = 0;
	}
	if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
		move_.z = 0;
	}

	playerTransform_.translate += move_ * timeScale_;
	
	if (isDown_) {
		downVector_.y += downSpeed_ * timeScale_;
	}
	
	if (dashCoolTime_ != 0) {
		dashCoolTime_--;
	}
	/*武器の浮遊*/
	floatSin_ += floatSpeed_ * timeScale_;
	if (floatSin_ >= (std::numbers::pi * 2.0f)) {
		floatSin_ = 0.0f;
	}
	playerTransform_.translate.y += downVector_.y * timeScale_;
	//武器の消滅処理をしていなければ武器のSRTを更新
	if (!isDissolve_ ) {

		if (!isWeaponDebugFlug_) {
			addPosition_.y = kFloatHeight_ * std::sinf(floatSin_);
			weapon_offset_Base_ = weapon_offset_RootBase_;

			weaponTransform_.rotate.x = (float)(std::numbers::pi);
			weaponTransform_.rotate.y = 0;
			weaponTransform_.rotate.z = 0;
		}
		weaponCollisionTransform_.scale = kWeaponCollisionBase_;

		Vector3 weaponRotateVec = weaponTransform_.rotate;
		Matrix4x4 weaponRotateMatrix = Matrix::MakeRotateMatrix(weaponRotateVec);
		weaponRotateMatrix = Matrix::Multiply(weaponRotateMatrix, playerRotateMatrix_);
		weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponRotateMatrix);
		weaponTransform_.translate = playerTransform_.translate + weapon_offset_;
		Vector3 addPosRotate{};
		addPosRotate = Matrix::TransformNormal(addPosition_, playerRotateMatrix_);

		weaponTransform_.translate += addPosRotate * timeScale_;
	}
	
	if (timeScale_ != 0.0f) {
		trail_->SetPos(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	}
	//ダッシュを発動
	if (input_->GetPadButtonTriger(Input::GamePad::RB) && dashCoolTime_ <= 0) {
		behaviorRequest_ = Behavior::kDash;
	}
	//弱攻撃を発動
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_X)) {
		audio_->PlayAudio(attackMotionSE_, seVolume_, false);
		workAttack_.comboIndex = 1;
		behaviorRequest_ = Behavior::kAttack;
		isDissolve_ = false;
		weaponThreshold_ = 0.0f;
	}
	//強攻撃を発動
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_Y)) {
		workAttack_.comboIndex = 1;
		behaviorRequest_ = Behavior::kStrongAttack;
		isDissolve_ = false;
		weaponThreshold_ = 0.0f;
	}
	
}

void Player::PreBehaviorAttackInitialize(){
	trail_->Reset();
	workAttack_.comboNext = false;
	workAttack_.strongComboNext = false;
	workAttack_.attackParameter = 0;
	if (workAttack_.comboIndex != 4) {
		weaponTransform_.translate = playerTransform_.translate;
		weapon_offset_Base_ = { 0.0f,0.0f,0.0f };
	}
	weapon_offset_Base_.y = kWeapon_offset_;
}

void Player::PostBehaviorAttackInitialize(){
	if (workAttack_.comboIndex != 4) {
		Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
		weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
		weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;
		weaponCollisionTransform_.rotate = { 0.0f,0.0f,weaponTransform_.rotate.z };
		weaponCollisionTransform_.scale = kWeaponCollisionBase_;
		isShakeDown_ = false;
	}
	else{
		isShakeDown_ = true;
	}
	workAttack_.AttackTimer = 0;
	hitRecord_.Clear();
	waitTime_ = waitTimeBase_;

	
	isEndAttack_ = false;
}

void Player::BehaviorAttackInitialize(){
	type_ = KnockbackType::Left;
	workAttack_.nextAttackTimer = nextAttackTimerFirst_;
	//baseRotate_.y = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = weaponAttackTransformRotates_[0].x;
	weaponTransform_.rotate.z = weaponAttackTransformRotates_[0].z;

	weapon_Rotate_ = weapon_Rotates_[0];
	
}

void Player::BehaviorSecondAttackInitialize(){
	type_ = KnockbackType::Right;

	workAttack_.nextAttackTimer = nextAttackTimer_;

	weaponTransform_.rotate = weaponAttackTransformRotates_[1];

	weapon_Rotate_ = weapon_Rotates_[1];
}

void Player::BehaviorThirdAttackInitialize(){
	type_ = KnockbackType::Left;

	workAttack_.nextAttackTimer = nextAttackTimer_;

	weaponTransform_.rotate = weaponAttackTransformRotates_[2];

	weapon_Rotate_ = weapon_Rotates_[2];
}

void Player::BehaviorFourthAttackInitialize(){
	type_ = KnockbackType::Left;
	workAttack_.nextAttackTimer = nextAttackTimer_;
	weaponTransform_.rotate.z = weaponAttackTransformRotates_[3].z;

}

void Player::BehaviorFifthAttackInitialize(){
	type_ = KnockbackType::Right;
	workAttack_.nextAttackTimer = nextAttackTimer_;
	weaponTransform_.rotate = weaponAttackTransformRotates_[4];

	weapon_Rotate_ = weapon_Rotates_[4];
}

void Player::BehaviorSixthAttackInitialize(){
	type_ = KnockbackType::Strong;
	workAttack_.nextAttackTimer = nextAttackTimer_;
	weaponTransform_.rotate = weaponAttackTransformRotates_[5];

	weapon_Rotate_ = weapon_Rotates_[5];
}

void Player::BehaviorAttackUpdate(){
	frontVec_ = postureVec_;
	//技が終わったか
	if (isEndAttack_) {
		//次の攻撃を発動するか
		if (workAttack_.comboNext) {
			workAttack_.comboIndex++;
			//攻撃の方向を決定
			if (input_->GetPadLStick().x != 0 || input_->GetPadLStick().y != 0) {
				postureVec_ = { input_->GetPadLStick().x ,0,input_->GetPadLStick().y };
				Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(viewProjection_->rotation_);
				postureVec_ = Matrix::TransformNormal(postureVec_, newRotateMatrix);
				postureVec_.y = 0;
				postureVec_ = Vector3::Normalize(postureVec_);
				Matrix4x4 directionTodirection_;
				directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
				playerRotateMatrix_ = Matrix::Multiply(playerRotateMatrix_, directionTodirection_);

			}
			else if (lockOn_ && lockOn_->ExistTarget()) {
				Vector3 lockOnPos = lockOn_->GetTargetPosition();
				Vector3 sub = lockOnPos - playerTransform_.translate;
				sub.y = 0;
				sub = Vector3::Normalize(sub);
				postureVec_ = sub;


				Matrix4x4 directionTodirection_;
				directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
				playerRotateMatrix_ = Matrix::Multiply(playerRotateMatrix_, directionTodirection_);


			}
			//各攻撃行動の初期化処理

			PreBehaviorAttackInitialize();
			if (workAttack_.comboIndex == 1) {
				
				BehaviorAttackInitialize();
			}
			else if (workAttack_.comboIndex == 2) {
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				BehaviorSecondAttackInitialize();
			}
			else if (workAttack_.comboIndex == 3) {
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				BehaviorThirdAttackInitialize();
			}
			else if (workAttack_.comboIndex == 4) {
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				BehaviorFourthAttackInitialize();
			}
			else if (workAttack_.comboIndex == 5) {
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);

				BehaviorFifthAttackInitialize();
			}
			else if (workAttack_.comboIndex == 6) {

				BehaviorSixthAttackInitialize();
			}
			PostBehaviorAttackInitialize();
		}
		else {
			//強攻撃を行うか
			if (workAttack_.strongComboNext) {
				workAttack_.comboIndex++;
				behaviorRequest_ = Behavior::kStrongAttack;
			}
			else {
				workAttack_.attackParameter += timeScale_;
				//通常状態に戻る処理
				if (workAttack_.attackParameter >= ((float)(workAttack_.nextAttackTimer + motionDistance_) / motionSpeed_)) {
					behaviorRequest_ = Behavior::kRoot;
					isDissolve_ = true;
					workAttack_.attackParameter = 0;
				}
				//スティックを倒しているときは少しだけ早く抜けるように
				if (input_->GetIsPushedLStick() and (workAttack_.attackParameter * kAttackParameterCorection_ >= ((float)(workAttack_.nextAttackTimer) / motionSpeed_))) {
					behaviorRequest_ = Behavior::kRoot;
					isDissolve_ = true;
					workAttack_.attackParameter = 0;
				}
			}

		}
		
	}
	
	//強攻撃に派生する場合はここで帰る
	if (behaviorRequest_==Behavior::kStrongAttack){
		return;
	}
	//各行動の処理
	switch (workAttack_.comboIndex){
	case 1:
		AttackMotion();
		break;
	case 2:		
		SecondAttackMotion();
		break;
	case 3:	
		ThirdAttackMotion();
		break;
	case 4:
		FourthAttackMotion();
		break;
	case 5:
		FifthAttackMotion();
		
		break;
	case 6:
		SixthAttackMotion();

		break;
	default:
		break;
	}
	//コンボ上限に達していない
	if (workAttack_.comboIndex< conboNum_) {
		if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_X)) {
			//コンボ有効
			workAttack_.comboNext= true;
		}
		else if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_Y)) {
			//強コンボ派生有効
			workAttack_.strongComboNext= true;
		}
	}
	downVector_.y += downSpeed_;

	playerTransform_.translate.y += downVector_.y * timeScale_;
	
	//コリジョン用のtransform等を更新
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weaponCollisionRotateMatrix = Matrix::Multiply(weaponCollisionRotateMatrix, playerRotateMatrix_);
	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;
	weaponCollisionTransform_.translate.y += (addPosition_.y + 1.0f);

	weaponTransform_.translate = weaponCollisionTransform_.translate;

	if (workAttack_.comboIndex != 0) {
		//トレイルのために少し早く情報を更新
		weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_.scale, weaponCollisionRotateMatrix, weaponCollisionTransform_.translate);
		weaponObj_->SetMatrix(weaponMatrix_);
		weaponObj_->UniqueUpdate();
		if (timeScale_ != 0.0f){
			trail_->SetPos(weaponObj_->GetTopAndTailVerTex().head, weaponObj_->GetTopAndTailVerTex().tail);
		}
		
	}
	
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_RIGHT_SHOULDER) && dashCoolTime_ <= 0) {
		behaviorRequest_ = Behavior::kDash;
	}

	
}

void Player::BehaviorAllStrongAttackInitialize(){
	endCombo_ = false;
	chargeEnd_ = false;

	weapon_offset_Base_ = { 0.0f,0.0f,0.0f };
	//攻撃方向の指定
	if (input_->GetPadLStick().x != 0 || input_->GetPadLStick().y != 0) {
		postureVec_ = { input_->GetPadLStick().x ,0,input_->GetPadLStick().y };
		Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(viewProjection_->rotation_);
		postureVec_ = Matrix::TransformNormal(postureVec_, newRotateMatrix);
		postureVec_.y = 0;
		postureVec_ = Vector3::Normalize(postureVec_);

		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::Multiply(playerRotateMatrix_, directionTodirection_);

	}
	else if (lockOn_ && lockOn_->ExistTarget()) {
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPos - playerTransform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		postureVec_ = sub;

		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::Multiply(playerRotateMatrix_, directionTodirection_);

	}
	//各強攻撃の初期化
	PreBehaviorStrongAttackInitialize();

	if (workAttack_.comboIndex == 1) {
		BehaviorStrongAttackInitialize();		
	}
	else if (workAttack_.comboIndex == 2) {
		BehaviorSecondStrongAttackInitialize();
	}
	else if (workAttack_.comboIndex == 3) {
		BehaviorThirdStrongAttackInitialize();
	}
	else if (workAttack_.comboIndex == 4) {
		BehaviorFourthStrongAttackInitialize();
	}
	else if (workAttack_.comboIndex == 5) {
		BehaviorFifthStrongAttackInitialize();
		
	}
	else if (workAttack_.comboIndex == 6) {
		BehaviorSixthStrongAttackInitialize();

	}

	PostBehaviorStrongAttackInitialize();

	particle_->SetIsDraw(true);
	particle_->AddParticle(emitter_);
}

void Player::BehaviorStrongAttackUpdate(){
	frontVec_ = postureVec_;
	//攻撃が終わったら通常状態へ
	if (isEndAttack_) {
		workAttack_.attackParameter += timeScale_;
		if (workAttack_.attackParameter >= ((float)(workAttack_.nextAttackTimer + motionDistance_) / motionSpeed_)) {
			isDissolve_ = true;
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.attackParameter = 0;
		}	
	}

	//各強攻撃の更新
	switch (workAttack_.comboIndex) {
	case 1:
		StrongAttackMotion();
		break;
	case 2:
		SecondStrongAttackMotion();
		break;
	case 3:
		ThirdStrongAttackMotion();
		break;
	case 4:
		FourthStrongAttackMotion();
		break;
	case 5:
		FifthStrongAttackMotion();
		break;
	case 6:
		SixthStrongAttackMotion();
		break;
	default:
		break;
	}
	//SRT等情報を更新
	downVector_.y += downSpeed_;
	playerTransform_.translate.y += downVector_.y;
	weaponTransform_.translate = playerTransform_.translate;
	weaponTransform_.translate.y += addPosition_.y;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weaponCollisionRotateMatrix = Matrix::Multiply(weaponCollisionRotateMatrix, playerRotateMatrix_);
	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;
	weaponCollisionTransform_.translate.y += addPosition_.y;
	//バグって入ってしまった時対策
	if (workAttack_.comboIndex != 0) {
		//トレイルの更新
		weaponCollisionTransform_.translate.y += 1.0f;
		weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_.scale, weaponCollisionRotateMatrix, weaponCollisionTransform_.translate);
		weaponObj_->SetMatrix(weaponMatrix_);
		weaponObj_->UniqueUpdate();
		if (timeScale_ != 0.0f) {
			if (isTrail_) {
				if (workAttack_.comboIndex == 1) {
					trail_->SetPos(weaponObj_->GetTopAndTailVerTex().head, weaponObj_->GetTopAndTailVerTex().tail);
				}
				else {
					trail_->SetPos(weaponObj_->GetTopAndTailVerTex().head, weaponObj_->GetTopAndTailVerTex().tail);
				}
			}
			else {
				trail_->SetPos(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
			}
		}
	}
	//ダッシュが入力されたら即中断
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_RIGHT_SHOULDER) && dashCoolTime_ <= 0) {
		behaviorRequest_ = Behavior::kDash;
	}

}

void Player::BehaviorDashInitialize(){
	isAvoidAttack_ = false;
	audio_->PlayAudio(avoidSE_, seVolume_, false);

	workDash_.dashParameter_ = 0;

	isDash_ = true;
}

void Player::BehaviorJustAvoidInitialize(){
	//isJustAvoid_ = false;
	isAvoidAttack_ = false;
	type_ = KnockbackType::Center;
	/*移動部分の初期化*/
	//ターゲットに向かって移動する
	
	Vector3 lockOnPos = lockOn_->GetTargetPosition();
	Vector3 sub = lockOnPos - playerTransform_.translate;
	sub.y = 0;
	Vector3 subNorm = Vector3::Normalize(sub);
	postureVec_ = subNorm;

	Matrix4x4 directionTodirection_;
	directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
	playerRotateMatrix_ = Matrix::Multiply(playerRotateMatrix_, directionTodirection_);
	move_ = subNorm * workAvoidAttack_.tackleSpeed_;
	move_.y = 0;

	workAvoidAttack_.tackleTimer_ = 0.0f;
	workAvoidAttack_.isChangeEndAttack_ = false;

	/*武器や攻撃関連の初期化*/
	/*剣を引いて突くような動作*/
	workAttack_.attackParameter = 0;
	weaponTransform_.rotate = justAvoidWeaponRotate_;
	weaponTransform_.translate = playerTransform_.translate;
	weapon_offset_Base_ = { 0.0f,0.0f,0.0f };
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;
	hitRecord_.Clear();
	workAttack_.AttackTimer = 0;
	easeT_ = 1.0f;
	addEaseT_ = addEaseSpeed_;
	waitTime_ = waitTimeBase_;
	addPosition_.y = 0.0f;
	trail_->Reset();
	strongSecondAttackCount_ = 0;
	isNextAttack_ = false;
	isTrail_ = false;
	isThrust_ = false;
	isShakeDown_ = false;
	isEndAttack_ = false;
	weaponCollisionTransform_.scale = { 0.0f,0.0f,0.0f };
}

void Player::BehaviorDashUpdate(){
	frontVec_ = postureVec_;
	Matrix4x4 newRotateMatrix_ = playerRotateMatrix_;
	move_ = { 0, 0, moveSpeed_ * dashSpeed_ };

	move_ = Matrix::TransformNormal(move_, newRotateMatrix_);
	//ジャスト回避していたら追撃ができるように
	if (isJustAvoid_) {
		if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_X)) {
			

			behaviorRequest_ = Behavior::kJustAvoid;
			dashCoolTime_ = dashCoolTimeBase_;
		}
	}
	
	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 15;

	if (!isAvoidAttack_) {
		Vector3 NextPos = playerTransform_.translate + (move_ * timeScale_);

		if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
			move_.x = 0;
		}
		if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
			move_.z = 0;
		}
		playerTransform_.translate += move_ * timeScale_;
	}
	

	//既定の時間経過で通常状態に戻る
	workDash_.dashParameter_ += timeScale_;
	if (workDash_.dashParameter_ >= behaviorDashTime) {
		dashCoolTime_ = dashCoolTimeBase_;
		isDash_ = false;
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorJustAvoidUpdate(){
	frontVec_ = postureVec_;
	if (!isAvoidAttack_){

		if (!isThrust_){	
			//突く前の予備動作
			easeT_ -= (addEaseT_ * justAvoidEaseMagnification_.x) ;
			if (easeT_ < 0.0f) {
				easeT_ = 0.0f;
				isThrust_ = true;
			}
			
		}
		else {
			//突きのeaseの更新
			easeT_ += (addEaseT_ * justAvoidEaseMagnification_.y);
			if (easeT_ > 1.0f) {
				//構え終わったら攻撃に移る
				GameTime::ReverseTimeChange();
				easeT_ = 1.0f;
				waitTime_ -= 1;
				addEaseT_ = 0.0f;
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				justAvoidAttackTimer_ = 0;
				isJustAvoid_ = false;
				isAvoidAttack_ = true;
			}

		}
		
		weapon_offset_Base_.x = Ease::Easing(Ease::EaseName::EaseInExpo, justAvoidEaseStart_.x, justAvoidEaseEnd_.x, easeT_);
		weapon_offset_Base_.y = Ease::Easing(Ease::EaseName::EaseInExpo, justAvoidEaseStart_.y, justAvoidEaseEnd_.y, easeT_);
		
	}
	else {
		//突き攻撃の動作処理
		weaponCollisionTransform_.scale = kWeaponCollisionBase_;
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPos - playerTransform_.translate;

		Vector3 NextPos = playerTransform_.translate + (move_);

		if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
			move_.x = 0;
		}
		if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
			move_.z = 0;
		}

		workAvoidAttack_.tackleHitTimer_ += GameTime::deltaTime_;
		//連続ヒットの調整
		if (workAvoidAttack_.tackleHitTimer_ > workAvoidAttack_.tackleHitTimerBase_){
			workAvoidAttack_.tackleHitTimer_ = 0.0f;
			hitRecord_.Clear();
		}

		playerTransform_.translate += move_;

		workAvoidAttack_.tackleTimer_ += GameTime::deltaTime_;

		

		if (workAvoidAttack_.tackleTimerBase_ < workAvoidAttack_.tackleTimer_) {
			behaviorRequest_ = Behavior::kRoot;
		}
	}
	//座標や回転の更新
	Matrix4x4 directionTodirection_;
	directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
	playerRotateMatrix_ = Matrix::Multiply(playerRotateMatrix_, directionTodirection_);

	
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weaponCollisionRotateMatrix = Matrix::Multiply(weaponCollisionRotateMatrix, playerRotateMatrix_);
	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;
	weaponCollisionTransform_.translate.y += (addPosition_.y + 1.0f);

	weaponTransform_.translate = weaponCollisionTransform_.translate;
		
}

void Player::AttackMotion(){
	//振っている現在の角度で処理を変更
	if (weapon_Rotate_ >= weapon_RotatesMinMax_[0].x) {
		waitTime_ -= timeScale_;
		weapon_Rotate_ = weapon_RotatesMinMax_[0].x * kAttackMagnification_;
	}
	else if (weapon_Rotate_ <= weapon_RotatesMinMax_[0].y) {
		isShakeDown_ = true;
	}
	else {
		//攻撃時に移動
		move_ = { 0.0f,0.0f,moveSpeed_ * kAttackMagnification_ };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
				move_.x = 0;
			}
			if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
				move_.z = 0;
			}

			playerTransform_.translate += (move_ * motionSpeed_) * timeScale_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer += timeScale_;
	}
	//動作が終わったら待機
	if (waitTime_ <= 0) {
		isEndAttack_ = true;
	}
	//フラグによって振る方向を分岐
	if (!isShakeDown_) {
		weapon_Rotate_ -= (kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
	}
	else if (isShakeDown_) {

		weapon_Rotate_ += (kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_) * timeScale_;
	}


	weaponTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
}

void Player::SecondAttackMotion(){	
	//振っている現在の角度で処理を変更
	if (weapon_Rotate_ >= weapon_RotatesMinMax_[1].x) {
		waitTime_ -= timeScale_;
		weapon_Rotate_ = weapon_RotatesMinMax_[1].x;
	}
	else if (weapon_Rotate_ <= weapon_RotatesMinMax_[1].y) {
		isShakeDown_ = true;
	}
	else {
		//攻撃時に移動
		move_ = { 0.0f,0.0f,moveSpeed_ * kAttackMagnification_ };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
				move_.x = 0;
			}
			if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
				move_.z = 0;
			}

			playerTransform_.translate += (move_ * motionSpeed_) * timeScale_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer += timeScale_;
	}

	if (waitTime_ <= 0) {
		isEndAttack_ = true;
	}
	//フラグによって振る方向を分岐
	if (!isShakeDown_) {
		weapon_Rotate_ -= (kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_) ;
	}
	else if (isShakeDown_) {
		
		weapon_Rotate_ += (kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_) * timeScale_;
	}
	

	weaponTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;

}

void Player::ThirdAttackMotion(){	
	//振っている現在の角度で処理を変更
	if (weapon_Rotate_ >= weapon_RotatesMinMax_[2].x) {
		waitTime_ -= timeScale_;
		weapon_Rotate_ = weapon_RotatesMinMax_[2].x;
	}
	else if (weapon_Rotate_ <= weapon_RotatesMinMax_[2].y) {
		isShakeDown_ = true;
	}
	else {
		//攻撃時に移動
		move_ = { 0.0f,0.0f,moveSpeed_ * kAttackMagnification_ };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
				move_.x = 0;
			}
			if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
				move_.z = 0;
			}

			playerTransform_.translate += (move_ * motionSpeed_) * timeScale_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer += timeScale_;
	}

	if (waitTime_ <= 0) {
		isEndAttack_ = true;
	}
	//フラグによって振る方向を分岐
	if (!isShakeDown_) {
		weapon_Rotate_ -= (kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
	}
	else if (isShakeDown_) {

		weapon_Rotate_ += (kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_) * timeScale_;
	}


	weaponTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
}

void Player::FourthAttackMotion(){
	//振っている現在の角度で処理を変更
	if (weapon_Rotate_ >= weapon_RotatesMinMax_[3].x) {
		waitTime_ -= timeScale_;
		weapon_Rotate_ = weapon_RotatesMinMax_[3].x;
	}	
	else {
		//攻撃時に移動
		move_ = { 0.0f,0.0f,moveSpeed_ * kAttackMagnification_ };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
				move_.x = 0;
			}
			if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
				move_.z = 0;
			}

			playerTransform_.translate += (move_ * motionSpeed_) * timeScale_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer += timeScale_;
	}

	if (waitTime_ <= 0) {
		isEndAttack_ = true;
	}
	//フラグによって振る方向を分岐
	if (!isShakeDown_) {
		weapon_Rotate_ -= (kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
	}
	else if (isShakeDown_) {

		weapon_Rotate_ += (kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_) * timeScale_;
	}


	weaponTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
}

void Player::FifthAttackMotion(){
	//振っている現在の角度で処理を変更
	if (weapon_Rotate_ >= weapon_RotatesMinMax_[4].x) {
		waitTime_ -= timeScale_;
		weapon_Rotate_ = weapon_RotatesMinMax_[4].x;
	}
	else if (weapon_Rotate_ <= weapon_RotatesMinMax_[4].y) {
		isShakeDown_ = true;
	}
	else {
		//攻撃時に移動
		move_ = { 0.0f,0.0f,moveSpeed_ * kAttackMagnification_ };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
				move_.x = 0;
			}
			if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
				move_.z = 0;
			}

			playerTransform_.translate += (move_ * motionSpeed_) * timeScale_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer += timeScale_;
	}

	if (waitTime_ <= 0) {
		isEndAttack_ = true;
	}
	//フラグによって振る方向を分岐
	if (!isShakeDown_) {
		weapon_Rotate_ -= (kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
	}
	else if (isShakeDown_) {

		weapon_Rotate_ += (kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_) * timeScale_;
	}


	weaponTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
	
}

void Player::SixthAttackMotion(){
	//振っている現在の角度で処理を変更
	if (weapon_Rotate_ >= weapon_RotatesMinMax_[5].x) {
		waitTime_ -= timeScale_;
		weapon_Rotate_ = weapon_RotatesMinMax_[5].x;
		SettingGroundCrushTex();
	}
	else if (weapon_Rotate_ <= weapon_RotatesMinMax_[5].y) {
		audio_->PlayAudio(attackMotionSE_, seVolume_, false);
		isShakeDown_ = true;
	}
	else {
		//攻撃時に移動
		move_ = { 0.0f,0.0f,moveSpeed_ * kAttackMagnification_ };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
				move_.x = 0;
			}
			if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
				move_.z = 0;
			}

			playerTransform_.translate += (move_ * motionSpeed_) * timeScale_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer += timeScale_;
	}

	if (waitTime_ <= 0) {
		isEndAttack_ = true;
	}
	//フラグによって振る方向を分岐
	if (!isShakeDown_) {
		weapon_Rotate_ -= (kMoveWeapon_ * motionSpeed_ / (kAttackDivisionMagnification_ * kAttackMagnification_));
	}
	else if (isShakeDown_) {

		weapon_Rotate_ += (kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_) * timeScale_;
	}


	weaponTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
}

void Player::PreBehaviorStrongAttackInitialize(){
	trail_->Reset();
	workAttack_.comboNext = false;
	workAttack_.strongComboNext = false;
	workAttack_.attackParameter = 0;
	weaponTransform_.translate = playerTransform_.translate;

	workAttack_.AttackTimer = 0;
	hitRecord_.Clear();
	waitTime_ = waitTimeBase_;
	isEndAttack_ = false;

}

void Player::PostBehaviorStrongAttackInitialize(){

}

void Player::BehaviorStrongAttackInitialize(){
	type_ = KnockbackType::Center;
	workAttack_.nextAttackTimer = nextAttackTimer_;
	weaponTransform_.rotate = weaponStrongAttackTransformRotates_[0];
	weapon_offset_Base_ = weaponStrongAttackOffset_[0];
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;


	weapon_Rotate_ = 1.0f;
	isTrail_ = false;
	isShakeDown_ = true;
	chargeEnd_ = false;
}

void Player::BehaviorSecondStrongAttackInitialize(){

	weaponTransform_.rotate = weaponStrongAttackTransformRotates_[1];
	weapon_offset_Base_ = weaponStrongAttackOffset_[1];
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;
	easeT_ = 0;
	addEaseT_ = addEaseSpeedStrong_;
	waitTime_ = waitTimeBase_ * kAttackDivisionMagnification_;
	addPosition_.y = 0.0f;
	strongSecondAttackCount_ = 0;
	isNextAttack_ = false;
	isTrail_ = false;
	isShakeDown_ = false;
}

void Player::BehaviorThirdStrongAttackInitialize(){
	type_ = KnockbackType::Strong;
	workAttack_.nextAttackTimer = nextAttackTimer_;
	//baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate = weaponStrongAttackTransformRotates_[2];
	weapon_offset_Base_ = weaponStrongAttackOffset_[2];
	//addPosition_.y = 0.5f;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;
	

	weapon_Rotate_ = -0.0f;
	isTrail_ = false;
	isShakeDown_ = false;
}

void Player::BehaviorFourthStrongAttackInitialize(){
	workAttack_.nextAttackTimer = nextAttackTimer_;
	//baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate = weaponStrongAttackTransformRotates_[3];
	weapon_offset_Base_.y = weaponStrongAttackOffset_[3].y;
	//addPosition_.y = 0.5f;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;
	weaponCollisionTransform_.scale = kWeaponCollisionBase_ * strongAddScale_;

	weapon_Rotate_ = -0.0f;
	isTrail_ = true;
	isShakeDown_ = false;
}

void Player::BehaviorFifthStrongAttackInitialize(){
	
	type_ = KnockbackType::Few;
	///
	workAttack_.nextAttackTimer = nextAttackTimer_;
	//baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate = weaponStrongAttackTransformRotates_[4];
	weapon_offset_Base_ = weaponStrongAttackOffset_[4];

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;
	weaponCollisionTransform_.scale = kWeaponCollisionBase_ * strongAddScale_;

	weapon_Rotate_ = fifthWeapon_Rotate_;
	strongSecondAttackCount_ = 0;
	isFirstAttack_ = true;
	isNextAttack_ = false;
	isFinishAttack_ = false;
	isTrail_ = false;
	isShakeDown_ = false;
}

void Player::BehaviorSixthStrongAttackInitialize(){
	workAttack_.nextAttackTimer = nextAttackTimer_;
	//baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate = weaponStrongAttackTransformRotates_[5];
	weapon_offset_Base_ = weaponStrongAttackOffset_[5];
	addPosition_.y = 0.0f;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	weaponCollisionTransform_.translate = playerTransform_.translate + weapon_offset_;
	weaponCollisionTransform_.scale = kWeaponCollisionBase_ * strongAddScale_;

	easeT_ = 0;
	addEaseT_ = addEaseSpeed_;

	weapon_Rotate_ = 1.0f;
	isTrail_ = false;
	isShakeDown_ = true;
	chargeEnd_ = false;
}

void Player::StrongAttackMotion(){
	
	if (!chargeEnd_){
		//長押ししている間は構え
		if (input_->GetPadButton(XINPUT_GAMEPAD_Y)){
			isGuard_ = true;
			weaponCollisionTransform_.scale = collsionScaleGuade_;
		}
		//ボタンを離すかガード中に攻撃がヒットした場合派生
		if (input_->GetPadButtonRelease(XINPUT_GAMEPAD_Y) or isGuardHit_){
			type_ = HitRecord::Strong;
			isGuard_ = false;
			chargeEnd_ = true;
			weapon_offset_Base_ = weaponStrongAttackOffset_[2];
			weaponTransform_.rotate.z = strongAttackRotateZ_;
			if (isGuardHit_) {
				counterScale_ = kWeaponCollisionBase_ * kGuadeMagnification_;
			}
			else {
				counterScale_ = kWeaponCollisionBase_;
			}
		}

	}
	else {
		//攻撃動作
		//限界まで振ったら終わり
		if (weapon_Rotate_ >= weapon_StrongRotatesMinMax_[0].y) {
			waitTime_ -= 1;
			weapon_Rotate_ = weapon_StrongRotatesMinMax_[0].y;
		}
		else {

			//移動
			move_ = { 0.0f,0.0f,moveSpeed_ * kAttackMagnification_ };
			move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

			if (!isCollisionEnemy_) {
				Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

				if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
					move_.x = 0;
				}
				if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
					move_.z = 0;
				}

				playerTransform_.translate += move_ * motionSpeed_ * timeScale_;
			}
			weaponTransform_.translate = playerTransform_.translate;
			
		
		}

		if (waitTime_ <= 0) {		
			isEndAttack_ = true;
		}

		if (isShakeDown_) {
			//角度によってトレイルや当たり判定が働くように
			if (weapon_Rotate_ < weapon_StrongRotatesMinMax_[0].x){
				isTrail_ = false;
				weaponCollisionTransform_.scale = { 0.0f,0.0f,0.0f };
				weapon_Rotate_+= kMoveWeaponShakeDown_ / kAttackDivisionMagnification_ * motionSpeed_ * timeScale_;
			}
			else {		
				if (!isTrail_ and workAttack_.comboIndex != 2) {
					audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				};
				weaponCollisionTransform_.scale = counterScale_;
				isTrail_ = true;
				weapon_Rotate_ += kMoveWeaponShakeDown_ * kStrongAttackMagnification_ * motionSpeed_ * timeScale_;
			}

		}

		weaponTransform_.rotate.x = weapon_Rotate_;
		weaponCollisionTransform_.rotate.x = weapon_Rotate_;
		
	}
	
	
}

void Player::SecondStrongAttackMotion(){
	//突き攻撃
	easeT_ += addEaseT_;
	if (easeT_ > 1.0f) {
		easeT_ = 1.0f;
		addEaseT_ = 0.0f;
		audio_->PlayAudio(attackMotionSE_, seVolume_, false);
	}

	if (easeT_ == 1.0f) {
		waitTime_ -= 1.0f;
	}
	//追加攻撃入力受付時間
	if (easeT_ >= easeSecondStrong_){
		if (strongSecondAttackCount_ < kStrongSecondAttackCountMax_) {

			if (input_->GetPadButtonTriger(Input::GamePad::Y)) {
				isNextAttack_ = true;
			}
		}
	}
	
	if (waitTime_ <= 0) {
		//フラグがたっていたら追加攻撃
		if (isNextAttack_ && !isEndAttack_) {
			waitTime_ = waitTimeBase_ * kAttackDivisionMagnification_;
			easeT_ = 0;
			addEaseT_ = addEaseSpeedStrong_;
			strongSecondAttackCount_++;
			isNextAttack_ = false;
		}
		else{
			isEndAttack_ = true;
		}
		
	}
	weapon_offset_Base_.x = Ease::Easing(Ease::EaseName::EaseInBack, -0.5f, 0.0f, easeT_);
	weapon_offset_Base_.y = Ease::Easing(Ease::EaseName::EaseInBack, 0.0f, 4.0f, easeT_);

		
}

void Player::ThirdStrongAttackMotion(){
	//シンプルな縦振り
	//限度によって行う処理を変更
	if (weapon_Rotate_ >= weapon_StrongRotatesMinMax_[2].y) {
		//振り切った後
		waitTime_ -= 1;
		weapon_Rotate_ = 1.35f;
		SettingGroundCrushTex();
	}
	else if (weapon_Rotate_ <= weapon_StrongRotatesMinMax_[2].x) {
		//振りかぶった後
		audio_->PlayAudio(attackMotionSE_, seVolume_, false);
		isShakeDown_ = true;
		isTrail_ = true;
	}
	

	if (waitTime_ <= 0) {
		isEndAttack_ = true;
	}
	//振り下ろしているかどうかで処理を変更
	if (!isShakeDown_) {
		weaponCollisionTransform_.scale = Vector3();
		weapon_Rotate_ -= (kMoveWeapon_ * motionSpeed_ / kGuadeMagnification_);
	}
	else if (isShakeDown_) {
		weaponCollisionTransform_.scale = kWeaponCollisionBase_ * strongAddScale_;
		weapon_Rotate_ += kMoveWeaponShakeDown_ * kAttackDivisionMagnification_ * motionSpeed_;
	}


	weaponTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
}

void Player::FourthStrongAttackMotion(){
	//斬りはらって後退
	//限度によって行う処理を変更
	if (weapon_Rotate_ >= weapon_StrongRotatesMinMax_[3].y) {
		//振り切った後
		waitTime_ -= 1;
		weapon_Rotate_ = weapon_StrongRotatesMinMax_[3].y;
		
	}
	else if (weapon_Rotate_ <= weapon_StrongRotatesMinMax_[3].x) {
		//振りかぶったあと
		audio_->PlayAudio(attackMotionSE_, seVolume_, false);
		isShakeDown_ = true;
		downVector_.y += jumpPower_ / kStrongAttackMagnification_;
	}
	

	if (waitTime_ <= 0) {
		isEndAttack_ = true;
	}
	//振り下ろしているかどうかで処理を変更
	if (!isShakeDown_) {
		weaponCollisionTransform_.scale = Vector3();
		weapon_Rotate_ -= (kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
	}
	else if (isShakeDown_) {
		weaponCollisionTransform_.scale = kWeaponCollisionBase_ * strongAddScale_;
		weapon_Rotate_ += kMoveWeaponShakeDown_ * kStrongAttackMagnification_ * motionSpeed_;

		if (downVector_.y != 0.0f) {
			//後退処理
			move_ = { 0.0f,0.0f,-(moveSpeed_ * kStrongAttackMagnification_) };
			move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

			if (!isCollisionEnemy_) {
				Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

				if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
					move_.x = 0;
				}
				if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
					move_.z = 0;
				}

				playerTransform_.translate += move_ * motionSpeed_;
			}
			weaponTransform_.translate = playerTransform_.translate;
			workAttack_.AttackTimer += timeScale_;
		}

	}


	weaponTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
}

void Player::FifthStrongAttackMotion(){
	//ボタン連打で連続切り、後にフィニッシュ攻撃

	if (!isFinishAttack_){	
		//連続切り
		if (isFirstAttack_){
			//一段目
			if (weapon_Rotate_ >= weapon_StrongRotatesMinMax_[4].y) {
				waitTime_ -= 1;
				weapon_Rotate_ = weapon_StrongRotatesMinMax_[4].y;
			}
			else if (weapon_Rotate_ <= weapon_StrongRotatesMinMax_[4].x) {
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				isShakeDown_ = true;
				isTrail_ = true;
			}

			//攻撃が終わったら二段目に派生
			if (waitTime_ <= 0) {
				isFirstAttack_ = false;
				waitTime_ = waitTimeBase_;
				weaponTransform_.rotate.z = weapon_StrongRotatesMinMax_[4].x;
				hitRecord_.Clear();
			}

			if (!isShakeDown_) {
				weapon_Rotate_ -= (kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
			}
			else if (isShakeDown_) {

				weapon_Rotate_ += kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_;
			}
		}
		else {
			//二段目
			if (weapon_Rotate_ <= weapon_StrongRotatesMinMax_[5].y) {
				waitTime_ -= 1;
				weapon_Rotate_ = weapon_StrongRotatesMinMax_[5].y;
			}
			else if (weapon_Rotate_ >= weapon_StrongRotatesMinMax_[5].x) {
				
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				isShakeDown_ = true;
			}

			if (!isNextAttack_) {			
				//ボタンを押したら連続攻撃を継続
				if (input_->GetPadButtonTriger(Input::GamePad::Y)) {
					isNextAttack_ = true;
				}			
			}
			if (waitTime_ <= 0) {
				//攻撃が終わったときにフラグによってとる行動を分岐
				if (isNextAttack_){
					//一段目に戻り連続攻撃を継続
					isNextAttack_ = false;
					isFirstAttack_ = true;
					waitTime_ = waitTimeBase_;
					weaponTransform_.rotate.z = -kAttackMagnification_;
					hitRecord_.Clear();
					audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				}
				else {
					//連続攻撃をやめ、フィニッシュ攻撃を行う
					waitTime_ = waitTimeBase_;
					weaponTransform_.rotate.z = -weapon_StrongRotatesMinMax_[4].x;
					weapon_Rotate_ = -0.0f;
					isShakeDown_ = false;
					isTrail_ = false;
					isFinishAttack_ = true;
					type_ = HitRecord::Strong;
					hitRecord_.Clear();
				}
			}
			else if (!isShakeDown_) {
				weapon_Rotate_ += (kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
			}
			else if (isShakeDown_) {

				weapon_Rotate_ -= kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_;
			}
		}
	}
	else {
		//フィニッシュ攻撃
		if (weapon_Rotate_ >= weapon_StrongRotatesMinMax_[4].y) {
			//振り切った後
			waitTime_ -= 1;
			weapon_Rotate_ = weapon_StrongRotatesMinMax_[4].y;
			SettingGroundCrushTex();
		}
		else if (weapon_Rotate_ <= weapon_StrongRotatesMinMax_[4].x) {
			//振りかぶった後
			audio_->PlayAudio(attackMotionSE_, seVolume_, false);
			isTrail_ = true;
			isShakeDown_ = true;
		}
		
		//一連行動終わり
		if (waitTime_ <= 0) {
			isEndAttack_ = true;
		}

		if (!isShakeDown_) {
			weapon_Rotate_ -= (kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
		}
		else if (isShakeDown_) {
			weaponCollisionTransform_.scale = fifthWeaponCollisionScale_ * strongAddScale_;
			weapon_Rotate_ += kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_;
		}
	}
	


	weaponTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate_ + baseRotate_.x;

}

void Player::SixthStrongAttackMotion(){
	//ガード反撃
	//構え中
	if (!chargeEnd_) {
		weaponCollisionTransform_.scale = Vector3();

		//ガード
		if (input_->GetPadButton(XINPUT_GAMEPAD_Y)) {
			isGuard_ = true;
			weaponTransform_.rotate.x += 0.01f;
			if (weaponTransform_.rotate.x > strongSixthAttackRotate_) {
				weaponTransform_.rotate.x = strongSixthAttackRotate_;
			}
		}
		//ボタンを離したら派生
		if (input_->GetPadButtonRelease(XINPUT_GAMEPAD_Y)) {
			isGuard_ = false;
			chargeEnd_ = true;
			isTrail_ = true;
			weapon_offset_Base_ = { 0.0f,kWeapon_offset_,0.0f };
			weaponTransform_.rotate.z = strongAttackRotateZ_;
		}

	}
	else {
		//攻撃振り
		weaponCollisionTransform_.scale = kWeaponCollisionBase_ * strongAddScale_;
		easeT_ += addEaseT_;

		if (easeT_ > 1.0f) {
			//振り切った
			easeT_ = 1.0f;
			
			addEaseT_ = 0.0f;
			audio_->PlayAudio(attackMotionSE_, seVolume_, false);
			SettingGroundCrushTex();
		}
		//最後まで行ったら待機
		if (easeT_ == 1.0f) {
			waitTime_ -= 1;
		}
		
		//待機時間が終わったら終了
		if (waitTime_ <= 0) {			
			isEndAttack_ = true;
		}

		weapon_Rotate_ = Ease::Easing(Ease::EaseName::EaseInCubic, 2.3f, -5.5f, easeT_);
		weaponTransform_.rotate.z = Ease::Easing(Ease::EaseName::EaseInCubic, 1.57f, 2.8f, easeT_);
		


		weaponTransform_.rotate.x = weapon_Rotate_;
		weaponCollisionTransform_.rotate.x = weapon_Rotate_;
	}
}

void Player::SettingGroundCrushTex(){
	isStopCrush_ = true;
	groundCrush_->color_.w = 1.0f;
}
