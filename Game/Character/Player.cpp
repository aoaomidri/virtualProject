#include "Player.h"
#include"../Camera/LockOn.h"
#include "../../externals/imgui/imgui.h"

const std::array<Player::ConstAttack, Player::ConboNum>
Player::kConstAttacks_ = {
	{
	{0,0,20,0,0.0f,0.0f,0.15f},
	{15,10,15,0,0.2f,0.0f,0.15f},
	{15,10,15,30,0.2f,0.0f,0.15f}
	}
};

void Player::ApplyGlobalVariables() {
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";

	kDashSpeed = adjustment_item->GetfloatValue(groupName, "DashSpeed");
	kDashCoolTime = adjustment_item->GetIntValue(groupName, "DashCoolTime");
}

void Player::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "DashCoolTime", kDashCoolTime);
	adjustment_item->AddItem(groupName, "DashSpeed", kDashSpeed);


	playerModel_ = std::make_unique<Object3D>();
	playerModel_->Initialize(device, commandList, "box");

	weaponModel_ = std::make_unique<Object3D>();
	weaponModel_->Initialize(device, commandList, "weapon");

	weaponCollisionModel_ = std::make_unique<Object3D>();
	weaponCollisionModel_->Initialize(device, commandList, "box");
	//Weapon

	playerTransform_ = {
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.8f,0.0f}
	};

	weaponTransform_ = {
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	weaponCollisionTransform_ = {
		.scale = {0.3f,1.7f,0.3f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.8f,0.0f}
	};

	dashCoolTime = kDashCoolTime;

	playerRotateMatrix_ = Matrix::GetInstance()->MakeIdentity4x4();

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };
}

void Player::Update(Input* input){
	ApplyGlobalVariables();

	

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
		BehaviorRootUpdate(input);
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate(input);
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	
	}
	playerScaleMatrix_ = Matrix::GetInstance()->MakeScaleMatrix(playerTransform_.scale);
	playerTransformMatrix_ = Matrix::GetInstance()->MakeTranslateMatrix(playerTransform_.translate);

	playerOBB_.center = playerTransform_.translate;
	playerOBB_.size = playerTransform_.scale;
	SetOridentatios(playerOBB_, playerRotateMatrix_);
	
	
	weaponOBB_.center = weaponCollisionTransform_.translate;
	weaponOBB_.size = weaponCollisionTransform_.scale;
	Matrix4x4 weaponRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	SetOridentatios(weaponOBB_, weaponRotateMatrix);

	playerMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(playerScaleMatrix_, playerRotateMatrix_, playerTransformMatrix_);
	weaponMatrix_= Matrix::GetInstance()->MakeAffineMatrix(weaponTransform_.scale, weaponTransform_.rotate, weaponTransform_.translate);
	weaponCollisionMatrix_= Matrix::GetInstance()->MakeAffineMatrix(weaponCollisionTransform_.scale, weaponCollisionTransform_.rotate, weaponCollisionTransform_.translate);

	

	if (playerTransform_.translate.y <= -5.0f) {
		Respawn();
	}
	DrawImgui();
}

void Player::Draw(TextureManager* textureManager, const ViewProjection& viewProjection){
	playerModel_->Update(playerMatrix_, viewProjection);
	playerModel_->Draw(textureManager->SendGPUDescriptorHandle(4));

	if (behavior_==Behavior::kAttack){
		weaponModel_->Update(weaponMatrix_, viewProjection);
		weaponModel_->Draw(textureManager->SendGPUDescriptorHandle(7));

		/*weaponCollisionModel_->Update(weaponCollisionMatrix_, viewProjection);
		weaponCollisionModel_->Draw(textureManager->SendGPUDescriptorHandle(7));*/
	}
	
}

