#include "PlayerAttack.h"
#include"LockOn.h"

bool PlayerAttack::isLoad_ = false;

void PlayerAttack::ApplyGlobalVariables(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "PlayerAttackParameter";

	motionSpeed_ = adjustment_item->GetfloatValue(groupName, "MotionSpeed");
	motionDistance_ = adjustment_item->GetfloatValue(groupName, "MotionDistance");
	waitTimeBase_ = adjustment_item->GetfloatValue(groupName, "WaitTimeBase");
	rotateXSpeed_ = adjustment_item->GetfloatValue(groupName, "RotateXSpeed");
	rotateYSpeed_ = adjustment_item->GetfloatValue(groupName, "RotateYSpeed");
}

void PlayerAttack::InitGlobalVariables() const{
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "PlayerAttackParameter";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "MotionSpeed", motionSpeed_);
	adjustment_item->AddItem(groupName, "MotionDistance", motionDistance_);
	adjustment_item->AddItem(groupName, "WaitTimeBase", waitTimeBase_);
	adjustment_item->AddItem(groupName, "RotateXSpeed", rotateXSpeed_);
	adjustment_item->AddItem(groupName, "RotateYSpeed", rotateYSpeed_);
}

void PlayerAttack::Initialize(){
	BasePlayerState::Initialize();
	if (!isLoad_) {
		InitGlobalVariables();
		isLoad_ = true;
	}
	PreAttackInitialize();
	AttackInitialize();
	PostAttackInitialize();
}

void PlayerAttack::PreAttackInitialize(){
	context_.isTrail_ = true;
	context_.workAttack_.trailResetFlug_ = true;
	context_.workAttack_.isStrongHitStop_ = false;
	context_.workAttack_.comboNext_ = false;
	context_.workAttack_.strongComboNext_ = false;
	context_.workAttack_.attackParameter_= 0;
	if (context_.workAttack_.comboIndex_ != 4) {
		context_.weaponParameter_.weaponTransform_.translate = context_.playerTransform_.translate;
		context_.weaponParameter_.weapon_offset_Base_ = { 0.0f,0.0f,0.0f };
	}
	context_.weaponParameter_.weapon_offset_Base_.y = context_.weaponParameter_.kWeapon_offset_;
	context_.appearanceTransform_.rotate = { 0.0f,0.0f,0.0f };
}

void PlayerAttack::PostAttackInitialize(){
	if (context_.workAttack_.comboIndex_ != 4) {
		Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
		context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
		context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
		context_.weaponParameter_.weaponCollisionTransform_.rotate = { 0.0f,0.0f,context_.weaponParameter_.weaponTransform_.rotate.z };
		context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_;
		context_.workAttack_.isShakeDown_ = false;
	}
	else {
		context_.workAttack_.isShakeDown_ = true;
	}
	context_.workAttack_.AttackTimer_ = 0;
	context_.workAttack_.hitRecordRestFlug_ = true;
	waitTime_ = waitTimeBase_;
	context_.workAttack_.isEndAttack_ = false;
}

void PlayerAttack::AttackInitialize(){
	context_.workAttack_.type_ = HitRecord::KnockbackType::Left;
	context_.workAttack_.nextAttackTimer_ = nextAttackTimerFirst_;	
	context_.weaponParameter_.weaponTransform_.rotate.x = weaponAttackTransformRotates_[0].x;
	context_.weaponParameter_.weaponTransform_.rotate.z = weaponAttackTransformRotates_[0].z;
	context_.weaponParameter_.weapon_Rotate_ = weapon_Rotates_[0];
}

void PlayerAttack::SecondAttackInitialize(){
	context_.workAttack_.type_ = HitRecord::KnockbackType::Right;
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	context_.weaponParameter_.weaponTransform_.rotate = weaponAttackTransformRotates_[1];
	context_.weaponParameter_.weapon_Rotate_ = weapon_Rotates_[1];
}

void PlayerAttack::ThirdAttackInitialize(){
	context_.workAttack_.type_ = HitRecord::KnockbackType::Left;
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	context_.weaponParameter_.weaponTransform_.rotate = weaponAttackTransformRotates_[2];
	context_.weaponParameter_.weapon_Rotate_ = weapon_Rotates_[2];
}

void PlayerAttack::FourthAttackInitialize(){
	context_.workAttack_.type_ = HitRecord::KnockbackType::Left;
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	context_.weaponParameter_.weaponTransform_.rotate.z = weaponAttackTransformRotates_[3].z;
}

