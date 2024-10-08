#include "Player.h"
#include"LockOn.h"
#include"ImGuiManager.h"
#include"Ease/Ease.h"
#include"LevelLoader/LevelLoader.h"

const std::array<Player::ConstAttack, Player::ConboNum>
Player::kConstAttacks_ = {
	{
	{0,0,20,0,0.0f,0.0f,0.15f},
	{15,10,15,0,0.2f,0.0f,0.15f},
	{15,10,15,30,0.2f,0.0f,0.15f}
	}
};

Player::~Player(){
}

void Player::ApplyGlobalVariables() {
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";

	kDashSpeed = adjustment_item->GetfloatValue(groupName, "DashSpeed");
	kDashCoolTime = adjustment_item->GetIntValue(groupName, "DashCoolTime");
	jumpPower = adjustment_item->GetfloatValue(groupName, "JumpPower");
	downSpeed = adjustment_item->GetfloatValue(groupName, "DownSpeed");
	moveSpeed_ = adjustment_item->GetfloatValue(groupName, "MoveSpeed");
	baseAttackPower_ = adjustment_item->GetIntValue(groupName, "AttackPower");
	motionSpeed_ = adjustment_item->GetfloatValue(groupName, "MotionSpeed");
	motionDistance_ = adjustment_item->GetfloatValue(groupName, "MotionDistance");

}

void Player::Initialize(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "DashCoolTime", kDashCoolTime);
	adjustment_item->AddItem(groupName, "DashSpeed", kDashSpeed);
	adjustment_item->AddItem(groupName, "JumpPower", jumpPower);
	adjustment_item->AddItem(groupName, "DownSpeed", downSpeed);
	adjustment_item->AddItem(groupName, "MoveSpeed", moveSpeed_);
	adjustment_item->AddItem(groupName, "AttackPower", baseAttackPower_);
	adjustment_item->AddItem(groupName, "MotionSpeed", motionSpeed_);
	adjustment_item->AddItem(groupName, "MotionDistance", motionDistance_);
	
	input_ = Input::GetInstance();

	playerSkinAnimObj_ = LevelLoader::GetInstance()->GetLevelSkinAnimObject("Player");
	playerSkinAnimObj_->SetAnimation("human", "stand", true);
	playerSkinAnimObj_->SetAnimation("human", "Run", true);
	playerSkinAnimObj_->SetAnimation("human", "jump", false);

	playerSkinAnimObj_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());

	playerSkinAnimObj_->SetIsLighting(true);

	weaponObj_ = std::make_unique<Object3D>();
	weaponObj_->Initialize("Weapon");
	weaponObj_->SetIsGetTop(true);
	weaponObj_->SetIsLighting(false);

	shadow_ = std::make_unique<Sprite>();
	shadow_->Initialize(TextureManager::GetInstance()->Load("resources/texture/shadow.png"));
	shadow_->rotation_.x = 1.57f;
	shadow_->scale_ = { 0.7f,0.7f };
	shadow_->anchorPoint_ = { 0.5f,0.5f };
	shadow_->color_.w = 0.5f;



	weaponCollisionObj_ = std::make_unique<Object3D>();
	weaponCollisionObj_->Initialize("box");


	particle_ = std::make_unique<ParticleBase>();
	particle_->Initialize();

	particleSword_ = std::make_unique<ParticleBase>();
	particleSword_->Initialize();
	particleSword_->SetPositionRange({ 0.0f,0.0f });
	particleSword_->SetVelocityRange({ -0.5f,0.5f });
	particleSword_->SetAcceleration(Vector3::Normalize(postureVec_) * 0.0f);
	particleSword_->SetAddParticle(3);
	particleSword_->SetLifeTime(0.5f);
	particleSword_->SetNotMove();

	playerTransform_ = playerSkinAnimObj_->transform_;
	particleTrans_ = playerTransform_;
	particleTransCenter_ = playerTransform_;

	weaponTransform_ = {
		.scale = {0.15f,0.15f,0.15f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	weaponCollisionTransform_ = {
		.scale = {0.9f,3.0f,0.9f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.8f,0.0f}
	};

	dashCoolTime = kDashCoolTime;

	playerRotateMatrix_ = Matrix::MakeIdentity4x4();


	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

	isDown_ = true;

	trail_ = std::make_unique<TrailEffect>();
	trail_->Initialize(15, "resources/texture/TrailEffect/Trail.png");

	trailRender_ = std::make_unique<TrailRender>();
	trailRender_->Initialize();
}

void Player::Update(){
	ApplyGlobalVariables();

	if (hitTimer_ != 0) {
		hitTimer_--;
	}
	

	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kStrongAttack:
			BehaviorAllStrongAttackInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;		
		}

	}
	// 振る舞いリクエストをリセット
	behaviorRequest_ = std::nullopt;

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kStrongAttack:
		BehaviorStrongAttackUpdate(input_);
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	
	}
	
	shadow_->position_ = playerTransform_.translate;
	shadow_->position_.y = 1.01f;

	shadow_->scale_.x = shadowScaleBase_ + (1.0f - playerTransform_.translate.y);
	shadow_->scale_.y = shadowScaleBase_ + (1.0f - playerTransform_.translate.y);

	if (shadow_->scale_.x < 0.0f) {
		shadow_->scale_.x = 0.0f;
		shadow_->scale_.y = 0.0f;
	}

	playerScaleMatrix_ = Matrix::MakeScaleMatrix(playerTransform_.scale);
	playerTransformMatrix_ = Matrix::MakeTranslateMatrix(playerTransform_.translate);

	if (isDash_){
		playerOBB_.center.y = 10000;

	}
	else {
		playerOBB_.center = playerTransform_.translate + obbPoint_;

		playerOBB_.size = playerTransform_.scale + obbAddScale_;
		SetOridentatios(playerOBB_, playerRotateMatrix_);
	}

	
	
	
	weaponOBB_.center = weaponCollisionTransform_.translate;
	weaponOBB_.size = weaponCollisionTransform_.scale;
	Matrix4x4 weaponRotateMatrix = Matrix::MakeRotateMatrix(weaponCollisionTransform_.rotate);
	SetOridentatios(weaponOBB_, weaponRotateMatrix);

	playerMatrix_ = Matrix::MakeAffineMatrix(playerScaleMatrix_, playerRotateMatrix_, playerTransformMatrix_);

	playerOBBScaleMatrix_.MakeScaleMatrix(playerOBB_.size);
	playerOBBTransformMatrix_.MakeTranslateMatrix(playerOBB_.center);

	playerOBBMatrix_ = Matrix::MakeAffineMatrix(playerOBBScaleMatrix_, playerRotateMatrix_, playerOBBTransformMatrix_);
	if (behavior_ != Behavior::kRoot) {
		Matrix4x4 weaponRotateVec = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
		weaponRotateVec *= (playerRotateMatrix_);
		weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_.scale, weaponRotateVec, weaponCollisionTransform_.translate);
	}
	else {
		//Matrix4x4 scaleinverse = Matrix::Multiply(debugMatrix_[rightHandNumber_].ScaleInverce(), debugMatrix_[rightHandNumber_]);
		Matrix4x4 weaponRotateVec = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
		weaponRotateVec *= (playerRotateMatrix_);
		weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_.scale, weaponRotateVec, weaponTransform_.translate);
		//weaponMatrix_ = Matrix::Multiply(weaponMatrix_,playerRotateMatrix_);
	}
	weaponCollisionMatrix_= Matrix::MakeAffineMatrix(weaponCollisionTransform_.scale, weaponCollisionTransform_.rotate, weaponCollisionTransform_.translate);

	trail_->Update();

	if (playerTransform_.translate.y <= -5.0f) {
		Respawn();
	}
}

