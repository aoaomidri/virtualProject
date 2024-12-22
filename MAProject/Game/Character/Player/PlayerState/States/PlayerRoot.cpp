#include "PlayerRoot.h"
#include"LockOn.h"

void PlayerRoot::Initialize(){
	IPlayerState::Initialize();
	//context_.isDash_ = false;
	//GameTime::ReverseTimeChange();
	//context_.move_ = { 0.0f,0.0f,0.0f };
	//context_.workAttack_.comboIndex = 0;

	//weaponCollisionTransform_.translate.y = kWeaponRootTranslate_;

}

void PlayerRoot::Update(){
	//context_.frontVec_ = context_.postureVec_;
	///*自機の移動*/
	//context_.move_.z = input_->GetPadLStick().y * context_.moveSpeed_;
	//if (abs(context_.move_.z) < 0.005f) {
	//	context_.move_.z = 0;
	//}
	//context_.move_.x = input_->GetPadLStick().x * context_.moveSpeed_;
	//if (abs(context_.move_.x) < 0.005f) {
	//	context_.move_.x = 0;
	//}

	//if (viewProjection_) {
	//	Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(viewProjection_->rotation_);
	//	context_.move_ = Matrix::TransformNormal(context_.move_, newRotateMatrix);
	//	context_.move_.y = 0.0f;
	//}

	//context_.move_ = Vector3::Mutiply(Vector3::Normalize(context_.move_), context_.moveSpeed_ * 3.0f);
	//context_.move_.y = 0.0f;

	//if (context_.move_.x != 0.0f || context_.move_.z != 0.0f) {
	//	context_.postureVec_ = context_.move_;
	//	Matrix4x4 directionTodirection_;
	//	directionTodirection_.DirectionToDirection(Vector3::Normalize(context_.frontVec_), Vector3::Normalize(context_.postureVec_));
	//	context_.playerRotateMatrix_ = Matrix::Multiply(context_.playerRotateMatrix_, directionTodirection_);
	//	
	//}
	//else if (lockOn_ && lockOn_->ExistTarget()) {
	//	Vector3 lockOnPos = lockOn_->GetTargetPosition();
	//	Vector3 sub = lockOnPos - context_.playerTransform_.translate;
	//	sub.y = 0;
	//	sub = Vector3::Normalize(sub);
	//	context_.postureVec_ = sub;

	//	Matrix4x4 directionTodirection_;
	//	directionTodirection_.DirectionToDirection(Vector3::Normalize(context_.frontVec_), Vector3::Normalize(context_.postureVec_));
	//	context_.playerRotateMatrix_ = Matrix::Multiply(context_.playerRotateMatrix_, directionTodirection_);
	//}

	//if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A) && !context_.isDown_) {
	//	context_.downVector_.y += context_.jumpPower_;

	//}
	//Vector3 NextPos = context_.playerTransform_.translate + context_.move_;

	//if (NextPos.x >= 95.0f or NextPos.x <= -95.0f) {
	//	context_.move_.x = 0;
	//}
	//if (NextPos.z >= 95.0f or NextPos.z <= -95.0f) {
	//	context_.move_.z = 0;
	//}

	//context_.playerTransform_.translate += context_.move_;

	//if (context_.isDown_) {
	//	context_.downVector_.y += context_.downSpeed_;
	//}

	//if (context_.dashCoolTime_ != 0) {
	//	context_.dashCoolTime_ -= 1;
	//}
	//context_.floatSin_ += context_.floatSpeed_;
	//if (context_.floatSin_ >= (std::numbers::pi * 2.0f)) {
	//	context_.floatSin_ = 0.0f;
	//}
	//context_.playerTransform_.translate.y += context_.downVector_.y;
	//if (!context_.isDissolve_) {

	//	if (!context_.isWeaponDebugFlug_) {
	//		addPosition_.y = kFloatHeight_ * std::sinf(context_.floatSin_);
	//		weapon_offset_Base_ = { 0.4f,-1.8f,1.0f };

	//		weaponTransform_.rotate.x = 3.14f;
	//		weaponTransform_.rotate.y = 0;
	//		weaponTransform_.rotate.z = 0;
	//	}
	//	weaponCollisionTransform_.scale = { 0.9f,3.0f,0.9f };

	//	Vector3 weaponRotateVec = weaponTransform_.rotate;
	//	Matrix4x4 weaponRotateMatrix = Matrix::MakeRotateMatrix(weaponRotateVec);
	//	weaponRotateMatrix = Matrix::Multiply(weaponRotateMatrix, context_.playerRotateMatrix_);
	//	weapon_offset_ = Matrix::TransformNormal(weapon_offset_Base_, weaponRotateMatrix);
	//	weaponTransform_.translate = context_.playerTransform_.translate + weapon_offset_;
	//	Vector3 addPosRotate{};
	//	addPosRotate = Matrix::TransformNormal(addPosition_, context_.playerRotateMatrix_);

	//	weaponTransform_.translate += addPosRotate;
	//}

	////weaponCollisionTransform_.translate = playerTransform_.translate + Weapon_offset;
	//trail_->SetPos(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));

	//if (input_->GetPadButtonTriger(Input::GamePad::RB) && context_.dashCoolTime_ <= 0) {
	//	PlayerStateManager::GetInstance()->ChangeState(StateName::Dash);
	//}

	//if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_X) && !context_.isDown_) {
	//	context_.workAttack_.comboIndex = 1;
	//	PlayerStateManager::GetInstance()->ChangeState(StateName::Attack);
	//	context_.isDissolve_ = false;
	//	weaponThreshold_ = 0.0f;
	//}
	//if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_Y) && !context_.isDown_) {
	//	context_.workAttack_.comboIndex = 1;
	//	PlayerStateManager::GetInstance()->ChangeState(StateName::StrongAttack);
	//	context_.isDissolve_ = false;
	//	weaponThreshold_ = 0.0f;
	//}
}
