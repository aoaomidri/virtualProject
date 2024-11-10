#include "FollowCamera.h"
#include"LockOn.h"
void FollowCamera::ApplyGlobalVariables(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Camera";

	angle_t = adjustment_item->GetfloatValue(groupName, "AngleComplement");
	t_ = adjustment_item->GetfloatValue(groupName, "PositionComplement");
	distance = adjustment_item->GetfloatValue(groupName, "distance");

	if (angle_t > 1.0f) {
		angle_t = 1.0f;
	}
	if (t_ > 1.0f) {
		t_ = 1.0f;
	}
}


void FollowCamera::Initialize(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Camera";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "AngleComplement", angle_t);
	adjustment_item->AddItem(groupName, "PositionComplement", t_);
	adjustment_item->AddItem(groupName, "distance", distance);

	input_ = Input::GetInstance();

	destinationAngleX_ = 0.0f;


	rootOffset = { 0.0f, height_, distance };

	minRotate = 0.1f;
	maxRotate = 0.9f;

	baseOffset = rootOffset;

	destinationAngleX_ = 0.2f;

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

}

void FollowCamera::Update(){
	frontVec_ = postureVec_;
	
	ApplyGlobalVariables();


	if (lockOn_ && lockOn_->target_) {
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		Vector3 sub = lockOnPos - target_->translate;
		sub.y = 0;
		postureVec_ = sub;

		Matrix4x4 newMatrix;
		newMatrix.DirectionToDirection(Vector3::Normalize(Vec), Vector3::Normalize(postureVec_));
		destinationAngleY_ = Matrix::RotateAngleYFromMatrix(newMatrix);
		destinationAngleX_ = 0.15f;
	}
	else {

		if (input_->GetConnectPad() && isMove_) {
			cameraMove_ = { -input_->GetPadRStick().y * 0.05f,input_->GetPadRStick().x * 0.05f,0.0f };
			Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(viewProjection_.rotation_);
			postureVec_ = Matrix::TransformNormal(Vec, newRotateMatrix);
			postureVec_.y = 0.0f;
			postureVec_ = Vector3::Normalize(postureVec_);
			if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_RIGHT_THUMB)) {
				destinationAngleY_ = Matrix::RotateAngleYFromMatrix(*targetRotateMatrix);
				destinationAngleX_ = 0.2f;
			}
		}
	}
	
	/*if (input_->GetConnectPad()) {*/
		destinationAngleX_ += cameraMove_.x;
		destinationAngleY_ += cameraMove_.y;


		if (destinationAngleX_ <= minRotate) {
			destinationAngleX_ = minRotate;
		}
		else if (destinationAngleX_ >= maxRotate) {
			destinationAngleX_ = maxRotate;
		}

		viewProjection_.rotation_.y =
			Vector3::LerpShortAngle(viewProjection_.rotation_.y, destinationAngleY_, angle_t);
		viewProjection_.rotation_.x =
			Vector3::LerpShortAngle(viewProjection_.rotation_.x, destinationAngleX_, angle_t);
	//}
	rootOffset = { 0.0f, height_, distance };
	baseOffset = rootOffset;

	if (target_) {
		//追従座標の補完
		interTarget_ = Vector3::Lerp(interTarget_, target_->translate, t_);
		//追従対象からカメラまでのオフセット
		Vector3 offset = offsetCalculation(baseOffset);

		//座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = interTarget_ + offset;

	}


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
	Vector3 offset = offsetCalculation(baseOffset);
	viewProjection_.translation_ = interTarget_ + offset;
}

void FollowCamera::RotateReset(){
	destinationAngleY_ = Matrix::RotateAngleYFromMatrix(*targetRotateMatrix);
	destinationAngleX_ = 0.2f;
}

void FollowCamera::CameraPosInit(){
	destinationAngleY_ = Matrix::RotateAngleYFromMatrix(*targetRotateMatrix);
	destinationAngleX_ = 0.2f;
	cameraMove_ = { 0,0,0 };
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
	ImGui::DragFloat3("カメラのオフセット", &cameraOffset.x, 0.01f);
	ImGui::DragFloat3("カメラの向き", &postureVec_.x, 0.01f);
	ImGui::DragFloat("オフセットY", &height_, 0.1f);
	ImGui::Text("位置補完レート = %.1f", t_);
	ImGui::Text("アングル補完レート = %.1f", angle_t);

	ImGui::End();
#endif
}

