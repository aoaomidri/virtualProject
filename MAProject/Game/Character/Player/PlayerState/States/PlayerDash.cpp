#include "PlayerDash.h"
#include"LockOn.h"
bool PlayerDash::isLoad_ = false;

void PlayerDash::ApplyGlobalVariables(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "PlayerDashParameter";

	dashSpeed_ = adjustment_item->GetfloatValue(groupName, "DashSpeed");
	dashCoolTimeBase_ = adjustment_item->GetIntValue(groupName, "DashCoolTimeBase");
}

void PlayerDash::InitGlobalVariables() const{
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "PlayerDashParameter";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "DashCoolTimeBase", dashCoolTimeBase_);
	adjustment_item->AddItem(groupName, "DashSpeed", dashSpeed_);
}

void PlayerDash::Initialize(){
	BasePlayerState::Initialize();
	if (!isLoad_) {
		InitGlobalVariables();
		isLoad_ = true;
	}
	context_.isAvoidAttack_ = false;
	audio_->PlayAudio(avoidSE_, seVolume_, false);
	context_.workDash_.dashParameter_ = 0;
	context_.workDash_.isDash_ = true;
}

void PlayerDash::Update(const Vector3& cameraRotate){
	ApplyGlobalVariables();
	context_.cameraRotate_ = cameraRotate;
	context_.frontVec_ = context_.postureVec_;
	Matrix4x4 newRotateMatrix_ = context_.playerRotateMatrix_;
	context_.move_ = { 0, 0, context_.moveSpeed_ * dashSpeed_ };

	context_.move_ = Matrix::TransformNormal(context_.move_, newRotateMatrix_);
	//ジャスト回避していたら追撃ができるように
	if (context_.isJustAvoid_) {
		if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_X)) {
			PlayerStateManager::GetInstance()->ChangeState(StateName::kJustAvoid);
			context_.dashCoolTime_ = dashCoolTimeBase_;
		}
	}

	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 15;

	if (!context_.isAvoidAttack_) {
		Vector3 NextPos = context_.playerTransform_.translate + (context_.move_ * GameTime::timeScale_);

		if (NextPos.x >= context_.limitPos_.x or NextPos.x <= context_.limitPos_.y) {
			context_.move_.x = 0;
		}
		if (NextPos.z >= context_.limitPos_.x or NextPos.z <= context_.limitPos_.y) {
			context_.move_.z = 0;
		}
		context_.playerTransform_.translate += context_.move_ * GameTime::timeScale_;
	}


	//既定の時間経過で通常状態に戻る
	context_.workDash_.dashParameter_ += GameTime::timeScale_;
	if (context_.workDash_.dashParameter_ >= behaviorDashTime) {
		context_.dashCoolTime_ = dashCoolTimeBase_;
		
		PlayerStateManager::GetInstance()->ChangeState(StateName::Root);
	}

	
}