void Player::TexDraw(const Matrix4x4& viewProjection){
	shadow_->Draw(viewProjection);
	trailRender_->Draw(trail_.get(), viewProjection);
}

void Player::Draw(const ViewProjection& viewProjection){
	//if ((behavior_ == Behavior::kAttack) || (behavior_ == Behavior::kStrongAttack)) {
		weaponObj_->SetMatrix(weaponMatrix_);
		weaponObj_->SetShininess(shiness_);
		weaponObj_->Update(viewProjection);
		weaponObj_->Draw();
	//}



#ifdef _DEBUG
	

#endif
		/*particleTrans_.translate = weaponObj_->GetTopVerTex().head;
		particleTrans_.scale = { 0.3f,0.3f,0.3f };

		particleTransCenter_.translate = (weaponObj_->GetTopVerTex().tail + weaponObj_->GetTopVerTex().head) / 2.0f;
		particleTransCenter_.scale = { 0.3f,0.3f,0.3f };*/

	
}

void Player::SkinningDraw(const ViewProjection& viewProjection){
	playerSkinAnimObj_->SetMatrix(playerMatrix_);
	playerSkinAnimObj_->Update(viewProjection);
	playerSkinAnimObj_->Draw();
}

void Player::ParticleDraw(const ViewProjection& viewProjection){
	/*EulerTransform newTrans = playerTransform_;
	newTrans.translate.y += 3.0f;

	particle_->Update(newTrans, viewProjection);
	

	if (behavior_ == Behavior::kStrongAttack && chargeEnd_ == false && workAttack_.comboIndex_ == 1) {
		
		particle_->Draw();

		
	}
	particleSword_->Update(particleTransCenter_, viewProjection);
	if (behavior_ != Behavior::kRoot && workAttack_.comboIndex_ > 0) {
		
		particleSword_->SetScale({ 0.003f ,0.003f,0.003f });
		particleSword_->Draw();
	}
	*/
	
}

void Player::TrailDraw(){

}

