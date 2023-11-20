#include "Player.h"

void Player::Initislize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){
	playerSprite_ = std::make_unique<Sprite>();
	playerSprite_->Initialize(device, commandList);	

	playerTransform_ = {
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.8f,0.0f}
	};

}

void Player::Update(Input* input){
	DrawImgui();
	/*自機の移動*/
	if (input->PushUp()) {
		move_.y = -moveSpeed_;

	}
	else if (input->PushDown()) {
		move_.y = moveSpeed_;
	}
	else {
		move_.y = input->GetPadLStick().y * moveSpeed_;
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

	playerSprite_->position_ += move_;
	playerSprite_->Update();
}

void Player::Draw(TextureManager* textureManager){
	playerSprite_->Draw(textureManager->SendGPUDescriptorHandle(0));
}

void Player::DrawImgui(){
	ImGui::Begin("プレイヤー");
	ImGui::End();
}

void Player::MoveRight()
{
}

void Player::MoveLeft()
{
}

void Player::Respawn(){
	playerTransform_ = {
			.scale = {0.3f,0.3f,0.3f},
			.rotate = {0.0f,0.0f,0.0f},
			.translate = {0.0f,3.8f,0.0f}
	};
}


