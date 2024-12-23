#include "PlayerStateManager.h"
#include<assert.h>
#include"Adjustment_Item.h"
#include"LevelLoader/LevelLoader.h"

void PlayerStateManager::ApplyGlobalVariables(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "PlayerContextState";

	context_.jumpPower_ = adjustment_item->GetfloatValue(groupName, "JumpPower");
	context_.downSpeed_ = adjustment_item->GetfloatValue(groupName, "DownSpeed");
	context_.moveSpeed_ = adjustment_item->GetfloatValue(groupName, "MoveSpeed");
}
void PlayerStateManager::InitGlobalVariables() const{
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "PlayerContextState";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "JumpPower", context_.jumpPower_);
	adjustment_item->AddItem(groupName, "DownSpeed", context_.downSpeed_);
	adjustment_item->AddItem(groupName, "MoveSpeed", context_.moveSpeed_);
}
void PlayerStateManager::InitState() {
	context_.postureVec_ = { 0.0f,0.0f,1.0f };
	context_.frontVec_ = { 0.0f,0.0f,1.0f };
	context_.playerTransform_.translate.y = 5.0f;
	context_.playerTransform_.scale = { 1.0f,0.5f,0.7f };

	context_.weaponParameter_.weaponTransform_.scale = context_.weaponParameter_.kWeaponScale_;
	context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_;

	context_.playerRotateMatrix_ = Matrix4x4::StaticIdentity();
	context_.weaponParameter_.weaponMatrix_ = Matrix4x4::StaticIdentity();
	context_.weaponParameter_.weaponCollisionMatrix_ = Matrix4x4::StaticIdentity();
}
void PlayerStateManager::ChangeState(const StateName nextState){
	assert(nextState_ == nullptr);
	/*次の状態を生成*/
	if (nextState == StateName::Root){
		nextState_ = std::make_unique<PlayerRoot>(context_);
	}
	else if (nextState == StateName::Attack){
		nextState_ = std::make_unique<PlayerAttack>(context_);
	}
	else if (nextState == StateName::StrongAttack){
		nextState_ = std::make_unique<PlayerStrongAttack>(context_);
	}
	else if (nextState == StateName::Dash){
		nextState_ = std::make_unique<PlayerDash>(context_);
	}

	nowStateName_ = nextState;
}

void PlayerStateManager::Update(const Vector3& cameraRotate){
	/*状態切り替え処理*/
	if (nextState_) {
		//状態切り替え
		nowState_ = std::move(nextState_);
		nextState_ = nullptr;
		nowState_->SetStateManager(this);
		//次の状態を初期化する
		nowState_->Initialize();
	}
	ApplyGlobalVariables();
	//実行中の状態を更新する
	nowState_->Update(cameraRotate);
}