void Player::DrawImgui(){
#ifdef _DEBUG
	ImGui::Begin("プレイヤーのステータス");
	ImGui::DragFloat3("自機の座標", &playerTransform_.translate.x, 0.1f);
	ImGui::Text("ダッシュのクールタイム = %d", dashCoolTime);
	ImGui::Text("攻撃時間 = %d", workAttack_.AttackTimer_);
	ImGui::Text("今のコンボ段階 = %d", workAttack_.comboIndex_);
	ImGui::Text("敵に当たっているか = %d", isCollisionEnemy_);
	ImGui::DragFloat("武器判定の回転", &weapon_Rotate, 0.01f);
	ImGui::DragFloat3("武器の回転", &weaponTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("武器攻撃判定の回転", &weaponCollisionTransform_.rotate.x, 0.1f);	
	ImGui::DragFloat3("オフセットのベース", &Weapon_offset_Base.x, 0.1f);
	ImGui::DragFloat3("オフセット", &Weapon_offset.x, 0.1f);
	ImGui::DragFloat("武器の反射", &shiness_, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("武器の高さ補正", &addHeight_, 0.01f);

	ImGui::Text("丸影のあれこれ");
	ImGui::DragFloat3("丸影の回転", &shadow_->rotation_.x, 0.01f);
	ImGui::DragFloat("丸影の大きさ", &shadow_->scale_.x, 0.01f);
	shadow_->scale_.y = shadow_->scale_.x;
	ImGui::DragFloat3("obbの座標", &obbPoint_.x, 0.01f);
	ImGui::DragFloat3("obbのサイズ", &obbAddScale_.x, 0.01f);
	ImGui::End();
	
	playerSkinAnimObj_->DrawImgui("プレイヤー");

	trail_->DrawImgui("トレイル");
	//particle_->DrawImgui("プレイヤーパーティクル");
#endif
}

void Player::onFlootCollision(OBB obb){
	playerTransform_.translate.y = /*playerOBB_.size.y + */obb.size.y;
	downVector = { 0.0f,0.0f,0.0f };
}

void Player::Respawn(){
	playerRotateMatrix_ = Matrix::MakeIdentity4x4();

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

	playerTransform_ = LevelLoader::GetInstance()->GetLevelObjectTransform("Player");
	downVector = { 0.0f,0.0f,0.0f };

	playerScaleMatrix_ = Matrix::MakeScaleMatrix(playerTransform_.scale);
	playerTransformMatrix_ = Matrix::MakeTranslateMatrix(playerTransform_.translate);

	playerMatrix_ = Matrix::MakeAffineMatrix(playerScaleMatrix_, playerRotateMatrix_, playerTransformMatrix_);

	shadow_->rotation_.x = 1.57f;
	shadow_->scale_ = { 0.7f,0.7f };
	shadow_->anchorPoint_ = { 0.5f,0.5f };
	shadow_->color_.w = 0.5f;
}

void Player::SetHitTimer(){
	hitTimer_ = 30;
}

void Player::SetIsDown(bool isDown){
	if (move_.x == 0.0f && move_.z == 0.0f) {
		if (isDown_ == true && isDown == false) {

		}
	}
	isDown_ = isDown;
}

void Player::BehaviorRootInitialize(){
	move_ = { 0.0f,0.0f,0.0f };
	Weapon_offset_Base = { 0.0f,3.0f,0.0f };
	workAttack_.comboIndex_ = 0;
	weaponTransform_.translate = { 0.0f,0.0f,0.0f };
	weaponTransform_.rotate.x = 0;
	weaponTransform_.rotate.y = 0;
	weaponTransform_.rotate.z = 0;
	weaponCollisionTransform_.scale = { 0.9f,3.0f,0.9f };
	weaponCollisionTransform_.translate.y = 10000.0f;
	trail_->Reset();
}

void Player::BehaviorRootUpdate(){
	frontVec_ = postureVec_;
	/*自機の移動*/	
	move_.z = input_->GetPadLStick().y * moveSpeed_;
	if (abs(move_.z) < 0.005f) {
		move_.z = 0;
	}	
	move_.x = input_->GetPadLStick().x * moveSpeed_;
	if (abs(move_.x) < 0.005f) {
		move_.x = 0;
	}
	
	if (viewProjection_){

		Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(viewProjection_->rotation_);
		move_ = Matrix::TransformNormal(move_, newRotateMatrix);
		move_.y = 0.0f;
	}
	
	move_ = Vector3::Mutiply(Vector3::Normalize(move_), moveSpeed_ * 3.0f);
	move_.y = 0.0f;
	
	if (move_.x != 0.0f || move_.z != 0.0f) {
		postureVec_ = move_;

		particleSword_->SetAcceleration(Vector3::Normalize(postureVec_) * 0.0f);
		
		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::Multiply(playerRotateMatrix_, directionTodirection_);
		if (!isDown_) {
			playerSkinAnimObj_->SetAnimSpeed(1.0f);
			playerSkinAnimObj_->ChangeAnimation("walk");
			playerSkinAnimObj_->SetChangeAnimSpeed(3.0f);
		}
	}
	else if(lockOn_&&lockOn_->ExistTarget()){
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPos - playerTransform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		postureVec_ = sub;

		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::Multiply(playerRotateMatrix_, directionTodirection_);
	}

	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A) && !isDown_) {
		downVector.y += jumpPower;
		
	}
	Vector3 NextPos = playerTransform_.translate + move_;

	if (NextPos.x >= 95.0f or NextPos.x <= -95.0f){
		move_.x = 0;
	}
	if (NextPos.z >= 95.0f or NextPos.z <= -95.0f) {
		move_.z = 0;
	}

	playerTransform_.translate += move_;
	
	if (isDown_) {
		downVector.y += downSpeed;
		playerSkinAnimObj_->SetAnimSpeed(2.0f);
	
		playerSkinAnimObj_->ChangeAnimation("jump");
		playerSkinAnimObj_->SetChangeAnimSpeed(6.0f);
		playerSkinAnimObj_->AnimationTimeStop(60.0f);

	}
	else {
		if (input_->GetPadLStick().x == 0.0f && input_->GetPadLStick().y == 0.0f && playerSkinAnimObj_->ChackAnimationName() != "jump") {
			playerSkinAnimObj_->ChangeAnimation("stand");
			playerSkinAnimObj_->SetChangeAnimSpeed(3.0f);
			playerSkinAnimObj_->SetAnimSpeed(1.0f);
		}
		playerSkinAnimObj_->AnimationStart();
		
	}
	if (dashCoolTime != 0) {
		dashCoolTime -= 1;
	}
	playerTransform_.translate.y += downVector.y;
	Vector3 weaponRotateVec = weaponTransform_.rotate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponRotateVec);
	weaponCollisionRotateMatrix = Matrix::Multiply(weaponCollisionRotateMatrix, playerRotateMatrix_);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponTransform_.translate = playerTransform_.translate + Weapon_offset;
	weaponTransform_.translate.y += addHeight_;
	//weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_RIGHT_SHOULDER) && dashCoolTime <= 0) {
		behaviorRequest_ = Behavior::kDash;
	}
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_X) && !isDown_) {
		workAttack_.comboIndex_ = 1;
		behaviorRequest_ = Behavior::kAttack;
	}
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_Y) && !isDown_) {
		workAttack_.comboIndex_ = 1;
		behaviorRequest_ = Behavior::kStrongAttack;
	}
	
}