void PlayerAttack::FifthAttackInitialize(){
	context_.workAttack_.type_ = HitRecord::KnockbackType::Right;
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	context_.weaponParameter_.weaponTransform_.rotate = weaponAttackTransformRotates_[4];
	context_.weaponParameter_.weapon_Rotate_ = weapon_Rotates_[4];
}

void PlayerAttack::SixthAttackInitialize(){
	isRotated_ = false;
	context_.isJump_ = true;
	context_.workAttack_.isStrongHitStop_ = false;
	context_.workAttack_.type_ = HitRecord::KnockbackType::Center;
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	context_.weaponParameter_.weaponTransform_.rotate = weaponAttackTransformRotates_[5];
	context_.weaponParameter_.weapon_Rotate_ = weapon_Rotates_[5];
}

void PlayerAttack::Update(const Vector3& cameraRotate){
	ApplyGlobalVariables();
	context_.cameraRotate_ = cameraRotate;
	context_.frontVec_ = context_.postureVec_;
	//技が終わったか
	if (context_.workAttack_.isEndAttack_) {
		//次の攻撃を発動するか
		if (context_.workAttack_.comboNext_) {
			context_.workAttack_.comboIndex_++;
			//攻撃の方向を決定
			if (input_->GetPadLStick().x != 0 || input_->GetPadLStick().y != 0) {
				if (GameTime::timeScale_ != 0.0f) {
					context_.postureVec_ = { input_->GetPadLStick().x ,0,input_->GetPadLStick().y };
					Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(cameraRotate);
					context_.postureVec_ = Matrix::TransformNormal(context_.postureVec_, newRotateMatrix);
					context_.postureVec_.y = 0;
					context_.postureVec_ = Vector3::Normalize(context_.postureVec_);
					Matrix4x4 directionTodirection_;
					directionTodirection_.DirectionToDirection(Vector3::Normalize(context_.frontVec_), Vector3::Normalize(context_.postureVec_));
					context_.playerRotateMatrix_ = Matrix::Multiply(context_.playerRotateMatrix_, directionTodirection_);
				}
			}
			else if (lockOn_ && lockOn_->ExistTarget()) {
				Vector3 lockOnPos = lockOn_->GetTargetPosition();
				Vector3 sub = lockOnPos - context_.playerTransform_.translate;
				sub.y = 0;
				sub = Vector3::Normalize(sub);
				context_.postureVec_ = sub;

				Matrix4x4 directionTodirection_;
				directionTodirection_.DirectionToDirection(Vector3::Normalize(context_.frontVec_), Vector3::Normalize(context_.postureVec_));
				context_.playerRotateMatrix_ = Matrix::Multiply(context_.playerRotateMatrix_, directionTodirection_);

			}
			//各攻撃行動の初期化処理

			PreAttackInitialize();
			switch (context_.workAttack_.comboIndex_){
			case 1:
				AttackInitialize();
				break;
			case 2:
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				SecondAttackInitialize();
				break;
			case 3:
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				ThirdAttackInitialize();
				break;
			case 4:
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				FourthAttackInitialize();
				break;
			case 5:
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				FifthAttackInitialize();
				break;
			case 6:
				SixthAttackInitialize();
				break;
			default:
				break;
			}
			PostAttackInitialize();
		}
		else {
			//強攻撃を行うか
			if (context_.workAttack_.strongComboNext_) {
				context_.workAttack_.comboIndex_++;
				stateManager_->ChangeState(StateName::StrongAttack);
			}
			else {
				context_.workAttack_.attackParameter_ += GameTime::timeScale_;
				//通常状態に戻る処理
				if (context_.workAttack_.attackParameter_ >= ((float)(context_.workAttack_.nextAttackTimer_ + motionDistance_) / motionSpeed_)) {
					stateManager_->ChangeState(StateName::Root);
					context_.isDissolve_ = true;
					context_.workAttack_.trailResetFlug_ = true;
					context_.workAttack_.attackParameter_ = 0;
				}
				//スティックを倒しているときは少しだけ早く抜けるように
				if (input_->GetIsPushedLStick() and (context_.workAttack_.attackParameter_ * kAttackParameterCorection_ >= ((float)(context_.workAttack_.nextAttackTimer_) / motionSpeed_))) {
					stateManager_->ChangeState(StateName::Root);
					context_.isDissolve_ = true;
					context_.workAttack_.trailResetFlug_ = true;
					context_.workAttack_.attackParameter_ = 0;
				}
			}

		}

	}

	//強攻撃に派生する場合はここで帰る
	if (stateManager_->GetStateName() == StateName::StrongAttack){
		return;
	}
	//各行動の処理
	switch (context_.workAttack_.comboIndex_) {
	case 1:
		AttackMotion();
		break;
	case 2:
		SecondAttackMotion();
		break;
	case 3:
		ThirdAttackMotion();
		break;
	case 4:
		FourthAttackMotion();
		break;
	case 5:
		FifthAttackMotion();
		break;
	case 6:
		SixthAttackMotion();
		break;
	default:
		break;
	}
	//見た目を回転させる
	AppearanceRotate(context_.workAttack_.comboIndex_);
	//コンボ上限に達していない
	if (context_.workAttack_.comboIndex_ < context_.workAttack_.conboNum_) {
		if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_X)) {
			//コンボ有効
			context_.workAttack_.comboNext_ = true;
		}
		else if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_Y) ) {
			//強コンボ派生有効
			context_.workAttack_.strongComboNext_ = true;
		}
	}

	if (input_->GetPadButtonTriger(Input::GamePad::RB)) {
		context_.workAttack_.trailResetFlug_ = true;
		stateManager_->ChangeState(StateName::Dash);
	}

	//コリジョン用のtransform等を更新
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	weaponCollisionRotateMatrix = Matrix::Multiply(weaponCollisionRotateMatrix, context_.playerRotateMatrix_);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weaponCollisionTransform_.translate.y += (context_.weaponParameter_.addPosition_.y + 1.0f);
	context_.weaponParameter_.weaponTransform_.translate = context_.weaponParameter_.weaponCollisionTransform_.translate;
}

