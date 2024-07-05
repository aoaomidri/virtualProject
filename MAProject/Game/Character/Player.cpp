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
	
	input_ = Input::GetInstance();

	playerSkinAnimObj_ = LevelLoader::GetInstance()->GetLevelSkinAnimObject("Player");
	playerSkinAnimObj_->SetAnimation("human", "stand", true);
	playerSkinAnimObj_->SetAnimation("human", "Run", true);
	playerSkinAnimObj_->SetAnimation("human", "jump", false);

	playerSkinAnimObj_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());

	playerSkinAnimObj_->SetIsLighting(true);
	

	debugJoints_ = playerSkinAnimObj_->GetJoint();

	debugSphere_.resize(debugJoints_.size());
	debugMatrix_.resize(debugJoints_.size());

	for (size_t i = 0; i < debugJoints_.size(); i++){
		Vector3 trans{};
		trans = { 0.1f,0.1f,0.1f };
		debugSphere_[i] = std::make_unique<Object3D>();
		debugSphere_[i]->Initialize("box");
		debugMatrix_[i] = debugJoints_[i].skeltonSpaceMatrix * Matrix::GetInstance()->MakeScaleMatrix(trans);
		if (debugJoints_[i].name == "mixamorig:LeftHand" && leftHandNumber_ != i) {
			leftHandNumber_ = i;
		}

		if (debugJoints_[i].name == "mixamorig:RightHand" && rightHandNumber_ != i) {
			rightHandNumber_ = i;
		}
	}

	weaponObj_ = std::make_unique<Object3D>();
	weaponObj_->Initialize("Weapon");

	weaponCollisionObj_ = std::make_unique<Object3D>();
	weaponCollisionObj_->Initialize("box");


	particle_ = std::make_unique<ParticleBase>();
	particle_->Initialize();

	particleHands_ = std::make_unique<ParticleBase>();
	particleHands_->Initialize();
	particleHands_->SetPositionRange({ 0.0f,0.0f });
	particleHands_->SetVelocityRange({ -0.5f,0.5f });
	particleHands_->SetAcceleration(Vector3::Normalize(postureVec_) * 0.0f);
	particleHands_->SetAddParticle(1);

	playerTransform_ = playerSkinAnimObj_->transform_;
	particleTrans_ = playerTransform_;

	weaponTransform_ = {
		.scale = {0.2f,0.2f,0.2f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	weaponCollisionTransform_ = {
		.scale = {0.9f,3.0f,0.9f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.8f,0.0f}
	};

	dashCoolTime = kDashCoolTime;

	playerRotateMatrix_ = Matrix::GetInstance()->MakeIdentity4x4();


	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

	isDown_ = true;
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
	

	playerScaleMatrix_ = Matrix::GetInstance()->MakeScaleMatrix(playerTransform_.scale);
	playerTransformMatrix_ = Matrix::GetInstance()->MakeTranslateMatrix(playerTransform_.translate);

	playerOBB_.center = playerTransform_.translate;
	
	playerOBB_.size = playerTransform_.scale;
	playerOBB_.size.y = 0.8f;
	SetOridentatios(playerOBB_, playerRotateMatrix_);
	
	
	weaponOBB_.center = weaponCollisionTransform_.translate;
	weaponOBB_.size = weaponCollisionTransform_.scale;
	Matrix4x4 weaponRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	SetOridentatios(weaponOBB_, weaponRotateMatrix);

	playerMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(playerScaleMatrix_, playerRotateMatrix_, playerTransformMatrix_);
	weaponScaleMatrix_.MakeScaleMatrix(weaponTransform_.scale);
	if (behavior_ != Behavior::kRoot) {
		weaponMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(weaponTransform_.scale, weaponTransform_.rotate, weaponCollisionTransform_.translate);
	}
	else {
		Matrix4x4 scaleinverse = Matrix::GetInstance()->Multiply(debugMatrix_[rightHandNumber_].ScaleInverce(), debugMatrix_[rightHandNumber_]);
		weaponMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(weaponTransform_.scale, Vector3({0.0f,0.0f,0.0f}), weaponTransform_.translate);
		//weaponMatrix_ = Matrix::GetInstance()->Multiply(weaponMatrix_, debugMatrix_[leftHandNumber_]);
		weaponMatrix_ = Matrix::GetInstance()->Multiply(weaponMatrix_, scaleinverse);
	}
	weaponCollisionMatrix_= Matrix::GetInstance()->MakeAffineMatrix(weaponCollisionTransform_.scale, weaponCollisionTransform_.rotate, weaponCollisionTransform_.translate);

	

	if (playerTransform_.translate.y <= -5.0f) {
		Respawn();
	}
}

void Player::Draw(const ViewProjection& viewProjection){

#ifdef _DEBUG
	debugJoints_ = playerSkinAnimObj_->GetJoint();
	for (size_t i = 0; i < debugJoints_.size(); i++) {
		
		debugMatrix_[i] = debugJoints_[i].skeltonSpaceMatrix * playerMatrix_;

		if (debugJoints_[i].name == "mixamorig:LeftHand" && leftHandNumber_ != i) {
			leftHandNumber_ = i;
		}

		if (debugJoints_[i].name == "mixamorig:RightHand" && rightHandNumber_ != i) {
			rightHandNumber_ = i;
		}

		particleTrans_.translate = debugMatrix_[leftHandNumber_].GetTranslate();
		particleTrans_.scale = { 0.3f,0.3f,0.3f };

		/*debugSphere_[i]->SetMatrix(debugMatrix_[i]);
		debugSphere_[i]->Update(viewProjection);
		debugSphere_[i]->Draw();*/
	}

#endif

	//if ((behavior_ == Behavior::kAttack) || (behavior_ == Behavior::kStrongAttack)) {
		weaponObj_->SetMatrix(weaponMatrix_);
		weaponObj_->Update(viewProjection);
		weaponObj_->Draw();

		/*weaponCollisionObj_->SetMatrix(weaponCollisionMatrix_);
		weaponCollisionObj_->Update(viewProjection);
		weaponCollisionObj_->Draw();*/

	//}

	
}

void Player::SkinningDraw(const ViewProjection& viewProjection){
	playerSkinAnimObj_->SetMatrix(playerMatrix_);
	playerSkinAnimObj_->Update(viewProjection);
	playerSkinAnimObj_->Draw();
}

void Player::ParticleDraw(const ViewProjection& viewProjection){
	EulerTransform newTrans = playerTransform_;
	newTrans.translate.y += 3.0f;

	particle_->Update(newTrans, viewProjection);
	

	if (behavior_ == Behavior::kStrongAttack && chargeEnd_ == false && workAttack_.comboIndex_ == 1) {
		
		particle_->Draw();

		
	}

	/*particleHands_->Update(particleTrans_, viewProjection);
	particleHands_->SetScale({ 0.003f ,0.003f,0.003f });
	particleHands_->Draw();*/
	
}

void Player::DrawImgui(){
#ifdef _DEBUG
	ImGui::Begin("プレイヤーのステータス");
	ImGui::Text("ダッシュのクールタイム = %d", dashCoolTime);
	ImGui::Text("攻撃時間 = %d", workAttack_.AttackTimer_);
	ImGui::Text("今のコンボ段階 = %d", workAttack_.comboIndex_);
	ImGui::Text("敵に当たっているか = %d", isCollisionEnemy_);
	ImGui::DragFloat("武器判定の回転", &weapon_Rotate, 0.1f);
	ImGui::DragFloat3("武器の回転", &weaponTransform_.rotate.x, 0.1f);
	ImGui::DragFloat3("武器攻撃判定の回転", &weaponCollisionTransform_.rotate.x, 0.1f);	
	ImGui::DragFloat3("オフセットのベース", &Weapon_offset_Base.x, 0.1f);
	ImGui::DragFloat3("オフセット", &Weapon_offset.x, 0.1f);
	ImGui::DragFloat("モーションスピード", &motionSpeed_, 0.01f, 1.0f, 2.0f);
	
	ImGui::End();


	ImGui::Begin("プレイヤーのアニメーション");
	animetionNames_ = playerSkinAnimObj_->GetAnimations();
	for (size_t i = 0; i < animetionNames_.size(); i++){
		if (ImGui::Button(animetionNames_[i].c_str())) {
			playerSkinAnimObj_->ChangeAnimation(animetionNames_[i]);
		}
	}

	ImGui::End();
	playerSkinAnimObj_->DrawImgui("プレイヤー");
	particle_->DrawImgui("プレイヤーパーティクル");
#endif
}

void Player::onFlootCollision(OBB obb){
	playerTransform_.translate.y = playerOBB_.size.y + obb.size.y - 0.005f;
	downVector = { 0.0f,0.0f,0.0f };
}

void Player::Respawn(){
	playerRotateMatrix_ = Matrix::GetInstance()->MakeIdentity4x4();

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

	playerTransform_ = {
			.scale = {1.5f,1.5f,1.5f},
			.rotate = {0.0f,0.0f,0.0f},
			.translate = {0.0f,0.8f,0.0f}
	};
	downVector = { 0.0f,0.0f,0.0f };
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
	weaponTransform_.scale = { 0.3f,0.3f,0.3f };
	weaponTransform_.rotate.x = 0;
	weaponTransform_.rotate.y = 0;
	weaponTransform_.rotate.z = 0;
	weaponCollisionTransform_.scale = { 0.9f,3.0f,0.9f };
	weaponCollisionTransform_.translate.y = 10000.0f;
}

void Player::BehaviorRootUpdate(){
	frontVec_ = postureVec_;
	/*自機の移動*/
	if (input_->PushUp()) {
		//move_.z = moveSpeed_;

	}
	else if (input_->PushDown()) {
		//move_.z = -moveSpeed_view
	}
	else {
		move_.z = input_->GetPadLStick().y * moveSpeed_;
	}

	if (input_->PushRight()) {
		//move_.x = moveSpeed_;
	}
	else if (input_->PushLeft()) {
		//move_.x = -moveSpeed_;
	}
	else {
		move_.x = input_->GetPadLStick().x * moveSpeed_;
	}
	
	Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(viewProjection_->rotation_);
	move_ = Matrix::GetInstance()->TransformNormal(move_, newRotateMatrix);
	move_.y = 0.0f;
	move_ = Vector3::Mutiply(Vector3::Normalize(move_), moveSpeed_ * 3.0f);
	move_.y = 0.0f;
	
	if (move_.x != 0.0f || move_.z != 0.0f) {
		postureVec_ = move_;

		particleHands_->SetAcceleration(Vector3::Normalize(postureVec_) * 0.0f);
		
		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::GetInstance()->Multiply(playerRotateMatrix_, directionTodirection_);
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
		playerRotateMatrix_ = Matrix::GetInstance()->Multiply(playerRotateMatrix_, directionTodirection_);
	}

	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A) && !isDown_) {
		downVector.y += jumpPower;
		
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
		if (move_.x == 0.0f && move_.z == 0.0f && playerSkinAnimObj_->ChackAnimationName() != "jump") {
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
	//weaponTransform_.translate = playerTransform_.translate;
	
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(playerTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
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
	
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 21;
	baseRotate_.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = 0.0f;
	weaponTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.z = 0.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
	weaponCollisionTransform_.rotate = { 0.0f,-weaponTransform_.rotate.y,0.0f };

	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = 0.5f;
	isShakeDown = false;
}

void Player::BehaviorSecondAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 28;
	baseRotate_.x = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.y = 0;
	weaponTransform_.rotate.z = 1.57f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	//weaponCollisionTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponCollisionTransform_.rotate = { 0.0f,0.00f,1.57f };
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = 1.0f;
	isShakeDown = false;
}

void Player::BehaviorThirdAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 12;
	//baseRotate_.x = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = 1.57f;
	weaponTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.z = 0.0f ;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	//weaponCollisionTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponCollisionTransform_.rotate = { 1.57f,weaponTransform_.rotate.y,0 };
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
	hitRecord_.Clear();
	workAttack_.AttackTimer_ = 0;
	easeT_ = 0;
	WaitTime = WaitTimeBase;
	weapon_Rotate = 3.0f;
	isShakeDown = false;
}