void Player::BehaviorAttackInitialize(){
	trail_->Reset();
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 21;
	//baseRotate_.y = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = 0.0f;
	weaponTransform_.rotate.z = -0.5f;
	weaponTransform_.translate = playerTransform_.translate;
	Weapon_offset_Base.y = 2.0f;
	addHeight_ = 1.0f;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };

	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = -0.5f;
	isShakeDown = false;
	isEndAttack_ = false;
}

void Player::BehaviorSecondAttackInitialize(){
	trail_->Reset();
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 28;
	//baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = -0.3f;
	weaponTransform_.rotate.y = 0.0f;
	weaponTransform_.rotate.z = 2.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Weapon_offset_Base.y = 2.0f;
	addHeight_ = 0.5f;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	
	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = -0.4f;
	isShakeDown = false;
	isEndAttack_ = false;
}

void Player::BehaviorThirdAttackInitialize(){
	trail_->Reset();
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 28;
	//baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = -0.3f;
	weaponTransform_.rotate.y = 0.0f;
	weaponTransform_.rotate.z = -1.7f;
	weaponTransform_.translate = playerTransform_.translate;
	Weapon_offset_Base.y = 2.0f;
	addHeight_ = 0.5f;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;


	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = -0.4f;
	isShakeDown = false;
	isEndAttack_ = false;
}

void Player::BehaviorFourthAttackInitialize()
{
	trail_->Reset();
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 28;
	////baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	//weaponTransform_.rotate.x = -0.3f;
	//weaponTransform_.rotate.y = 0.0f;
	weaponTransform_.rotate.z = -2.2f;
	//weaponTransform_.translate = playerTransform_.translate;
	Weapon_offset_Base.y = 2.0f;
	addHeight_ = 0.5f;

	//Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	//Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	//weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	//weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	//weapon_Rotate = -2.0f;
	isShakeDown = true;
	isEndAttack_ = false;
}

void Player::BehaviorFifthAttackInitialize()
{
	trail_->Reset();
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 28;
	//baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = -0.3f;
	weaponTransform_.rotate.y = 0.0f;
	weaponTransform_.rotate.z = 2.5f;
	weaponTransform_.translate = playerTransform_.translate;
	Weapon_offset_Base.y = 2.0f;
	addHeight_ = 0.5f;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;


	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = -0.4f;
	isShakeDown = false;
	isEndAttack_ = false;
}