void PlayerAttack::CommonAttackMotion(){
	EndRequest();
	ShakeDownControl();
	context_.weaponParameter_.weaponTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
	context_.weaponParameter_.weaponCollisionTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
}

void PlayerAttack::AppearanceRotate(const int combo){
	if (combo == 3 or combo == 4) {
		context_.appearanceTransform_.rotate.y -= rotateYSpeed_ * GameTime::timeScale_;
	}
	else if (combo == 6) {
		context_.appearanceTransform_.rotate.x += rotateXSpeed_ * GameTime::timeScale_;
	}

	if (context_.appearanceTransform_.rotate.x >= maxRotateX_){
		context_.appearanceTransform_.rotate.x = maxRotateX_;
	}
	if (context_.appearanceTransform_.rotate.y <= maxRotateY_) {
		context_.appearanceTransform_.rotate.y = maxRotateY_;
	}
	
}

void PlayerAttack::AttackMove(){
	if (context_.workAttack_.comboIndex_ > 2 and context_.workAttack_.comboIndex_ != 6){
		motionSpeed_ *= 2.5f;
	}

	//攻撃時に移動
	context_.move_ = { 0.0f,0.0f,context_.moveSpeed_ * kAttackMagnification_ };
	context_.move_ = Matrix::TransformNormal(context_.move_, context_.playerRotateMatrix_);

	if (!context_.isCollisionEnemy_) {
		Vector3 NextPos = context_.playerTransform_.translate + context_.move_ * motionSpeed_;

		if (NextPos.x >= context_.limitPos_.x or NextPos.x <= context_.limitPos_.y) {
			context_.move_.x = 0;
		}
		if (NextPos.z >= context_.limitPos_.x or NextPos.z <= context_.limitPos_.y) {
			context_.move_.z = 0;
		}

		context_.playerTransform_.translate += (context_.move_ * motionSpeed_) * GameTime::timeScale_;
	}
	context_.weaponParameter_.weaponTransform_.translate = context_.playerTransform_.translate;
	context_.workAttack_.AttackTimer_ += GameTime::timeScale_;
}

void PlayerAttack::EndRequest(){
	//動作が終わったら待機
	if (waitTime_ <= 0) {
		context_.workAttack_.isEndAttack_ = true;
	}
}

