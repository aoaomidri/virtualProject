#include "PlayerStrongAttack.h"
#include"LockOn.h"

bool PlayerStrongAttack::isLoad_ = false;

void PlayerStrongAttack::ApplyGlobalVariables(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "PlayerStrongAttackParameter";

	motionSpeed_ = adjustment_item->GetfloatValue(groupName, "MotionSpeed");
	motionDistance_ = adjustment_item->GetfloatValue(groupName, "MotionDistance");
	waitTimeBase_ = adjustment_item->GetfloatValue(groupName, "WaitTimeBase");
}

void PlayerStrongAttack::InitGlobalVariables() const{
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "PlayerStrongAttackParameter";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "MotionSpeed", motionSpeed_);
	adjustment_item->AddItem(groupName, "MotionDistance", motionDistance_);
	adjustment_item->AddItem(groupName, "WaitTimeBase", waitTimeBase_);
}

void PlayerStrongAttack::Initialize(){
	BasePlayerState::Initialize();
	if (!isLoad_) {
		InitGlobalVariables();
		isLoad_ = true;
	}
	context_.isSuperArmor_ = true;
	PreStrongAttackInitialize();
	AllStrongAttackInitialize();
	PostStrongAttackInitialize();	
}

void PlayerStrongAttack::AllStrongAttackInitialize(){
	chargeEnd_ = false;

	context_.weaponParameter_.weapon_offset_Base_ = { 0.0f,0.0f,0.0f };
	//攻撃方向の指定
	if (input_->GetPadLStick().x != 0 || input_->GetPadLStick().y != 0) {
		if (GameTime::timeScale_ != 0.0f) {
			context_.postureVec_ = { input_->GetPadLStick().x ,0,input_->GetPadLStick().y };
			Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(context_.cameraRotate_);
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
	//各強攻撃の初期化
	PreStrongAttackInitialize();
	switch (context_.workAttack_.comboIndex_) {
	case 1:
		StrongAttackInitialize();
		break;
	case 2:
		SecondStrongAttackInitialize();
		break;
	case 3:
		ThirdStrongAttackInitialize();
		break;
	case 4:
		FourthStrongAttackInitialize();
		break;
	case 5:
		FifthStrongAttackInitialize();
		break;
	case 6:
		SixthStrongAttackInitialize();
		break;
	default:
		break;
	}
	PostStrongAttackInitialize();
}

void PlayerStrongAttack::PreStrongAttackInitialize(){
	context_.workAttack_.comboNext_ = false;
	context_.workAttack_.strongComboNext_ = false;
	context_.workAttack_.trailResetFlug_ = true;
	context_.workAttack_.hitRecordRestFlug_ = true;

	context_.workAttack_.attackParameter_ = 0;
	context_.weaponParameter_.weaponTransform_.translate = context_.playerTransform_.translate;
	context_.workAttack_.AttackTimer_ = 0;
	waitTime_ = waitTimeBase_;
	context_.workAttack_.isEndAttack_ = false;
}

void PlayerStrongAttack::PostStrongAttackInitialize(){

}

void PlayerStrongAttack::StrongAttackInitialize(){
	context_.workAttack_.type_ = HitRecord::KnockbackType::Center;
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	context_.weaponParameter_.weaponTransform_.rotate = weaponStrongAttackTransformRotates_[0];
	context_.weaponParameter_.weapon_offset_Base_ = weaponStrongAttackOffset_[0];
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.rotate = { 0.0f,0.00f,context_.weaponParameter_.weaponTransform_.rotate.z };
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weapon_Rotate_ = 1.0f;
	context_.isTrail_ = false;
	context_.workAttack_.isShakeDown_ = true;
	chargeEnd_ = false;
}

void PlayerStrongAttack::SecondStrongAttackInitialize(){
	context_.workAttack_.type_ = HitRecord::KnockbackType::Floating;
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	context_.weaponParameter_.weaponTransform_.rotate = weaponStrongAttackTransformRotates_[1];
	context_.weaponParameter_.weapon_offset_Base_ = weaponStrongAttackOffset_[1];
	isNextAttack_ = false;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.rotate = weaponStrongAttackTransformRotates_[1];
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weapon_Rotate_ = secondWeapon_Rotate_;
	context_.isTrail_ = false;
	context_.workAttack_.isShakeDown_ = false;
}

void PlayerStrongAttack::SSAttackWeakVersionInitialize(){
	context_.workAttack_.hitRecordRestFlug_ = true;
	context_.workAttack_.type_ = HitRecord::KnockbackType::Right;
	context_.weaponParameter_.weaponTransform_.rotate = weaponStrongAttackTransformRotates_[2];
	context_.weaponParameter_.weapon_offset_Base_ = weaponStrongAttackOffset_[2];
	waitTime_ = waitTimeBase_;
	context_.workAttack_.isEndAttack_ = false;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.rotate = { 0.0f,0.00f,context_.weaponParameter_.weaponTransform_.rotate.z };
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weapon_Rotate_ = -0.0f;
}

void PlayerStrongAttack::SSAttackStrongVersionInitialize(){
	context_.workAttack_.hitRecordRestFlug_ = true;
	context_.workAttack_.type_ = HitRecord::KnockbackType::Strong;
	context_.weaponParameter_.weaponTransform_.rotate = strongSecondRotate_;
	context_.weaponParameter_.weapon_offset_Base_ = -weaponStrongAttackOffset_[0];
	waitTime_ = waitTimeBase_;
	context_.isJump_ = true;
	context_.workAttack_.isEndAttack_ = false;
	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.rotate = strongSecondRotate_;
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weapon_Rotate_ = -0.0f;
}

void PlayerStrongAttack::ThirdStrongAttackInitialize(){
	context_.workAttack_.type_ = HitRecord::KnockbackType::Strong;
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	context_.weaponParameter_.weaponTransform_.rotate = weaponStrongAttackTransformRotates_[2];
	context_.weaponParameter_.weapon_offset_Base_ = weaponStrongAttackOffset_[2];

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.rotate = { 0.0f,0.00f,context_.weaponParameter_.weaponTransform_.rotate.z };
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weapon_Rotate_ = -0.0f;
	context_.isTrail_ = false;
	context_.workAttack_.isShakeDown_ = false;
}

void PlayerStrongAttack::FourthStrongAttackInitialize(){
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	context_.weaponParameter_.weaponTransform_.rotate = weaponStrongAttackTransformRotates_[3];
	context_.weaponParameter_.weapon_offset_Base_.y = weaponStrongAttackOffset_[3].y;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.rotate = { 0.0f,0.00f,context_.weaponParameter_.weaponTransform_.rotate.z };
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_ * strongAddScale_;
	context_.weaponParameter_.weapon_Rotate_ = -0.0f;
	context_.isTrail_ = true;
	context_.workAttack_.isShakeDown_ = false;
}

void PlayerStrongAttack::FifthStrongAttackInitialize(){
	context_.workAttack_.type_ = HitRecord::KnockbackType::Few;
	///
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	context_.weaponParameter_.weaponTransform_.rotate = weaponStrongAttackTransformRotates_[4];
	context_.weaponParameter_.weapon_offset_Base_ = weaponStrongAttackOffset_[4];

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.rotate = { 0.0f,0.00f,context_.weaponParameter_.weaponTransform_.rotate.z };
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_ * strongAddScale_;

	context_.weaponParameter_.weapon_Rotate_ = fifthWeapon_Rotate_;
	strongSecondAttackCount_ = 0;
	isFirstAttack_ = true;
	isNextAttack_ = false;
	isFinishAttack_ = false;
	context_.isTrail_ = false;
	context_.workAttack_.isShakeDown_ = false;
}

void PlayerStrongAttack::SixthStrongAttackInitialize(){
	context_.workAttack_.nextAttackTimer_ = nextAttackTimer_;
	//baseRotate_.x = Matrix::RotateAngleYFromMatrix(playerRotateMatrix_);
	context_.weaponParameter_.weaponTransform_.rotate = weaponStrongAttackTransformRotates_[5];
	context_.weaponParameter_.weapon_offset_Base_ = weaponStrongAttackOffset_[5];
	context_.weaponParameter_.addPosition_.y = 0.0f;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.rotate = { 0.0f,0.00f,context_.weaponParameter_.weaponTransform_.rotate.z };
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_ * strongAddScale_;

	easeT_ = 0;
	addEaseT_ = addEaseSpeed_;
	context_.weaponParameter_.weapon_Rotate_ = 1.0f;
	context_.isTrail_ = false;
	context_.workAttack_.isShakeDown_ = true;
	chargeEnd_ = false;
}

void PlayerStrongAttack::Update(const Vector3& cameraRotate){
	ApplyGlobalVariables();
	DrawImgui();
	context_.cameraRotate_ = cameraRotate;
	
	//攻撃が終わったら通常状態へ
	if (context_.workAttack_.isEndAttack_) {
		context_.workAttack_.attackParameter_ += GameTime::timeScale_;
		if (context_.workAttack_.attackParameter_ >= ((float)(context_.workAttack_.nextAttackTimer_ + motionDistance_) / motionSpeed_)) {
			context_.isDissolve_ = true;
			PlayerStateManager::GetInstance()->ChangeState(StateName::Root);
			context_.workAttack_.attackParameter_ = 0;
		}
	}

	//各強攻撃の更新
	switch (context_.workAttack_.comboIndex_) {
	case 1:
		StrongAttackMotion();
		break;
	case 2:
		if (isNextAttack_){
			if (isStrongVersion_){
				SSAttackStrongVersion();
			}
			else {
				SSAttackWeakVersion();
			}			
		}
		else {
			SecondStrongAttackMotion();
		}		
		break;
	case 3:
		ThirdStrongAttackMotion();
		break;
	case 4:
		FourthStrongAttackMotion();
		break;
	case 5:
		FifthStrongAttackMotion();
		break;
	case 6:
		SixthStrongAttackMotion();
		break;
	default:
		break;
	}
	//SRT等情報を更新
	context_.weaponParameter_.weaponTransform_.translate = context_.playerTransform_.translate;
	context_.weaponParameter_.weaponTransform_.translate.y += context_.weaponParameter_.addPosition_.y;

	Matrix4x4 weaponCollisionRotateMatrix = Matrix::MakeRotateMatrix(context_.weaponParameter_.weaponTransform_.rotate);
	weaponCollisionRotateMatrix = Matrix::Multiply(weaponCollisionRotateMatrix, context_.playerRotateMatrix_);
	context_.weaponParameter_.weapon_offset_ = Matrix::TransformNormal(context_.weaponParameter_.weapon_offset_Base_, weaponCollisionRotateMatrix);
	context_.weaponParameter_.weaponCollisionTransform_.translate = context_.playerTransform_.translate + context_.weaponParameter_.weapon_offset_;
	context_.weaponParameter_.weaponCollisionTransform_.translate.y += context_.weaponParameter_.addPosition_.y + 1.0f;
	
	//ダッシュが入力されたら即中断
	if (input_->GetPadButtonTriger(Input::GamePad::RB)) {
		PlayerStateManager::GetInstance()->ChangeState(StateName::Dash);
	}
}

void PlayerStrongAttack::DrawImgui(){
#ifdef _DEBUG
	ImGui::Begin("強攻撃時のステータス");
	ImGui::DragFloat("待ち時間", &waitTime_);
	ImGui::End();
#endif // _DEBUG	
}

void PlayerStrongAttack::StrongAttackMotion(){
	if (!chargeEnd_) {
		//長押ししている間は構え
		if (input_->GetPadButton(XINPUT_GAMEPAD_Y)) {
			context_.isGuard_ = true;
			context_.weaponParameter_.weaponCollisionTransform_.scale = collsionScaleGuade_;
		}
		//ボタンを離すかガード中に攻撃がヒットした場合派生
		if (input_->GetPadButtonRelease(XINPUT_GAMEPAD_Y) or context_.isGuardHit_) {
			context_.workAttack_.type_ = HitRecord::Strong;
			context_.isGuard_ = false;
			chargeEnd_ = true;
			context_.weaponParameter_.weapon_offset_Base_ = weaponStrongAttackOffset_[2];
			context_.weaponParameter_.weaponTransform_.rotate.z = strongAttackRotateZ_;
			if (context_.isGuardHit_) {
				counterScale_ = context_.weaponParameter_.kWeaponCollisionBase_ * kGuadeMagnification_;
			}
			else {
				counterScale_ = context_.weaponParameter_.kWeaponCollisionBase_;
			}
		}
	}
	else {
		//攻撃動作
		//限界まで振ったら終わり
		if (context_.weaponParameter_.weapon_Rotate_ >= weapon_StrongRotatesMinMax_[0].y) {
			waitTime_ -= 1;
			context_.weaponParameter_.weapon_Rotate_ = weapon_StrongRotatesMinMax_[0].y;
		}
		else {
			//移動
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
				context_.playerTransform_.translate += context_.move_ * motionSpeed_ * GameTime::timeScale_;
			}
			context_.weaponParameter_.weaponTransform_.translate = context_.playerTransform_.translate;
		}

		if (waitTime_ <= 0) {
			context_.workAttack_.isEndAttack_ = true;
		}

		if (context_.workAttack_.isShakeDown_) {
			//角度によってトレイルや当たり判定が働くように
			if (context_.weaponParameter_.weapon_Rotate_ < weapon_StrongRotatesMinMax_[0].x) {
				context_.isTrail_ = false;
				context_.weaponParameter_.weaponCollisionTransform_.scale = { 0.0f,0.0f,0.0f };
				context_.weaponParameter_.weapon_Rotate_ += context_.weaponParameter_.kMoveWeaponShakeDown_ / kAttackDivisionMagnification_ * motionSpeed_ * GameTime::timeScale_;
			}
			else {
				if (!context_.isTrail_ and context_.workAttack_.comboIndex_ != 2) {
					audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				};
				context_.weaponParameter_.weaponCollisionTransform_.scale = counterScale_;
				context_.isTrail_ = true;
				context_.weaponParameter_.weapon_Rotate_ += context_.weaponParameter_.kMoveWeaponShakeDown_ * kStrongAttackMagnification_ * motionSpeed_ * GameTime::timeScale_;
			}
		}
		context_.weaponParameter_.weaponTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
		context_.weaponParameter_.weaponCollisionTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
	}
}

void PlayerStrongAttack::SecondStrongAttackMotion(){
	//シンプルな縦振り
	//限度によって行う処理を変更
	if (context_.weaponParameter_.weapon_Rotate_ >= weapon_StrongRotatesMinMax_[1].y) {
		//振り切った後
		if (input_->GetPadButtonTriger(Input::GamePad::X) and !isNextAttack_) {
			SSAttackWeakVersionInitialize();
			isStrongVersion_ = false;
			isNextAttack_ = true;
			return;
		}
		else if (input_->GetPadButtonTriger(Input::GamePad::Y) and !isNextAttack_) {
			SSAttackStrongVersionInitialize();
			isStrongVersion_ = true;
			isNextAttack_ = true;
			return;
		}
		context_.weaponParameter_.weapon_Rotate_ = weapon_StrongRotatesMinMax_[1].y;		
		waitTime_ -= 1;
	}
	else if ((context_.weaponParameter_.weapon_Rotate_ <= weapon_StrongRotatesMinMax_[1].x)) {
		//振りかぶった後
		context_.isAttackJump_ = true;
		audio_->PlayAudio(attackMotionSE_, seVolume_, false);
		context_.workAttack_.isShakeDown_ = true;
		context_.isTrail_ = true;
	}

	if (waitTime_ <= 0) {
		context_.workAttack_.isEndAttack_ = true;
	}
	//振り下ろしているかどうかで処理を変更	
	if (!context_.workAttack_.isShakeDown_) {
		context_.weaponParameter_.weaponCollisionTransform_.scale = Vector3();
		context_.weaponParameter_.weapon_Rotate_ -= (context_.weaponParameter_.kMoveWeapon_ * motionSpeed_ / kGuadeMagnification_);
	}
	else {
		context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_;
		context_.weaponParameter_.weapon_Rotate_ += context_.weaponParameter_.kMoveWeaponShakeDown_ * kAttackDivisionMagnification_ * motionSpeed_;
	}	
	context_.weaponParameter_.weaponTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
	context_.weaponParameter_.weaponCollisionTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
}

void PlayerStrongAttack::SSAttackWeakVersion(){
	if ((context_.weaponParameter_.weapon_Rotate_ >= strongSecondRotatesMinMax_.y) or !context_.isAttackJump_) {
		//振り切った後		
		context_.weaponParameter_.weapon_Rotate_ = strongSecondRotatesMinMax_.y;
		context_.appearanceTransform_.rotate.x = maxRotateX_;
		waitTime_ -= 1;
		SettingGroundCrushTex();
	}
	else if ((context_.weaponParameter_.weapon_Rotate_ <= strongSecondRotatesMinMax_.x)) {
		//振りかぶった後
		context_.isAttackJump_ = true;
		audio_->PlayAudio(attackMotionSE_, seVolume_, false);
		context_.workAttack_.isShakeDown_ = true;
		context_.isTrail_ = true;
	}

	if (waitTime_ <= 0) {
		context_.workAttack_.isEndAttack_ = true;
	}
	//振り下ろしているかどうかで処理を変更	
	if (!context_.workAttack_.isShakeDown_) {
		context_.weaponParameter_.weapon_Rotate_ -= (context_.weaponParameter_.kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
	}
	else if (context_.workAttack_.isShakeDown_) {
		context_.weaponParameter_.weapon_Rotate_ += (context_.weaponParameter_.kMoveWeaponShakeDown_ * secondRotateSpeed_ * motionSpeed_) * GameTime::timeScale_;
	}
	context_.weaponParameter_.weaponTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
	context_.weaponParameter_.weaponCollisionTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;

	AppearanceRotate();	
}

void PlayerStrongAttack::SSAttackStrongVersion(){
	if (context_.isOnFloor_) {
		//振り切った後
		context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_ * strongSecondAddScale_;
		waitTime_ -= 1;
		SettingGroundCrushTex();
	}
	if (context_.isJump_ == true){
		context_.isJump_ = false;
	}

	if (waitTime_ <= 0) {
		context_.workAttack_.isEndAttack_ = true;
	}
}

void PlayerStrongAttack::ThirdStrongAttackMotion(){
	//シンプルな縦振り
	//限度によって行う処理を変更
	if (context_.weaponParameter_.weapon_Rotate_ >= weapon_StrongRotatesMinMax_[2].y) {
		//振り切った後
		waitTime_ -= 1;
		context_.weaponParameter_.weapon_Rotate_ = 1.35f;
		SettingGroundCrushTex();
	}
	else if (context_.weaponParameter_.weapon_Rotate_ <= weapon_StrongRotatesMinMax_[2].x) {
		//振りかぶった後
		audio_->PlayAudio(attackMotionSE_, seVolume_, false);
		context_.workAttack_.isShakeDown_ = true;
		context_.isTrail_ = true;
	}

	if (waitTime_ <= 0) {
		context_.workAttack_.isEndAttack_ = true;
	}
	//振り下ろしているかどうかで処理を変更
	if (!context_.workAttack_.isShakeDown_) {
		context_.weaponParameter_.weaponCollisionTransform_.scale = Vector3();
		context_.weaponParameter_.weapon_Rotate_ -= (context_.weaponParameter_.kMoveWeapon_ * motionSpeed_ / kGuadeMagnification_);
	}
	else {
		context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_ * strongAddScale_;
		context_.weaponParameter_.weapon_Rotate_ += context_.weaponParameter_.kMoveWeaponShakeDown_ * kAttackDivisionMagnification_ * motionSpeed_;
	}
	context_.weaponParameter_.weaponTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
	context_.weaponParameter_.weaponCollisionTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
}

void PlayerStrongAttack::FourthStrongAttackMotion(){
	//斬りはらって後退
	//限度によって行う処理を変更
	if (context_.weaponParameter_.weapon_Rotate_ >= weapon_StrongRotatesMinMax_[3].y) {
		//振り切った後
		waitTime_ -= 1;
		context_.weaponParameter_.weapon_Rotate_ = weapon_StrongRotatesMinMax_[3].y;
	}
	else if (context_.weaponParameter_.weapon_Rotate_ <= weapon_StrongRotatesMinMax_[3].x) {
		//振りかぶったあと
		audio_->PlayAudio(attackMotionSE_, seVolume_, false);
		context_.workAttack_.isShakeDown_ = true;
	}

	if (waitTime_ <= 0) {
		context_.workAttack_.isEndAttack_ = true;
	}
	//振り下ろしているかどうかで処理を変更
	if (!context_.workAttack_.isShakeDown_) {
		context_.weaponParameter_.weaponCollisionTransform_.scale = Vector3();
		context_.weaponParameter_.weapon_Rotate_ -= (context_.weaponParameter_.kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
	}
	else {
		context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_ * strongAddScale_;
		context_.weaponParameter_.weapon_Rotate_ += context_.weaponParameter_.kMoveWeaponShakeDown_ * kStrongAttackMagnification_ * motionSpeed_;
		if (!context_.workAttack_.isEndAttack_) {
			//後退処理
			context_.move_ = { 0.0f,0.0f,-(context_.moveSpeed_ * kStrongAttackMagnification_) };
			context_.move_ = Matrix::TransformNormal(context_.move_, context_.playerRotateMatrix_);
			if (!context_.isCollisionEnemy_) {
				Vector3 NextPos = context_.playerTransform_.translate + context_.move_ * motionSpeed_;
				if (NextPos.x >= context_.limitPos_.x or NextPos.x <= context_.limitPos_.y) {
					context_.move_.x = 0;
				}
				if (NextPos.z >= context_.limitPos_.x or NextPos.z <= context_.limitPos_.y) {
					context_.move_.z = 0;
				}
				context_.playerTransform_.translate += context_.move_ * motionSpeed_;
			}
			context_.weaponParameter_.weaponTransform_.translate = context_.playerTransform_.translate;
			context_.workAttack_.AttackTimer_ += GameTime::timeScale_;
		}
	}
	context_.weaponParameter_.weaponTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
	context_.weaponParameter_.weaponCollisionTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
}

void PlayerStrongAttack::FifthStrongAttackMotion(){
	//ボタン連打で連続切り、後にフィニッシュ攻撃
	if (!isFinishAttack_) {
		//連続切り
		if (isFirstAttack_) {
			//一段目
			if (context_.weaponParameter_.weapon_Rotate_ >= weapon_StrongRotatesMinMax_[4].y) {
				waitTime_ -= 1;
				context_.weaponParameter_.weapon_Rotate_ = weapon_StrongRotatesMinMax_[4].y;
			}
			else if (context_.weaponParameter_.weapon_Rotate_ <= weapon_StrongRotatesMinMax_[4].x) {
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				context_.workAttack_.isShakeDown_ = true;
				context_.isTrail_ = true;
			}
			//攻撃が終わったら二段目に派生
			if (waitTime_ <= 0) {
				isFirstAttack_ = false;
				waitTime_ = waitTimeBase_;
				context_.weaponParameter_.weaponTransform_.rotate.z = weapon_StrongRotatesMinMax_[4].x;
				context_.workAttack_.hitRecordRestFlug_ = true;
			}
			if (!context_.workAttack_.isShakeDown_) {
				context_.weaponParameter_.weapon_Rotate_ -= (context_.weaponParameter_.kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
			}
			else{
				context_.weaponParameter_.weapon_Rotate_ += context_.weaponParameter_.kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_;
			}
		}
		else {
			//二段目
			if (context_.weaponParameter_.weapon_Rotate_ <= weapon_StrongRotatesMinMax_[5].y) {
				waitTime_ -= 1;
				context_.weaponParameter_.weapon_Rotate_ = weapon_StrongRotatesMinMax_[5].y;
			}
			else if (context_.weaponParameter_.weapon_Rotate_ >= weapon_StrongRotatesMinMax_[5].x) {
				audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				context_.workAttack_.isShakeDown_ = true;
			}
			if (!isNextAttack_) {
				//ボタンを押したら連続攻撃を継続
				if (input_->GetPadButtonTriger(Input::GamePad::Y)) {
					isNextAttack_ = true;
				}
			}
			if (waitTime_ <= 0) {
				//攻撃が終わったときにフラグによってとる行動を分岐
				if (isNextAttack_) {
					//一段目に戻り連続攻撃を継続
					isNextAttack_ = false;
					isFirstAttack_ = true;
					waitTime_ = waitTimeBase_;
					context_.weaponParameter_.weaponTransform_.rotate.z = -kAttackMagnification_;
					context_.workAttack_.hitRecordRestFlug_ = true;
					audio_->PlayAudio(attackMotionSE_, seVolume_, false);
				}
				else {
					//連続攻撃をやめ、フィニッシュ攻撃を行う
					waitTime_ = waitTimeBase_;
					context_.weaponParameter_.weaponTransform_.rotate.z = -weapon_StrongRotatesMinMax_[4].x;
					context_.weaponParameter_.weapon_Rotate_ = -0.0f;
					context_.workAttack_.isShakeDown_ = false;
					context_.isTrail_ = false;
					isFinishAttack_ = true;
					context_.workAttack_.type_ = HitRecord::Strong;
					context_.workAttack_.hitRecordRestFlug_ = true;
				}
			}
			else if (!context_.workAttack_.isShakeDown_) {
				context_.weaponParameter_.weapon_Rotate_ += (context_.weaponParameter_.kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
			}
			else{
				context_.weaponParameter_.weapon_Rotate_ -= context_.weaponParameter_.kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_;
			}
		}
	}
	else {
		//フィニッシュ攻撃
		if (context_.weaponParameter_.weapon_Rotate_ >= weapon_StrongRotatesMinMax_[4].y) {
			//振り切った後
			waitTime_ -= 1;
			context_.weaponParameter_.weapon_Rotate_ = weapon_StrongRotatesMinMax_[4].y;
			SettingGroundCrushTex();
		}
		else if (context_.weaponParameter_.weapon_Rotate_ <= weapon_StrongRotatesMinMax_[4].x) {
			//振りかぶった後
			audio_->PlayAudio(attackMotionSE_, seVolume_, false);
			context_.isTrail_ = true;
			context_.workAttack_.isShakeDown_ = true;
		}
		//一連行動終わり
		if (waitTime_ <= 0) {
			context_.workAttack_.isEndAttack_ = true;
		}

		if (!context_.workAttack_.isShakeDown_) {
			context_.weaponParameter_.weapon_Rotate_ -= (context_.weaponParameter_.kMoveWeapon_ * motionSpeed_ / kAttackDivisionMagnification_);
		}
		else{
			context_.weaponParameter_.weaponCollisionTransform_.scale = fifthWeaponCollisionScale_ * strongAddScale_;
			context_.weaponParameter_.weapon_Rotate_ += context_.weaponParameter_.kMoveWeaponShakeDown_ * kAttackMagnification_ * motionSpeed_;
		}
	}
	context_.weaponParameter_.weaponTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
	context_.weaponParameter_.weaponCollisionTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
}

void PlayerStrongAttack::SixthStrongAttackMotion(){
	//ガード反撃
	//構え中
	if (!chargeEnd_) {
		context_.weaponParameter_.weaponCollisionTransform_.scale = Vector3();
		//ガード
		if (input_->GetPadButton(XINPUT_GAMEPAD_Y)) {
			context_.isGuard_ = true;
			context_.weaponParameter_.weaponTransform_.rotate.x += 0.01f;
			if (context_.weaponParameter_.weaponTransform_.rotate.x > strongSixthAttackRotate_) {
				context_.weaponParameter_.weaponTransform_.rotate.x = strongSixthAttackRotate_;
			}
		}
		//ボタンを離したら派生
		if (input_->GetPadButtonRelease(XINPUT_GAMEPAD_Y)) {
			context_.isGuard_ = false;
			chargeEnd_ = true;
			context_.isTrail_ = true;
			context_.weaponParameter_.weapon_offset_Base_ = { 0.0f,context_.weaponParameter_.kWeapon_offset_,0.0f };
			context_.weaponParameter_.weaponTransform_.rotate.z = strongAttackRotateZ_;
		}
	}
	else {
		//攻撃振り
		context_.weaponParameter_.weaponCollisionTransform_.scale = context_.weaponParameter_.kWeaponCollisionBase_ * strongAddScale_;
		easeT_ += addEaseT_;
		if (easeT_ > 1.0f) {
			//振り切った
			easeT_ = 1.0f;
			addEaseT_ = 0.0f;
			audio_->PlayAudio(attackMotionSE_, seVolume_, false);
			SettingGroundCrushTex();
		}
		//最後まで行ったら待機
		if (easeT_ == 1.0f) {
			waitTime_ -= 1;
		}
		//待機時間が終わったら終了
		if (waitTime_ <= 0) {
			context_.workAttack_.isEndAttack_ = true;
		}
		context_.weaponParameter_.weapon_Rotate_ = Ease::Easing(Ease::EaseName::EaseInCubic, 2.3f, -5.5f, easeT_);
		context_.weaponParameter_.weaponTransform_.rotate.z = Ease::Easing(Ease::EaseName::EaseInCubic, 1.57f, 2.8f, easeT_);
		context_.weaponParameter_.weaponTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
		context_.weaponParameter_.weaponCollisionTransform_.rotate.x = context_.weaponParameter_.weapon_Rotate_;
	}
}

void PlayerStrongAttack::AppearanceRotate(){
	//見た目の処理
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "PlayerAttackParameter";
	context_.appearanceTransform_.rotate.x += adjustment_item->GetfloatValue(groupName, "RotateXSpeed") * GameTime::timeScale_;
	if (context_.appearanceTransform_.rotate.x >= maxRotateX_) {
		context_.appearanceTransform_.rotate.x = maxRotateX_;
	}
}

void PlayerStrongAttack::SettingGroundCrushTex(){
	context_.isStopCrush_ = true;
	context_.groundCrushTexAlpha_ = 1.0f;
}