void Player::BehaviorSixthAttackInitialize(){
	trail_->Reset();
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 28;
	//baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = -0.3f;
	weaponTransform_.rotate.y = 0.0f;
	weaponTransform_.rotate.z = 0.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Weapon_offset_Base.y = 2.0f;
	addHeight_ = 0.5f;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;


	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = -0.0f;
	isShakeDown = false;
	isEndAttack_ = false;
}

void Player::BehaviorAttackUpdate(){
	frontVec_ = postureVec_;

	if (isEndAttack_) {
		if (workAttack_.comboNext_) {
			workAttack_.comboIndex_++;

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

			if (workAttack_.comboIndex_ == 1) {
				BehaviorAttackInitialize();
			}
			else if (workAttack_.comboIndex_ == 2) {
				BehaviorSecondAttackInitialize();
			}
			else if (workAttack_.comboIndex_ == 3) {
				BehaviorThirdAttackInitialize();
			}
			else if (workAttack_.comboIndex_ == 4) {
				Weapon_offset_Base = { 0.0f,4.0f,0.0f };
				BehaviorFourthAttackInitialize();
			}
			else if (workAttack_.comboIndex_ == 5) {

				BehaviorFifthAttackInitialize();
			}
			else if (workAttack_.comboIndex_ == 6) {

				BehaviorSixthAttackInitialize();
			}
		}
		else {
			
			if (workAttack_.strongComboNext_) {
				workAttack_.comboIndex_++;
				behaviorRequest_ = Behavior::kStrongAttack;
			}
			else {
				if (++workAttack_.attackParameter_ >= ((float)(workAttack_.nextAttackTimer + motionDistance_) / motionSpeed_)) {

					behaviorRequest_ = Behavior::kRoot;
					workAttack_.attackParameter_ = 0;
				}
			}

		}
		
	}
	

	if (behaviorRequest_==Behavior::kStrongAttack){
		return;
	}
	switch (workAttack_.comboIndex_){
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
	if (workAttack_.comboIndex_< ConboNum) {
		if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_X)) {
			//コンボ有効
			workAttack_.comboNext_ = true;
		}
		else if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_Y)) {
			//強コンボ派生有効
			workAttack_.strongComboNext_ = true;
		}
	}
	downVector.y += downSpeed;

	playerTransform_.translate.y += downVector.y;
	
	weaponTransform_.translate = playerTransform_.translate;
	weaponTransform_.translate.y += addHeight_;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weaponCollisionRotateMatrix = Matrix::Multiply(weaponCollisionRotateMatrix, playerRotateMatrix_);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
	weaponCollisionTransform_.translate.y += addHeight_;

	if (workAttack_.comboIndex_ != 0) {
		weaponCollisionTransform_.translate.y += 1.0f;
		weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_.scale, weaponCollisionRotateMatrix, weaponCollisionTransform_.translate);
		weaponObj_->SetMatrix(weaponMatrix_);
		weaponObj_->UniqueUpdate();
		trail_->SetPos(weaponObj_->GetTopVerTex().head, weaponObj_->GetTopVerTex().tail);
	}
	
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_RIGHT_SHOULDER) && dashCoolTime <= 0) {
		behaviorRequest_ = Behavior::kDash;
	}

	/*if (WaitTime <= 0) {
		behaviorRequest_ = Behavior::kRoot;
	}*/
	//weapon_Rotate==0.0fの時arm_Rotate-3.15f
}

void Player::BehaviorAllStrongAttackInitialize(){
	endCombo_ = false;
	chargeEnd_ = false;

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

	if (workAttack_.comboIndex_ == 1) {
		BehaviorStrongAttackInitialize();		
	}
	else if (workAttack_.comboIndex_ == 2) {
		BehaviorSecondStrongAttackInitialize();
	}
	else if (workAttack_.comboIndex_ == 3) {
		BehaviorThirdStrongAttackInitialize();
	}
	else if (workAttack_.comboIndex_ == 4) {
		Weapon_offset_Base = { 0.0f,4.0f,0.0f };
		BehaviorFourthStrongAttackInitialize();
	}
	else if (workAttack_.comboIndex_ == 5) {
		BehaviorFifthStrongAttackInitialize();
		
	}
}

