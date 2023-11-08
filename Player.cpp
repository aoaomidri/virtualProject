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

	playerTransform_ = {
		.scale = {0.3f,0.3f,0.3f},
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

	playerMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(playerTransform_.scale, playerTransform_.rotate, playerTransform_.translate);

	playerOBB_.center = playerTransform_.translate;
	playerOBB_.size = playerTransform_.scale;
	Matrix4x4 playerRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(playerTransform_.rotate);
	SetOridentatios(playerOBB_, playerRotateMatrix);
	if (playerTransform_.translate.y <= -5.0f) {
		playerTransform_ = {
			.scale = {0.3f,0.3f,0.3f},
			.rotate = {0.0f,0.0f,0.0f},
			.translate = {0.0f,3.8f,0.0f}
		};
	}
	
}

void Player::Draw(TextureManager* textureManager, const Transform& cameraTransform){
	playerModel_->Update(playerMatrix_, cameraTransform);
	playerModel_->Draw(textureManager->SendGPUDescriptorHandle(4));
}

void Player::DrawImgui(){
	ImGui::Begin("プレイヤー");
	ImGui::Text("ダッシュのクールタイム = %d", dashCoolTime);
	ImGui::End();
}

void Player::BehaviorRootInitialize(){
	move_ = { 0.0f,0.0f,0.0f };
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
		playerTransform_.translate.y -= 0.03f;
	}
	if (dashCoolTime != 0) {
		dashCoolTime -= 1;
	}
	

	if (input->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER) && dashCoolTime <= 0) {
		behaviorRequest_ = Behavior::kDash;
	}
}

void Player::BehaviorAttackInitialize(){

}

void Player::BehaviorAttackUpdate(){

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
