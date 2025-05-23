#include "FollowCamera.h"
#include"GameTime.h"
#include"PlayerStateManager.h"
#include"LockOn.h"
void FollowCamera::ApplyGlobalVariables(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Camera";

	angle_t = adjustment_item->GetfloatValue(groupName, "AngleComplement");
	attackAngle_t = adjustment_item->GetfloatValue(groupName, "AttackAngleComplement");
	t_ = adjustment_item->GetfloatValue(groupName, "PositionComplement");
	distance_ = adjustment_item->GetfloatValue(groupName, "distance");

	if (angle_t > 1.0f) {
		angle_t = 1.0f;
	}
	if (t_ > 1.0f) {
		t_ = 1.0f;
	}
}

void FollowCamera::ShakeUpdate(){
	const float minusOffset = 0.5f;

	cameraShake_.elapsedTime += GameTime::deltaTime_;

	float progress = (cameraShake_.elapsedTime) / cameraShake_.duration;

	cameraShake_.amplitude *= std::exp(-progress * shakePower_);

	// 乱数に基づくオフセット
	float offsetX = (static_cast<float>(rand()) / RAND_MAX - minusOffset) * cameraShake_.amplitude;
	float offsetY = (static_cast<float>(rand()) / RAND_MAX - minusOffset) * cameraShake_.amplitude;
	float offsetZ = 0.0f;  // Z軸方向は揺れないようにする

	rootOffset_.x += offsetX;
	rootOffset_.y += offsetY;
	rootOffset_.z += offsetZ;

	// シェイクが終了したら振幅をリセット
	if (cameraShake_.elapsedTime >= cameraShake_.duration) {
		cameraShake_.amplitude = 0.0f;
	}
}


void FollowCamera::Initialize(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Camera";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "AngleComplement", angle_t);
	adjustment_item->AddItem(groupName, "AttackAngleComplement", attackAngle_t);
	adjustment_item->AddItem(groupName, "PositionComplement", t_);
	adjustment_item->AddItem(groupName, "distance", distance_);

	input_ = Input::GetInstance();

	destinationAngleX_ = 0.0f;

	rootOffset_= { 0.0f, height_, distance_ };

	minRotate_ = 0.1f;
	maxRotate_ = 0.9f;

	baseOffset_ = rootOffset_;

	destinationAngleX_ = 0.2f;

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

}