void Player::BehaviorStrongAttackUpdate(Input* input){
	frontVec_ = postureVec_;

	if (isEndAttack_) {
			 
		if (++workAttack_.attackParameter_ >= ((float)(workAttack_.nextAttackTimer + motionDistance_) / motionSpeed_)) {

			behaviorRequest_ = Behavior::kRoot;
			workAttack_.attackParameter_ = 0;
		}	

	}


	switch (workAttack_.comboIndex_) {
	case 1:
		StrongAttackMotion(input);
		break;
	case 2:
		secondStrongAttackMotion();
		break;
	case 3:
		thirdStrongAttackMotion();
		break;
	case 4:
		fourthStrongAttackMotion();
		break;
	case 5:
		fifthStrongAttackMotion();
		break;
	default:
		break;
	}
	downVector.y += downSpeed;
	playerTransform_.translate.y += downVector.y;
	weaponTransform_.translate = playerTransform_.translate;
	weaponTransform_.translate.y += addHeight_;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	weaponCollisionRotateMatrix = Matrix::Multiply(weaponCollisionRotateMatrix, playerRotateMatrix_);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
	weaponCollisionTransform_.translate.y += addHeight_;

	if (workAttack_.comboIndex_ != 0) {
		weaponCollisionTransform_.translate.y += 1.0f;
		weaponMatrix_ = Matrix::MakeAffineMatrix(weaponTransform_.scale, weaponCollisionRotateMatrix, weaponCollisionTransform_.translate);
		weaponObj_->SetMatrix(weaponMatrix_);
		weaponObj_->UniqueUpdate();
		if (workAttack_.comboIndex_ == 1){
			if (isTrail_){
				trail_->SetPos(weaponObj_->GetTopVerTex().head, weaponObj_->GetTopVerTex().tail);
			}
		}
		else {
			trail_->SetPos(weaponObj_->GetTopVerTex().head, weaponObj_->GetTopVerTex().tail);
		}
		
	}

	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_RIGHT_SHOULDER) && dashCoolTime <= 0) {
		behaviorRequest_ = Behavior::kDash;
	}

}

void Player::BehaviorDashInitialize(){
	workDash_.dashParameter_ = 0;

	isDash_ = true;
}

void Player::BehaviorDashUpdate(){
	Matrix4x4 newRotateMatrix_ = playerRotateMatrix_;
	move_ = { 0, 0, moveSpeed_ * kDashSpeed };

	move_ = Matrix::TransformNormal(move_, newRotateMatrix_);

	playerSkinAnimObj_->ChangeAnimation("Run");
	playerSkinAnimObj_->SetChangeAnimSpeed(6.0f);
	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 15;

	if (!isCollisionEnemy_) {
		Vector3 NextPos = playerTransform_.translate + move_;

		if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
			move_.x = 0;
		}
		if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
			move_.z = 0;
		}
		playerTransform_.translate += move_;
	}

	//既定の時間経過で通常状態に戻る
	if (++workDash_.dashParameter_ >= behaviorDashTime) {
		dashCoolTime = kDashCoolTime;
		isDash_ = false;
		behaviorRequest_ = Behavior::kRoot;
		playerSkinAnimObj_->ChangeAnimation("stand");
		playerSkinAnimObj_->SetChangeAnimSpeed(3.0f);
	}
}

void Player::AttackMotion(){
	if (weapon_Rotate >= 2.4f) {
		WaitTime -= 1;
		weapon_Rotate = 3.16f;
	}
	else if (weapon_Rotate <= -1.0f) {
		isShakeDown = true;
	}
	else {
		move_ = { 0.0f,0.0f,moveSpeed_ * 1.5f };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
				move_.x = 0;
			}
			if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
				move_.z = 0;
			}

			playerTransform_.translate += move_ * motionSpeed_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer_++;
	}

	if (WaitTime <= 0) {
		isEndAttack_ = true;
	}

	if (!isShakeDown) {
		weapon_Rotate -= (moveWeapon * motionSpeed_ / 2.0f);
	}
	else if (isShakeDown) {

		weapon_Rotate += moveWeaponShakeDown * 1.5f * motionSpeed_;
	}


	weaponTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
}

void Player::SecondAttackMotion(){	
	
	if (weapon_Rotate >= 3.16f) {
		WaitTime -= 1;
		weapon_Rotate = 3.16f;
	}
	else if (weapon_Rotate <= -0.9f) {
		isShakeDown = true;
	}
	else {
		move_ = { 0.0f,0.0f,moveSpeed_ * 1.5f };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
				move_.x = 0;
			}
			if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
				move_.z = 0;
			}

			playerTransform_.translate += move_ * motionSpeed_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer_++;
	}

	if (WaitTime <= 0) {
		isEndAttack_ = true;
	}

	if (!isShakeDown) {
		weapon_Rotate -= (moveWeapon * motionSpeed_ / 2.0f) ;
	}
	else if (isShakeDown) {
		
		weapon_Rotate += moveWeaponShakeDown * 1.5f * motionSpeed_;
	}
	

	weaponTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate + baseRotate_.x;

	//Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	//Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	//weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}

void Player::ThirdAttackMotion(){
	
	
	if (weapon_Rotate >= 3.16f) {
		WaitTime -= 1;
		weapon_Rotate = 3.16f;
	}
	else if (weapon_Rotate <= -0.9f) {
		isShakeDown = true;
	}
	else {
		move_ = { 0.0f,0.0f,moveSpeed_ * 1.5f };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
				move_.x = 0;
			}
			if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
				move_.z = 0;
			}

			playerTransform_.translate += move_ * motionSpeed_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer_++;
	}

	if (WaitTime <= 0) {
		isEndAttack_ = true;
	}

	if (!isShakeDown) {
		weapon_Rotate -= (moveWeapon * motionSpeed_ / 2.0f);
	}
	else if (isShakeDown) {

		weapon_Rotate += moveWeaponShakeDown * 1.5f * motionSpeed_;
	}


	weaponTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
}

