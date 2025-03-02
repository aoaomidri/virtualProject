#include "PlayerLeaningBack.h"

void PlayerLeaningBack::ApplyGlobalVariables(){

}

void PlayerLeaningBack::InitGlobalVariables() const{

}

void PlayerLeaningBack::Initialize(){
	BasePlayerState::Initialize();
	context_.isAttackJump_ = true;
	isHit_ = false;
}

void PlayerLeaningBack::Update(const Vector3& cameraRotate){
	cameraRotate;
	//床に着地
	if (isHit_){
		if (context_.isOnFloor_) {
			PlayerStateManager::GetInstance()->ChangeState(StateName::Root);
		}
		else {
			//見た目の処理
			Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
			const char* groupName = "PlayerAttackParameter";
			context_.appearanceTransform_.rotate.x -= adjustment_item->GetfloatValue(groupName, "RotateXSpeed") * GameTime::timeScale_ * rotateSpeed_;
			if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A) or input_->GetPadButtonTriger(XINPUT_GAMEPAD_B)
				or input_->GetPadButtonTriger(XINPUT_GAMEPAD_X) or input_->GetPadButtonTriger(XINPUT_GAMEPAD_Y)){
				context_.isJump_ = true;
				PlayerStateManager::GetInstance()->ChangeState(StateName::Root);
			}
		}
	}
	else {
		isHit_ = true;
	}
	
}

