#include "PlayerSpecialAttack.h"
#include"Ease/Ease.h"

void PlayerSpecialAttack::ApplyGlobalVariables() {

}

void PlayerSpecialAttack::InitGlobalVariables() const {

}

void PlayerSpecialAttack::Initialize() {
	BasePlayerState::Initialize();
	InitializeMove();
	InitializeAttack();
}

void PlayerSpecialAttack::InitializeMove() {
	GameTime::SlowDownTime(slowTime_, slow_);
	isAvoidAttack_ = false;
	context_.workAttack_.type_ = HitRecord::KnockbackType::Center;
	/*移動部分の初期化*/
	Vector3 subNorm = Vector3::Normalize(context_.frontVec_);
	context_.postureVec_ = subNorm;

	Matrix4x4 directionTodirection_;
	directionTodirection_.DirectionToDirection(Vector3::Normalize(context_.frontVec_), Vector3::Normalize(context_.postureVec_));
	context_.playerRotateMatrix_ = Matrix::Multiply(context_.playerRotateMatrix_, directionTodirection_);
	context_.move_ = subNorm * context_.workAvoidAttack_.avoidSpeed_;
	context_.move_.y = 0;
	context_.workAvoidAttack_.avoidTimer_ = 0.0f;
	context_.workAvoidAttack_.isChangeEndAttack_ = false;
	context_.workDash_.isDash_ = false;
}

void PlayerSpecialAttack::InitializeAttack() {
	/*武器や攻撃関連の初期化*/
	/*剣を引いて突くような動作*/
	
	context_.workAttack_.attackParameter_ = 0;
	context_.weaponParameter_.weaponTransform_.rotate = justAvoidWeaponRotate_;
	context_.weaponParameter_.weaponTransform_.translate = context_.playerTransform_.translate;
	context_.weaponParameter_.weapon_offset_Base_ = { 0.0f,0.0f,0.0f };
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.workAttack_.trailResetFlug_ = true;
	context_.workAttack_.hitRecordRestFlug_ = true;
	context_.workAttack_.AttackTimer_ = 0;
	easeT_ = 1.0f;
	addEaseT_ = addEaseSpeed_;
	waitTime_ = waitTimeBase_;
	context_.weaponParameter_.addPosition_.y = 0.0f;
	context_.isTrail_ = false;
	isThrust_ = false;
	context_.workAttack_.isShakeDown_ = false;
	context_.workAttack_.isEndAttack_ = false;
	context_.weaponParameter_.weaponCollisionTransform_.scale = { 0.0f,0.0f,0.0f };
}

void PlayerSpecialAttack::Update(const Vector3& cameraRotate) {
	context_.cameraRotate_ = cameraRotate;
	context_.frontVec_ = context_.postureVec_;
	if (!isAvoidAttack_) {
		if (!isThrust_) {
			//突く前の予備動作
			easeT_ -= (addEaseT_ * justAvoidEaseMagnification_.x);
			if (easeT_ < 0.0f) {
				easeT_ = 0.0f;
				isThrust_ = true;
			}
		}
		else {
			//突きのeaseの更新
			easeT_ += (addEaseT_ * justAvoidEaseMagnification_.y);
			if (easeT_ > 1.0f) {
				//構え終わったら攻撃に移る
				GameTime::ReverseTimeChange();
				easeT_ = 1.0f;
				waitTime_ -= 1;
				addEaseT_ = 0.0f;
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				context_.workAvoidAttack_.justAvoidAttackTimer_ = 0;
				context_.isJustAvoid_ = false;
				isAvoidAttack_ = true;
			}
		}
		context_.weaponParameter_.weapon_offset_Base_.x = Ease::Easing(Ease::EaseName::EaseInExpo, justAvoidEaseStart_.x, justAvoidEaseEnd_.x, easeT_);
		context_.weaponParameter_.weapon_offset_Base_.y = Ease::Easing(Ease::EaseName::EaseInExpo, justAvoidEaseStart_.y, justAvoidEaseEnd_.y, easeT_);
	}
	else {
		//突き攻撃の動作処理
		context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_;
		Vector3 lockOnPos = context_.lockOnPos_;
		Vector3 sub = lockOnPos - context_.playerTransform_.translate;
		Vector3 NextPos = context_.playerTransform_.translate + (context_.move_);
		if (NextPos.x >= context_.limitPos_.x or NextPos.x <= context_.limitPos_.y) {
			context_.move_.x = 0;
		}
		if (NextPos.z >= context_.limitPos_.x or NextPos.z <= context_.limitPos_.y) {
			context_.move_.z = 0;
		}
		context_.workAvoidAttack_.avoidHitTimer_ += GameTime::deltaTime_;
		//連続ヒットの調整
		if (context_.workAvoidAttack_.avoidHitTimer_ > context_.workAvoidAttack_.avoidHitTimerBase_) {
			context_.workAvoidAttack_.avoidHitTimer_ = 0.0f;
			context_.workAttack_.hitRecordRestFlug_ = true;
		}
		context_.playerTransform_.translate += context_.move_;
		context_.workAvoidAttack_.avoidTimer_ += GameTime::deltaTime_;
		if (context_.workAvoidAttack_.avoidTimerBase_ < context_.workAvoidAttack_.avoidTimer_) {
			stateManager_->ChangeState(StateName::Root);
		}
	}
	//座標や回転の更新
	Matrix4x4 directionTodirection_;
	directionTodirection_.DirectionToDirection(Vector3::Normalize(context_.frontVec_), Vector3::Normalize(context_.postureVec_));
	context_.playerRotateMatrix_ = Matrix::Multiply(context_.playerRotateMatrix_, directionTodirection_);
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	weaponCollisionRotateMatrix = Matrix::Multiply(weaponCollisionRotateMatrix, context_.playerRotateMatrix_);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weaponCollisionTransform_.translate.y += (context_.weaponParameter_.addPosition_.y + 1.0f);
	context_.weaponParameter_.weaponTransform_.translate = context_.weaponParameter_.weaponCollisionTransform_.translate;
}