void Player::FourthAttackMotion(){
	if (weapon_Rotate >= 3.16f + 6.28f) {
		WaitTime -= 1;
		weapon_Rotate = 3.16f + 6.28f;
	}
	
	else {
		move_ = { 0.0f,0.0f,moveSpeed_ * 1.5f };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
				move_.x = 0;
			}
			if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
				move_.z = 0;
			}

			playerTransform_.translate += move_ * motionSpeed_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer_++;
	}

	if (WaitTime <= 0) {
		isEndAttack_ = true;
	}

	if (!isShakeDown) {
		weapon_Rotate -= (moveWeapon * motionSpeed_ / 2.0f);
	}
	else if (isShakeDown) {

		weapon_Rotate += moveWeaponShakeDown * 1.5f * motionSpeed_;
	}


	weaponTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
}

void Player::FifthAttackMotion(){
	if (weapon_Rotate >= 3.16f) {
		WaitTime -= 1;
		weapon_Rotate = 3.16f;
	}
	else if (weapon_Rotate <= -0.9f) {
		isShakeDown = true;
	}
	else {
		move_ = { 0.0f,0.0f,moveSpeed_ * 1.5f };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
				move_.x = 0;
			}
			if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
				move_.z = 0;
			}

			playerTransform_.translate += move_ * motionSpeed_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer_++;
	}

	if (WaitTime <= 0) {
		isEndAttack_ = true;
	}

	if (!isShakeDown) {
		weapon_Rotate -= (moveWeapon * motionSpeed_ / 2.0f);
	}
	else if (isShakeDown) {

		weapon_Rotate += moveWeaponShakeDown * 1.5f * motionSpeed_;
	}


	weaponTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	
}

void Player::SixthAttackMotion(){
	if (weapon_Rotate >= 1.65f) {
		WaitTime -= 1;
		weapon_Rotate = 1.65f;
	}
	else if (weapon_Rotate <= -0.6f) {
		isShakeDown = true;
	}
	else {
		move_ = { 0.0f,0.0f,moveSpeed_ * 1.5f };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

			if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
				move_.x = 0;
			}
			if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
				move_.z = 0;
			}

			playerTransform_.translate += move_ * motionSpeed_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer_++;
	}

	if (WaitTime <= 0) {
		isEndAttack_ = true;
	}

	if (!isShakeDown) {
		weapon_Rotate -= (moveWeapon * motionSpeed_ / 3.0f);
	}
	else if (isShakeDown) {

		weapon_Rotate += moveWeaponShakeDown * 1.5f * motionSpeed_;
	}


	weaponTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
}

void Player::BehaviorStrongAttackInitialize(){
	trail_->Reset();
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 28;
	//baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = -0.0f;
	weaponTransform_.rotate.y = 0.0f;
	weaponTransform_.rotate.z = 1.8f;
	weaponTransform_.translate = playerTransform_.translate;
	Weapon_offset_Base.y = 0.0f;
	Weapon_offset_Base.z = 1.0f;
	addHeight_ = 1.0f;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,weaponTransform_.rotate.z };
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;


	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = 1.8f;
	isTrail_ = false;
	isShakeDown = true;
	chargeEnd_ = false;
	isEndAttack_ = false;
}

void Player::BehaviorSecondStrongAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	downVector.y += 0.3f;
	workAttack_.attackParameter_ = 0;
	baseRotate_.y = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = 1.7f;
	weaponTransform_.rotate.y = -0.2f + baseRotate_.y;
	weaponTransform_.rotate.z = 0.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate.y = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponCollisionTransform_.rotate.z = 0;
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = 1.5f;
	isShakeDown = false;
}

void Player::BehaviorThirdStrongAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.y = 0;
	weaponTransform_.rotate.z = 1.57f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = 1.0f;
	isShakeDown = false;
}

void Player::BehaviorFourthStrongAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	weaponTransform_.rotate.x = 1.57f;
	weaponTransform_.rotate.y = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.z = 0.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
	hitRecord_.Clear();
	workAttack_.AttackTimer_ = 0;
	easeT_ = 0;
	WaitTime = WaitTimeBase;
	weapon_Rotate = 3.0f;
	isShakeDown = false;
}

void Player::BehaviorFifthStrongAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.y = 0;
	weaponTransform_.rotate.z = 1.57f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
	weaponCollisionTransform_.rotate = weaponTransform_.rotate;
	weaponCollisionTransform_.rotate.x = 0;
	weaponCollisionTransform_.rotate.y = 0;
	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = 1.0f;
	isShakeDown = false;
}