void Player::DrawImgui(){
	ImGui::Begin("プレイヤー");
	ImGui::Text("ダッシュのクールタイム = %d", dashCoolTime);
	ImGui::Text("攻撃時間 = %d", workAttack_.AttackTimer_);
	ImGui::Text("今のコンボ段階 = %d", workAttack_.comboIndex_);
	ImGui::DragFloat3("武器の座標", &weaponCollisionTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("武器の回転", &weaponCollisionTransform_.rotate.x, 0.1f);
	ImGui::DragFloat3("武器の大きさ", &weaponCollisionTransform_.scale.x, 0.1f);
	ImGui::DragFloat("武器の回転", &weapon_Rotate, 0.1f);
	ImGui::End();
	
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
			.scale = {0.3f,0.3f,0.3f},
			.rotate = {0.0f,0.0f,0.0f},
			.translate = {0.0f,5.8f,0.0f}
	};
	downVector = { 0.0f,0.0f,0.0f };
}

void Player::BehaviorRootInitialize(){
	move_ = { 0.0f,0.0f,0.0f };
	weaponTransform_.translate.y = 1.0f;
	weaponCollisionTransform_.translate.y = 1.0f;
	
	
}

void Player::BehaviorRootUpdate(Input* input){
	frontVec_ = postureVec_;
	/*自機の移動*/
	if (input->PushUp()) {
		move_.z = moveSpeed_;

	}
	else if (input->PushDown()) {
		move_.z = -moveSpeed_;
	}
	else {
		move_.z = input->GetPadLStick().y * moveSpeed_;
	}

	if (input->PushRight()) {
		move_.x = moveSpeed_;
	}
	else if (input->PushLeft()) {
		move_.x = -moveSpeed_;
	}
	else {
		move_.x = input->GetPadLStick().x * moveSpeed_;
	}
	
	Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(viewProjection_->rotation_);
	move_ = Matrix::GetInstance()->TransformNormal(move_, newRotateMatrix);
	move_ = Vector3::Mutiply(Vector3::Normalize(move_), moveSpeed_);
	move_.y = 0.0f;
	

	if (move_.x != 0.0f || move_.z != 0.0f) {
		postureVec_ = move_;
		
		Matrix4x4 directionTodirection_= Matrix::GetInstance()->DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::GetInstance()->Multiply(playerRotateMatrix_, directionTodirection_);
		
	}
	else if(lockOn_&&lockOn_->ExistTarget()){
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPos - playerTransform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		postureVec_ = sub;

		Matrix4x4 directionTodirection_ = Matrix::GetInstance()->DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::GetInstance()->Multiply(playerRotateMatrix_, directionTodirection_);
	}

	if (input->GetPadButtonDown(XINPUT_GAMEPAD_A) && !isDown_) {
		downVector.y += jumpPower;
		playerTransform_.translate.y += downVector.y;
	}

	playerTransform_.translate += move_;
	
	if (isDown_) {
		downVector.y += downSpeed;
	}
	else {
		
	}
	if (dashCoolTime != 0) {
		dashCoolTime -= 1;
	}
	playerTransform_.translate.y += downVector.y;
	

	if (input->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER) && dashCoolTime <= 0) {
		behaviorRequest_ = Behavior::kDash;
	}
	if (input->GetPadButtonDown(XINPUT_GAMEPAD_B) && !isDown_) {
		workAttack_.comboIndex_ = 0;
		behaviorRequest_ = Behavior::kAttack;
	}
	
}

void Player::BehaviorAttackInitialize(){
	
	workAttack_.comboNext_ = false;
	workAttack_.attackParameter_ = 0;
	baseRotate_.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.z = 0.0f;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	workAttack_.AttackTimer_ = 0;

	WaitTime = WaitTimeBase;
	weapon_Rotate = 0.5f;
	isShakeDown = false;
}

void Player::BehaviorThirdAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.attackParameter_ = 0;
	baseRotate_.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	weaponTransform_.rotate.y = 0;
	weaponTransform_.rotate.z = 1.57f ;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate.y = Matrix::GetInstance()->RotateAngleYFromMatrix(playerRotateMatrix_);
	//weaponCollisionTransform_.rotate.x = 1.57f / 2.0f;
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	workAttack_.AttackTimer_ = 0;

	WaitTime = WaitTimeBase;
	weapon_Rotate = 3.0f;
	isShakeDown = false;
}