void PlayerAttack::ShakeDownControl(){
	//フラグによって振る方向を分岐
	if (!context_.workAttack_.isShakeDown_) {
		context_.weaponParameter_.weapon_Rotate_ -= (context_.weaponParameter_.kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
	}
	else if (context_.workAttack_.isShakeDown_) {
		context_.weaponParameter_.weapon_Rotate_ += (context_.weaponParameter_.kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_) * GameTime::timeScale_;
	}
}

void PlayerAttack::AttackMotion(){
	//振っている現在の角度で処理を変更
	if (context_.weaponParameter_.weapon_Rotate_ >= weapon_RotatesMinMax_[0].x) {
		waitTime_ -= GameTime::timeScale_;
		context_.weaponParameter_.weapon_Rotate_ = weapon_RotatesMinMax_[0].x * kAttackMagnification_;
	}
	else if (context_.weaponParameter_.weapon_Rotate_ <= weapon_RotatesMinMax_[0].y) {
		context_.workAttack_.isShakeDown_ = true;
	}
	else {
		AttackMove();
	}
	CommonAttackMotion();
}

void PlayerAttack::SecondAttackMotion(){
	//振っている現在の角度で処理を変更
	if (context_.weaponParameter_.weapon_Rotate_ >= weapon_RotatesMinMax_[1].x) {
		waitTime_ -= GameTime::timeScale_;
		context_.weaponParameter_.weapon_Rotate_ = weapon_RotatesMinMax_[1].x;
	}
	else if (context_.weaponParameter_.weapon_Rotate_ <= weapon_RotatesMinMax_[1].y) {
		context_.workAttack_.isShakeDown_ = true;
	}
	else {
		AttackMove();
	}
	CommonAttackMotion();
}

void PlayerAttack::ThirdAttackMotion(){
	//振っている現在の角度で処理を変更
	if (context_.weaponParameter_.weapon_Rotate_ >= weapon_RotatesMinMax_[2].x) {
		waitTime_ -= GameTime::timeScale_;
		context_.weaponParameter_.weapon_Rotate_ = weapon_RotatesMinMax_[2].x;
	}
	else if (context_.weaponParameter_.weapon_Rotate_ <= weapon_RotatesMinMax_[2].y) {
		context_.workAttack_.isShakeDown_ = true;
	}
	else {
		AttackMove();
	}
	CommonAttackMotion();
}

void PlayerAttack::FourthAttackMotion(){
	//振っている現在の角度で処理を変更
	if (context_.weaponParameter_.weapon_Rotate_ >= weapon_RotatesMinMax_[3].x) {
		waitTime_ -= GameTime::timeScale_;
		context_.weaponParameter_.weapon_Rotate_ = weapon_RotatesMinMax_[3].x;
	}
	else {
		AttackMove();
	}
	CommonAttackMotion();
}

void PlayerAttack::FifthAttackMotion(){
	//振っている現在の角度で処理を変更
	if (context_.weaponParameter_.weapon_Rotate_ >= weapon_RotatesMinMax_[4].x) {
		waitTime_ -= GameTime::timeScale_;
		context_.weaponParameter_.weapon_Rotate_ = weapon_RotatesMinMax_[4].x;
	}
	else if (context_.weaponParameter_.weapon_Rotate_ <= weapon_RotatesMinMax_[4].y) {
		context_.workAttack_.isShakeDown_ = true;
	}
	else {
		AttackMove();
	}
	CommonAttackMotion();
}

void PlayerAttack::SixthAttackMotion(){
	//振っている現在の角度で処理を変更
	if (context_.weaponParameter_.weapon_Rotate_ >= weapon_RotatesMinMax_[5].x) {
		waitTime_ -= GameTime::timeScale_;
		context_.weaponParameter_.weapon_Rotate_ = weapon_RotatesMinMax_[5].x;
		SettingGroundCrushTex();
	}
	else if (context_.weaponParameter_.weapon_Rotate_ <= weapon_RotatesMinMax_[5].y) {
		if (context_.workAttack_.isShakeDown_ == false) {
			audio_->PlayAudio(attackMotionSE_, seVolume_, false);
		}		
		context_.workAttack_.isShakeDown_ = true;
	}
	else {
		if ((context_.weaponParameter_.weapon_Rotate_ >= kRotateWeaponGround_) and !isRotated_) {
			context_.workAttack_.type_ = HitRecord::KnockbackType::Strong;
			context_.workAttack_.isStrongHitStop_ = true;
			isRotated_ = true;
			context_.workAttack_.hitRecordRestFlug_ = true;
		}
		AttackMove();
	}
	CommonAttackMotion();
}

void PlayerAttack::SettingGroundCrushTex(){
	context_.isStopCrush_ = true;
	context_.groundCrushTexAlpha_ = 1.0f;
}