void Player::StrongAttackMotion(Input* input){
	if (!chargeEnd_){
		if (input->GetPadButton(XINPUT_GAMEPAD_Y)){
			
		}
		if (input->GetPadButtonRelease(XINPUT_GAMEPAD_Y)){
			chargeEnd_ = true;
			Weapon_offset_Base = { 0.0f,2.0f,0.0f };
			weaponTransform_.rotate.z = 1.57f;
			addHeight_ = 0.5f;
		}

	}
	else {
		if (weapon_Rotate >= 9.2f) {
			WaitTime -= 1;
			weapon_Rotate = 9.2f;
		}

		else {
			move_ = { 0.0f,0.0f,moveSpeed_ * 1.5f };
			move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

			if (!isCollisionEnemy_) {
				Vector3 NextPos = playerTransform_.translate + move_ * motionSpeed_;

				if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
					move_.x = 0;
				}
				if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
					move_.z = 0;
				}

				playerTransform_.translate += move_ * motionSpeed_;
			}
			weaponTransform_.translate = playerTransform_.translate;
			workAttack_.AttackTimer_++;
		}

		if (WaitTime <= 0) {
			isEndAttack_ = true;
		}

		if (isShakeDown) {
			if (weapon_Rotate < 4.5f){
				isTrail_ = false;
				weapon_Rotate += moveWeaponShakeDown * 0.5f * motionSpeed_;
			}
			else {
				isTrail_ = true;
				weapon_Rotate += moveWeaponShakeDown * 3.0f * motionSpeed_;
			}

		}


		weaponTransform_.rotate.x = weapon_Rotate;
		weaponCollisionTransform_.rotate.x = weapon_Rotate;
	}
	
}

void Player::secondStrongAttackMotion(){
	if (weapon_Rotate >= 2.5f) {
		isShakeDown = true;
	}
	if (!isShakeDown) {
		weapon_Rotate += 0.1f;
	}
	else {
		weapon_Rotate -= 0.2f;
	}

	if (weapon_Rotate <= -1.2f) {
		weapon_Rotate = -1.2f;
	}

	if (isShakeDown) {
		workAttack_.AttackTimer_++;
		workAttack_.attackParameter_++;
	}


	weaponTransform_.rotate.x = weapon_Rotate;
	weaponCollisionTransform_.rotate.x = weapon_Rotate;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}

void Player::thirdStrongAttackMotion(){
	workAttack_.AttackTimer_++;
	if (workAttack_.AttackTimer_ >= 10) {
		workAttack_.attackParameter_++;
		isShakeDown = true;
	}
	if (weapon_Rotate >= 2.5f) {
		WaitTime -= 1;
		weapon_Rotate = 2.5f;
	}
	else {
		move_ = { 0.0f,0.0f,moveSpeed_ * 1.5f };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_;

			if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
				move_.x = 0;
			}
			if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
				move_.z = 0;
			}

			playerTransform_.translate += move_;
		}
		weaponTransform_.translate = playerTransform_.translate;
	}

	if (!isShakeDown) {
		weapon_Rotate -= moveWeapon / 2.0f;
	}
	else if (isShakeDown) {

		weapon_Rotate += moveWeaponShakeDown * 1.5f;
	}



	weaponTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate + baseRotate_.x;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}

void Player::fourthStrongAttackMotion(){
	easeT_ += 0.08f;
	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
	}
	else {

	}

	Weapon_offset_Base.x = Ease::Easing(Ease::EaseName::EaseInBack, -3.0f, 0.0f, easeT_);
	Weapon_offset_Base.y = Ease::Easing(Ease::EaseName::EaseInBack, 0.0f, 6.0f, easeT_);

	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
	}
	else {
		if (Weapon_offset_Base.y > 0.2f) {
			move_ = { 0.0f,0.0f,moveSpeed_ * 10.0f };
			move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

			
			if (!isCollisionEnemy_) {
				Vector3 NextPos = playerTransform_.translate + move_;

				if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
					move_.x = 0;
				}
				if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
					move_.z = 0;
				}

				playerTransform_.translate += move_;
			}
			weaponTransform_.translate = playerTransform_.translate;
		}
	}
	workAttack_.attackParameter_++;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}

void Player::fifthStrongAttackMotion(){
	workAttack_.AttackTimer_++;
	if (workAttack_.AttackTimer_ >= 10) {
		workAttack_.attackParameter_++;
		isShakeDown = true;
	}
	if (weapon_Rotate >= 2.5f) {
		WaitTime -= 1;
		weapon_Rotate = 2.5f;
	}
	else {
		move_ = { 0.0f,0.0f,moveSpeed_ * 1.5f };
		move_ = Matrix::TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			Vector3 NextPos = playerTransform_.translate + move_;

			if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
				move_.x = 0;
			}
			if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
				move_.z = 0;
			}

			playerTransform_.translate += move_;
		}
		weaponTransform_.translate = playerTransform_.translate;
	}

	if (!isShakeDown) {
		weapon_Rotate -= moveWeapon / 2.0f;
	}
	else if (isShakeDown) {

		weapon_Rotate += moveWeaponShakeDown * 1.5f;
	}



	weaponTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate + baseRotate_.x;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}