void Player::BehaviorAttackUpdate(Input* input){
	frontVec_ = postureVec_;
	


	if (++workAttack_.attackParameter_ >= 35) {
		if (workAttack_.comboNext_) {
			workAttack_.comboIndex_++;
			BehaviorAttackInitialize();
			if (workAttack_.comboIndex_ == 2) {
				BehaviorThirdAttackInitialize();
			}
		}
		else {
			workAttack_.attackParameter_ = 0;
			
			behaviorRequest_ = Behavior::kRoot;
		}
	}

	if (lockOn_ && lockOn_->ExistTarget()) {
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPos - playerTransform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		postureVec_ = sub;

		Matrix4x4 directionTodirection_ = Matrix::GetInstance()->DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		playerRotateMatrix_ = Matrix::GetInstance()->Multiply(playerRotateMatrix_, directionTodirection_);
	}

	switch (workAttack_.comboIndex_){
	case 0:
		AttackMotion();
		break;
	case 1:
		AttackMotion();
		break;
	case 2:
	default:
		thirdAttackMotion();
		break;	
	}
	//コンボ上限に達していない
	if (workAttack_.comboIndex_ + 1 < ConboNum) {
		if (input->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
			//コンボ有効
			workAttack_.comboNext_ = true;
		}
	}


	if (workAttack_.attackParameter_ >= 35) {
		if (workAttack_.comboNext_) {
			

		}
		else {
			workAttack_.attackParameter_ = 0;
			
			behaviorRequest_ = Behavior::kRoot;
		}
	}

	/*if (WaitTime <= 0) {
		behaviorRequest_ = Behavior::kRoot;
	}*/
	//weapon_Rotate==0.0fの時arm_Rotate-3.15f
}

void Player::BehaviorDashInitialize(){
	workDash_.dashParameter_ = 0;
}

void Player::BehaviorDashUpdate(){
	Matrix4x4 newRotateMatrix_ = playerRotateMatrix_;
	move_ = { 0, 0, moveSpeed_ * kDashSpeed };

	move_ = Matrix::GetInstance()->TransformNormal(move_, newRotateMatrix_);

	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 5;

	playerTransform_.translate += move_;

	//既定の時間経過で通常状態に戻る
	if (++workDash_.dashParameter_ >= behaviorDashTime) {
		dashCoolTime = kDashCoolTime;
		behaviorRequest_ = Behavior::kRoot;
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
		weapon_Rotate -= moveWeapon;
	}
	else if (isShakeDown && weapon_Rotate < MaxRotate) {
		weapon_Rotate += moveWeaponShakeDown;
	}
	if (isShakeDown) {
		workAttack_.AttackTimer_++;
	}
	/*if (workAttack_.attackParameter_ >= 35) {
		workAttack_.comboIndex_++;
	}*/

	weaponTransform_.rotate.x = weapon_Rotate;
	weaponCollisionTransform_.rotate.x = weapon_Rotate;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}

void Player::thirdAttackMotion(){
	workAttack_.AttackTimer_++;
	if (workAttack_.AttackTimer_ >= 20) {
		isShakeDown = true;
	}
	if (weapon_Rotate <= MaxRotateY) {
		WaitTime -= 1;
		weapon_Rotate = MaxRotateY;
	}

	if (!isShakeDown) {
		weapon_Rotate += moveWeapon / 2.0f;
	}
	else if (isShakeDown ) {
		weapon_Rotate -= moveWeaponShakeDown * 1.5f;
	}
	if (isShakeDown) {
		//.workAttack_.AttackTimer_++;
	}
	

	weaponTransform_.rotate.x = weapon_Rotate + baseRotate_.x;
	weaponCollisionTransform_.rotate.x = weapon_Rotate + baseRotate_.x;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
}
