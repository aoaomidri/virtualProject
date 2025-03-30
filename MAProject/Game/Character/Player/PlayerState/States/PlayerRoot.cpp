#include "PlayerRoot.h"
#include"LockOn.h"

void PlayerRoot::Initialize(){
	BasePlayerState::Initialize();
	context_.workDash_.isDash_ = false;
	GameTime::ReverseTimeChange();
	context_.move_ = { 0.0f,0.0f,0.0f };
	context_.workAttack_.comboIndex_ = 0;
	context_.workAvoidAttack_.justAvoidAttackTimer_ = 0;
	context_.workAttack_.trailResetFlug_ = true;
	context_.weaponParameter_.weaponCollisionTransform_.translate.y = kWeaponRootTranslate_;
}

void PlayerRoot::Update(const Vector3& cameraRotate){
	if (context_.isJump_){
		context_.isJump_ = false;
	}
	if (context_.isAttackJump_) {
		context_.isAttackJump_ = false;
	}
	context_.cameraRotate_ = cameraRotate;
	context_.frontVec_ = context_.postureVec_;

	//ダッシュを発動
	if (input_->GetPadButtonTriger(Input::GamePad::RB) && context_.dashCoolTime_ <= 0) {
		PlayerStateManager::GetInstance()->ChangeState(StateName::Dash);
		return;
	}
	//弱攻撃を発動
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_X)) {
		audio_->PlayAudio(attackMotionSE_, seVolume_, false);
		context_.workAttack_.comboIndex_ = 1;
		PlayerStateManager::GetInstance()->ChangeState(StateName::Attack);
		context_.isDissolve_ = false;
		context_.weaponParameter_.weaponThreshold_ = 0.0f;
		return;
	}
	//強攻撃を発動
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_Y)) {
		context_.workAttack_.comboIndex_ = 1;
		PlayerStateManager::GetInstance()->ChangeState(StateName::StrongAttack);
		context_.isDissolve_ = false;
		context_.weaponParameter_.weaponThreshold_ = 0.0f;
		return;
	}
	//必殺攻撃を発動
	if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_B)) {
		PlayerStateManager::GetInstance()->ChangeState(StateName::SpecialAttack);
		context_.isDissolve_ = false;
		context_.weaponParameter_.weaponThreshold_ = 0.0f;
		return;
	}

	/*自機の移動*/	
	context_.move_.z = input_->GetPadLStick().y * context_.moveSpeed_;
	if (abs(context_.move_.z) < context_.moveLimitMinimum_) {
		context_.move_.z = 0;
	}
	context_.move_.x = input_->GetPadLStick().x * context_.moveSpeed_;
	if (abs(context_.move_.x) < context_.moveLimitMinimum_) {
		context_.move_.x = 0;
	}
	//カメラによってmoveの値を補正
	Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(cameraRotate);
	context_.move_ = Matrix::TransformNormal(context_.move_, newRotateMatrix);
	context_.move_.y = 0.0f;	
	context_.move_ = Vector3::Mutiply(Vector3::Normalize(context_.move_), context_.moveSpeed_ * context_.moveCorrection_);
	context_.move_.y = 0.0f;

	Matrix4x4 directionTodirection_;
	if (context_.move_.x != 0.0f || context_.move_.z != 0.0f) {
		//移動ベクトルがあれば
		context_.postureVec_ = context_.move_;

		directionTodirection_.DirectionToDirection(Vector3::Normalize(context_.frontVec_), Vector3::Normalize(context_.postureVec_));
		context_.playerRotateMatrix_ = Matrix::Multiply(context_.playerRotateMatrix_, directionTodirection_);

	}
	else if (lockOn_ && lockOn_->ExistTarget()) {
		//対象がいれば注目する
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPos - context_.playerTransform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		context_.postureVec_ = sub;

		directionTodirection_.DirectionToDirection(Vector3::Normalize(context_.frontVec_), Vector3::Normalize(context_.postureVec_));
		context_.playerRotateMatrix_ = Matrix::Multiply(context_.playerRotateMatrix_, directionTodirection_);
	}
	
	//次のフレームを仮に計算し制御
	Vector3 NextPos = context_.playerTransform_.translate + context_.move_;
	if (NextPos.x >= context_.limitPos_.x or NextPos.x <= context_.limitPos_.y) {
		context_.move_.x = 0;
	}
	if (NextPos.z >= context_.limitPos_.x or NextPos.z <= context_.limitPos_.y) {
		context_.move_.z = 0;
	}
	context_.playerTransform_.translate += context_.move_ * GameTime::timeScale_;

	/*武器の浮遊*/
	context_.floatSin_ += context_.floatSpeed_ * GameTime::timeScale_;
	if (context_.floatSin_ >= (std::numbers::pi * 2.0f)) {
		context_.floatSin_ = 0.0f;
	}
	//武器の消滅処理をしていなければ武器のSRTを更新
	if (!context_.isDissolve_) {
		if (!context_.isWeaponDebugFlug_) {
			context_.weaponParameter_.addPosition_.y = context_.weaponParameter_.kFloatHeight_ * std::sinf(context_.floatSin_);
			context_.weaponParameter_.weapon_offset_Base_ = context_.weaponParameter_.weapon_offset_RootBase_;

			 context_.weaponParameter_.weaponTransform_.rotate.x = (float)(std::numbers::pi);
			 context_.weaponParameter_.weaponTransform_.rotate.y = 0;
			 context_.weaponParameter_.weaponTransform_.rotate.z = 0;
		}
		context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_;
		Vector3 weaponRotateVec = context_.weaponParameter_.weaponTransform_.rotate;
		Matrix4x4 weaponRotateMatrix = Matrix::MakeRotateMatrix(weaponRotateVec);
		weaponRotateMatrix = Matrix::Multiply(weaponRotateMatrix, context_.playerRotateMatrix_);
		context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponRotateMatrix);
		context_.weaponParameter_.weaponTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
		Vector3 addPosRotate{};
		addPosRotate = Matrix::TransformNormal(context_.weaponParameter_.addPosition_, context_.playerRotateMatrix_);
		context_.weaponParameter_.weaponTransform_.translate += addPosRotate * GameTime::timeScale_;
	}
	if (GameTime::timeScale_ != 0.0f) {
		context_.trail_->SetPos(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	}	
}