void Player::BehaviorFourthAttackInitialize()
{
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 29;
	baseRotate_.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = 1.7f;
	weaponTransform_.rotate.y = -0.2f + baseRotate_.y;
	weaponTransform_.rotate.z = 0.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = 1.5f;
	isShakeDown = false;
}

void Player::BehaviorFifthAttackInitialize()
{
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	downVector.y += 0.15f;
	workAttack_.attackParameter_ = 0;
	workAttack_.nextAttackTimer = 24;
	baseRotate_.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.z = 0.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = 0.5f;
	isShakeDown = false;
}

void Player::BehaviorAttackUpdate(){
	frontVec_ = postureVec_;

	if (++workAttack_.attackParameter_ >= (workAttack_.nextAttackTimer+5 / motionSpeed_)) {
		if (workAttack_.comboNext_) {
			workAttack_.comboIndex_++;

			if (input_->GetPadLStick().x != 0 || input_->GetPadLStick().y != 0) {
				postureVec_ = { input_->GetPadLStick().x ,0,input_->GetPadLStick().y };
				Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(viewProjection_->rotation_);
				postureVec_ = Matrix::GetInstance()->TransformNormal(postureVec_, newRotateMatrix);
				postureVec_.y = 0;
				postureVec_ = Vector3::Normalize(postureVec_);
				Matrix4x4 directionTodirection_;
				directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
				playerRotateMatrix_ = Matrix::GetInstance()->Multiply(playerRotateMatrix_, directionTodirection_);

			}
			else if (lockOn_ && lockOn_->ExistTarget()) {
				Vector3 lockOnPos = lockOn_->GetTargetPosition();
				Vector3 sub = lockOnPos - playerTransform_.translate;
				sub.y = 0;
				sub = Vector3::Normalize(sub);
				postureVec_ = sub;


				Matrix4x4 directionTodirection_;
				directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
				playerRotateMatrix_ = Matrix::GetInstance()->Multiply(playerRotateMatrix_, directionTodirection_);


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
		}
		else {
			workAttack_.attackParameter_ = 0;
			if (workAttack_.strongComboNext_) {
				workAttack_.comboIndex_++;
				behaviorRequest_ = Behavior::kStrongAttack;
			}
			else {
				behaviorRequest_ = Behavior::kRoot;
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
		secondAttackMotion();
		break;
	case 3:	
		thirdAttackMotion();
		break;
	case 4:
		fourthAttackMotion();
		break;
	case 5:
		fifthAttackMotion();
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

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	if (workAttack_.attackParameter_ >= 35) {
		if (workAttack_.comboNext_) {
			

		}
		else {
			
			workAttack_.attackParameter_ = 0;
			
			
			behaviorRequest_ = Behavior::kRoot;
		}
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
		Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(viewProjection_->rotation_);
		postureVec_ = Matrix::GetInstance()->TransformNormal(postureVec_, newRotateMatrix);
		postureVec_.y = 0;
		postureVec_ = Vector3::Normalize(postureVec_);

		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::GetInstance()->Multiply(playerRotateMatrix_, directionTodirection_);

	}
	else if (lockOn_ && lockOn_->ExistTarget()) {
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPos - playerTransform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		postureVec_ = sub;

		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::GetInstance()->Multiply(playerRotateMatrix_, directionTodirection_);

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

	if (input->Trigerkey(DIK_0)){
		workAttack_.attackParameter_ = 0;

		behaviorRequest_ = Behavior::kRoot;
	}

	if (workAttack_.attackParameter_ >= 35) {
		if (workAttack_.strongComboNext_) {

			
		}
		else {
			workAttack_.attackParameter_ = 0;

			behaviorRequest_ = Behavior::kRoot;
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

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

}

void Player::BehaviorDashInitialize(){
	workDash_.dashParameter_ = 0;
}

void Player::BehaviorDashUpdate(){
	Matrix4x4 newRotateMatrix_ = playerRotateMatrix_;
	move_ = { 0, 0, moveSpeed_ * kDashSpeed };

	move_ = Matrix::GetInstance()->TransformNormal(move_, newRotateMatrix_);

	playerSkinAnimObj_->ChangeAnimation("Run");
	playerSkinAnimObj_->SetChangeAnimSpeed(6.0f);
	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 15;

	if (!isCollisionEnemy_) {
		playerTransform_.translate += move_;
	}

	//既定の時間経過で通常状態に戻る
	if (++workDash_.dashParameter_ >= behaviorDashTime) {
		dashCoolTime = kDashCoolTime;
		behaviorRequest_ = Behavior::kRoot;
		playerSkinAnimObj_->ChangeAnimation("stand");
		playerSkinAnimObj_->SetChangeAnimSpeed(3.0f);
	}
}

void Player::AttackMotion(){
	if (weapon_Rotate <= MinRotate) {
		isShakeDown = true;
	}
	else if (weapon_Rotate >= MaxRotate) {
		WaitTime -= 1;
		weapon_Rotate = 1.58f;
	}

	if (!isShakeDown && weapon_Rotate > MinRotate) {
		weapon_Rotate -= moveWeapon * motionSpeed_;
		weaponTransform_.rotate.y = 2.5f + baseRotate_.y;
		workAttack_.AttackTimer_++;
	}
	else if (isShakeDown && weapon_Rotate < MaxRotate) {
		weapon_Rotate += moveWeaponShakeDown * motionSpeed_;
		if (weapon_Rotate>0.0f){
			weaponTransform_.rotate.y -= 0.3f * motionSpeed_;
			move_ = { 0, 0, moveSpeed_ * 2.5f };
			move_ = Matrix::GetInstance()->TransformNormal(move_, playerRotateMatrix_);
			if (!isCollisionEnemy_){
				playerTransform_.translate += move_ * motionSpeed_;
			}
			weaponCollisionTransform_.rotate.z = 3.14f;
			weaponTransform_.translate = playerTransform_.translate;
			workAttack_.AttackTimer_++;
		}
		else {
			weaponTransform_.rotate.y -= 0.1f;
			
		}
		
	}
	
	
		

	weaponTransform_.rotate.x = weapon_Rotate;
	weaponCollisionTransform_.rotate.x = weapon_Rotate;
	weaponCollisionTransform_.rotate.y = -weaponTransform_.rotate.y;
	

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}

void Player::secondAttackMotion(){
	
	
	if (weapon_Rotate >= 2.5f) {
		WaitTime -= 1;
		weapon_Rotate = 2.5f;
	}
	else if (weapon_Rotate <= 0.3f) {
		isShakeDown = true;
	}
	else {
		move_ = { 0.0f,0.0f,moveSpeed_ * 1.5f };
		move_ = Matrix::GetInstance()->TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			playerTransform_.translate += move_ * motionSpeed_;
		}
		weaponTransform_.translate = playerTransform_.translate;
		workAttack_.AttackTimer_++;
	}

	if (!isShakeDown) {
		weapon_Rotate -= (moveWeapon * motionSpeed_ / 2.0f) ;
	}
	else if (isShakeDown) {
		
		weapon_Rotate += moveWeaponShakeDown * 1.5f * motionSpeed_;
	}

	weaponTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate + baseRotate_.x;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}

void Player::thirdAttackMotion(){
	
	
	easeT_ += 0.08f * motionSpeed_;
	if (easeT_>=1.0f){
		easeT_ = 1.0f;
	}
	else {
		workAttack_.AttackTimer_++;
	}

	Weapon_offset_Base.x = Ease::Easing(Ease::EaseName::EaseInBack, 3.0f, 0.0f, easeT_);
	Weapon_offset_Base.y = Ease::Easing(Ease::EaseName::EaseInBack, 0.0f, 6.0f, easeT_);
	
	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
	}
	else {
		if (Weapon_offset_Base.y > 0.2f) {
			move_ = { 0.0f,0.0f,moveSpeed_ * 10.0f };
			move_ = Matrix::GetInstance()->TransformNormal(move_, playerRotateMatrix_);

			if (!isCollisionEnemy_) {
				playerTransform_.translate += move_ * motionSpeed_;
			}
			weaponTransform_.translate = playerTransform_.translate;
		}
	}

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}

void Player::fourthAttackMotion(){
	if (weapon_Rotate >= 2.5f){
		isShakeDown = true;
	}
	if (!isShakeDown){
		weapon_Rotate += 0.1f * motionSpeed_;
	}
	else {
		weapon_Rotate -= 0.2f * motionSpeed_;
	}
	
	if (weapon_Rotate<=-1.2f) {
		weapon_Rotate = -1.2f;
	}
	else {
		workAttack_.AttackTimer_++;
	}
	
	
	


	weaponTransform_.rotate.x = weapon_Rotate;
	weaponCollisionTransform_.rotate.x = weapon_Rotate;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}

void Player::fifthAttackMotion(){
	if (weapon_Rotate <= MinRotate) {
		isShakeDown = true;
	}

	if (!isShakeDown && weapon_Rotate > MinRotate) {
		weapon_Rotate -= moveWeapon * motionSpeed_;
		workAttack_.AttackTimer_++;
	}
	else if (isShakeDown && weapon_Rotate < MaxRotate) {
		weapon_Rotate += moveWeaponShakeDown * motionSpeed_;
		workAttack_.AttackTimer_++;
	}
	
	
	
	if(isDown_){
		
		move_ = { 0.0f,0.0f,moveSpeed_ * 3.0f };
		move_ = Matrix::GetInstance()->TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
			playerTransform_.translate += move_ * motionSpeed_;
		}
		weaponTransform_.translate = playerTransform_.translate;
	}


	weaponTransform_.rotate.x = weapon_Rotate;
	weaponCollisionTransform_.rotate.x = weapon_Rotate;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}

void Player::BehaviorStrongAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	workAttack_.attackParameter_ = 0;
	baseRotate_.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.z = 0.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponCollisionTransform_.rotate.x = 0;
	weaponCollisionTransform_.rotate.z = 0;
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	workAttack_.AttackTimer_ = 0;
	hitRecord_.Clear();
	WaitTime = WaitTimeBase;
	weapon_Rotate = 0.5f;
	isShakeDown = false;
}

void Player::BehaviorSecondStrongAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.strongComboNext_ = false;
	downVector.y += 0.3f;
	workAttack_.attackParameter_ = 0;
	baseRotate_.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = 1.7f;
	weaponTransform_.rotate.y = -0.2f + baseRotate_.y;
	weaponTransform_.rotate.z = 0.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
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
	baseRotate_.x = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.y = 0;
	weaponTransform_.rotate.z = 1.57f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
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
	//baseRotate_.x = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = 1.57f;
	weaponTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.z = 0.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	//weaponCollisionTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	////weaponCollisionTransform_.rotate.x = 1.57f / 2.0f;
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
	baseRotate_.x = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.x = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.y = 0;
	weaponTransform_.rotate.z = 1.57f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
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
			
			weaponTransform_.scale += {0.005f, 0.005f, 0.005f};
			weaponCollisionTransform_.scale += {0.0f, 0.07f, 0.0f};
		}
		if (weaponTransform_.scale.x>0.6|| input->GetPadButtonRelease(XINPUT_GAMEPAD_Y)){
			chargeEnd_ = true;
		}

	}

	if (weapon_Rotate <= MinRotate) {
		isShakeDown = true;
	}
	if (!isShakeDown && (weapon_Rotate > MinRotate) && chargeEnd_) {
		weapon_Rotate -= moveWeapon;
	}
	else if (isShakeDown && weapon_Rotate < MaxRotate - 0.1f) {
		weapon_Rotate += moveWeaponShakeDown;
	}
	if (isShakeDown){
		workAttack_.attackParameter_++;
	}
	if (chargeEnd_){
		weaponTransform_.rotate.x = weapon_Rotate;
		weaponCollisionTransform_.rotate.x = weapon_Rotate;
	}

	

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
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

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

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
		move_ = Matrix::GetInstance()->TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
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

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

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
			move_ = Matrix::GetInstance()->TransformNormal(move_, playerRotateMatrix_);

			
			if (!isCollisionEnemy_) {
				playerTransform_.translate += move_;
			}
			weaponTransform_.translate = playerTransform_.translate;
		}
	}
	workAttack_.attackParameter_++;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

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
		move_ = Matrix::GetInstance()->TransformNormal(move_, playerRotateMatrix_);

		if (!isCollisionEnemy_) {
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

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}
