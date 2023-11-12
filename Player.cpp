#include "Player.h"

void Player::ApplyGlobalVariables() {
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Player";

	kDashSpeed = adjustment_item->GetfloatValue(groupName, "DashSpeed");
	kDashCoolTime = adjustment_item->GetIntValue(groupName, "DashCoolTime");
}

void Player::Initislize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){
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
}

void Player::Update(Input* input){
	ApplyGlobalVariables();

	DrawImgui();

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
		BehaviorAttackUpdate();
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	
	}

	

	playerOBB_.center = playerTransform_.translate;
	playerOBB_.size = playerTransform_.scale;
	Matrix4x4 playerRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(playerTransform_.rotate);
	SetOridentatios(playerOBB_, playerRotateMatrix);
	
	
	weaponOBB_.center = weaponCollisionTransform_.translate;
	weaponOBB_.size = weaponCollisionTransform_.scale;
	Matrix4x4 weaponRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	SetOridentatios(weaponOBB_, weaponRotateMatrix);

	playerMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(playerTransform_.scale, playerTransform_.rotate, playerTransform_.translate);
	weaponMatrix_= Matrix::GetInstance()->MakeAffineMatrix(weaponTransform_.scale, weaponTransform_.rotate, weaponTransform_.translate);
	weaponCollisionMatrix_= Matrix::GetInstance()->MakeAffineMatrix(weaponCollisionTransform_.scale, weaponCollisionTransform_.rotate, weaponCollisionTransform_.translate);

	if (playerTransform_.translate.y <= -5.0f) {
		Respawn();
	}
	
}

void Player::Draw(TextureManager* textureManager, const Transform& cameraTransform){
	playerModel_->Update(playerMatrix_, cameraTransform);
	playerModel_->Draw(textureManager->SendGPUDescriptorHandle(4));

	weaponModel_->Update(weaponMatrix_, cameraTransform);
	weaponModel_->Draw(textureManager->SendGPUDescriptorHandle(7));

	weaponCollisionModel_->Update(weaponCollisionMatrix_, cameraTransform);
	weaponCollisionModel_->Draw(textureManager->SendGPUDescriptorHandle(7));
}

void Player::DrawImgui(){
	ImGui::Begin("プレイヤー");
	ImGui::Text("ダッシュのクールタイム = %d", dashCoolTime);
	ImGui::DragFloat3("武器の座標", &weaponCollisionTransform_.translate.x, 0.1f);
	ImGui::DragFloat3("武器の回転", &weaponCollisionTransform_.rotate.x, 0.1f);
	ImGui::DragFloat3("武器の大きさ", &weaponCollisionTransform_.scale.x, 0.1f);
	ImGui::End();
}

void Player::onFlootCollision(OBB obb){
	playerTransform_.translate.y = playerOBB_.size.y + obb.size.y;
}

void Player::Respawn(){
	playerTransform_ = {
			.scale = {0.3f,0.3f,0.3f},
			.rotate = {0.0f,0.0f,0.0f},
			.translate = {0.0f,3.8f,0.0f}
	};
}

void Player::BehaviorRootInitialize(){
	move_ = { 0.0f,0.0f,0.0f };
	weaponTransform_.translate.y = 1.0f;
	weaponTransform_.rotate.x = 0.0f;
	weaponCollisionTransform_.translate.y = 1.0f;
	weaponCollisionTransform_.rotate.x = 0.0f;
}

void Player::BehaviorRootUpdate(Input* input){
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

	Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(cameraTransform_->rotate);
	move_ = Matrix::GetInstance()->TransformNormal(move_, newRotateMatrix);
	move_.y = 0.0f;

	if (move_.x != 0.0f || move_.z != 0.0f) {
		playerTransform_.rotate.y = std::atan2(move_.x, move_.z);
	}

	playerTransform_.translate += move_;
	if (isDown_) {
		playerTransform_.translate.y -= 0.07f;
	}
	if (dashCoolTime != 0) {
		dashCoolTime -= 1;
	}
	weaponTransform_.rotate.y = playerTransform_.rotate.y;
	weaponTransform_.translate = playerTransform_.translate;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);
	weaponCollisionTransform_.rotate.y = playerTransform_.rotate.y;
	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	if (input->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER) && dashCoolTime <= 0) {
		behaviorRequest_ = Behavior::kDash;
	}
	if (input->GetPadButtonDown(XINPUT_GAMEPAD_B)){
		behaviorRequest_ = Behavior::kAttack;
	}
}

void Player::BehaviorAttackInitialize(){
	
	//weaponTransform_.translate.y = 1.0f;
	WaitTime = WaitTimeBase;
	weapon_Rotate = 0.5f;
	isShakeDown = false;
}

void Player::BehaviorAttackUpdate(){
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

	

	weaponTransform_.rotate.x = weapon_Rotate;
	weaponCollisionTransform_.rotate.x = weapon_Rotate;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(weaponCollisionTransform_.rotate);
	Weapon_offset = Matrix::GetInstance()->TransformNormal(Weapon_offset_Base, weaponCollisionRotateMatrix);

	weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;

	if (WaitTime <= 0) {
		behaviorRequest_ = Behavior::kRoot;
	}
	//weapon_Rotate==0.0fの時arm_Rotate-3.15f
}

void Player::BehaviorDashInitialize(){
	workDash_.dashParameter_ = 0;
}

void Player::BehaviorDashUpdate(){
	Matrix4x4 newRotateMatrix_ = Matrix::GetInstance()->MakeRotateMatrixY(playerTransform_.rotate);
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