void FollowCamera::Update(){
	frontVec_ = postureVec_;

	PlayerStateManager::StateName nowState;
	
	nowState = PlayerStateManager::GetInstance()->GetStateName();

	ApplyGlobalVariables();

	//ロックオンしている場合はターゲットに向くように
	if (lockOn_ && lockOn_->target_) {
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPos - target_->translate;
		sub.y = 0;
		postureVec_ = sub;

		Matrix4x4 newMatrix;
		newMatrix.DirectionToDirection(Vector3::Normalize(Vec_), Vector3::Normalize(postureVec_));
		destinationAngleY_ = Matrix::RotateAngleYFromMatrix(newMatrix);
		destinationAngleX_ = lockOnAngle_;
	}
	else {
		//スティック操作でカメラを動かす
		if (input_->GetConnectPad() && isMove_) {
			cameraMove_ = { -input_->GetPadRStick().y * moveMagnification_,input_->GetPadRStick().x * moveMagnification_,0.0f };
			Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(viewProjection_.rotation_);
			postureVec_ = Matrix::TransformNormal(Vec_, newRotateMatrix);
			postureVec_.y = 0.0f;
			postureVec_ = Vector3::Normalize(postureVec_);
			if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_RIGHT_THUMB)) {
				destinationAngleY_ = Matrix::RotateAngleYFromMatrix(*targetRotateMatrix_);
				destinationAngleX_ = resetAngle_;
			}
		}
	}
	if (targetRotateMatrix_) {
		attackAngleY_ = Matrix::RotateAngleYFromMatrix(*targetRotateMatrix_);
		attackAngleX_ = resetAngle_;
	}
	//回転を加算
	destinationAngleX_ += cameraMove_.x;
	destinationAngleY_ += cameraMove_.y;
	//カメラの限度
	if (destinationAngleX_ <= minRotate_) {
		destinationAngleX_ = minRotate_;
	}
	else if (destinationAngleX_ >= maxRotate_) {
		destinationAngleX_ = maxRotate_;
	}
	if (nowState == PlayerStateManager::StateName::Attack or nowState == PlayerStateManager::StateName::StrongAttack){
		//回転角の補完
		viewProjection_.rotation_.y =
			Vector3::LerpShortAngle(viewProjection_.rotation_.y, attackAngleY_, attackAngle_t);
		viewProjection_.rotation_.x =
			Vector3::LerpShortAngle(viewProjection_.rotation_.x, attackAngleX_, attackAngle_t);

		destinationAngleX_ = viewProjection_.rotation_.x;
		destinationAngleY_ = viewProjection_.rotation_.y;
	}
	else {
		//回転角の補完
		viewProjection_.rotation_.y =
			Vector3::LerpShortAngle(viewProjection_.rotation_.y, destinationAngleY_, angle_t);
		viewProjection_.rotation_.x =
			Vector3::LerpShortAngle(viewProjection_.rotation_.x, destinationAngleX_, angle_t);
	}
	
	rootOffset_ = { 0.0f, height_, distance_ };
	//カメラシェイク
	ShakeUpdate();

	baseOffset_ = rootOffset_;
	//カメラが外に行ってしまわないように簡単な補正
	if (target_) {
		//追従対象からカメラまでのオフセット
		Vector3 offset = offsetCalculation(baseOffset_);
		//追従座標の補完
		interTarget_ = Vector3::Lerp(interTarget_, target_->translate, t_);
		Vector3 result = interTarget_ + offset;
		if (result.x >= limitPos_.x or result.x <= limitPos_.y){
			if (result.x > 0){
				result.x = limitPos_.x;
			}
			else {
				result.x = limitPos_.y;
			}
		}
		if (result.z >= limitPos_.x or result.z <= limitPos_.y) {
			if (result.z > 0) {
				result.z = limitPos_.x;
			}
			else {
				result.z = limitPos_.y;
			}
		}
		//座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = result;
	}
	//視錐台に値を代入
	viewingFrustum_.translation_ = viewProjection_.translation_;
	viewingFrustum_.rotate_ = viewProjection_.rotation_;
	
	lockViewingFrustum_.translation_ = viewingFrustum_.translation_;
	lockViewingFrustum_.rotate_ = viewingFrustum_.rotate_;

	viewProjection_.UpdateMatrix();
}

void FollowCamera::Reset(){
	if (target_) {
		//追従座標・角度の初期化
		interTarget_ = target_->translate;
		viewProjection_.rotation_.y = target_->rotate.y;
	}
	Vector3 offset = offsetCalculation(baseOffset_);
	viewProjection_.translation_ = interTarget_ + offset;
}

void FollowCamera::RotateReset(){
	if (targetRotateMatrix_) {
		destinationAngleY_ = Matrix::RotateAngleYFromMatrix(*targetRotateMatrix_);
		destinationAngleX_ = 0.2f;
	}
}

void FollowCamera::CameraPosInit(){
	if (targetRotateMatrix_) {
		destinationAngleY_ = Matrix::RotateAngleYFromMatrix(*targetRotateMatrix_);
		destinationAngleX_ = 0.2f;
		cameraMove_ = { 0,0,0 };
	}
}

Vector3 FollowCamera::offsetCalculation(const Vector3& offset) const{
	Vector3 offset_ = offset;
	Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(viewProjection_.rotation_);

	offset_ = Matrix::TransformNormal(offset_, newRotateMatrix);

	return offset_;
}

void FollowCamera::SetTarget(const EulerTransform* target){
	target_ = target;
	Reset();
}

void FollowCamera::DrawImgui(){
#ifdef _DEBUG
	ImGui::Begin("カメラ関連");
	ImGui::DragFloat3("カメラ座標", &viewProjection_.translation_.x, 0.1f);
	ImGui::DragFloat3("カメラ回転", &viewProjection_.rotation_.x, 0.01f);
	ImGui::DragFloat3("カメラのオフセット", &cameraOffset_.x, 0.01f);
	ImGui::DragFloat3("カメラの向き", &postureVec_.x, 0.01f);
	ImGui::DragFloat("オフセットY", &height_, 0.1f);
	ImGui::Text("位置補完レート = %.1f", t_);
	ImGui::Text("アングル補完レート = %.1f", angle_t);
	ImGui::DragFloat("移動限界", &limitPos_.x, 0.01f);
	limitPos_.y = -limitPos_.x;
	if (ImGui::Button("カメラシェイク")) {
		StartShake(0.5f, 1.0f);
	}
	ImGui::End();
#endif
}

