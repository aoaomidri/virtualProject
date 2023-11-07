#include "Player.h"

void Player::Initislize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){
	playerModel_ = std::make_unique<Object3D>();
	playerModel_->Initialize(device, commandList, "box");

	playerTransform_ = {
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.8f,0.0f}
	};
}

void Player::Update(Input* input){
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

	Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(cameraTransform_.rotate);
	move_ = Matrix::GetInstance()->TransformNormal(move_, newRotateMatrix);

	move_.y = 0.0f;
	if (move_.x != 0.0f || move_.z != 0.0f) {
		playerTransform_.rotate.y = std::atan2(move_.x, move_.z);
	}

	


	playerTransform_.translate += move_;
	if (isDown_) {
		playerTransform_.translate.y -= 0.03f;
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

void Player::Draw(TextureManager* textureManager){
	playerModel_->Update(playerMatrix_, cameraTransform_);
	playerModel_->Draw(textureManager->SendGPUDescriptorHandle(4));
}

void Player::DrawImgui(){

}
