#include "FollowCamera.h"
void FollowCamera::ApplyGlobalVariables(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Camera";

	angle_t = adjustment_item->GetfloatValue(groupName, "AngleComplement");
	t = adjustment_item->GetfloatValue(groupName, "PositionComplement");

	if (angle_t > 1.0f) {
		angle_t = 1.0f;
	}
	if (t > 1.0f) {
		t = 1.0f;
	}
}


void FollowCamera::Initialize(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Camera";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "AngleComplement", angle_t);
	adjustment_item->AddItem(groupName, "PositionComplement", t);

	destinationAngleX_ = 0.2f;

	distance = -10.0f;

	shotOffset = { 2.0f, 6.0f, -7.5f };

	rootOffset = { 0.0f, 0.0f, distance };

	minRotate = -0.31f;
	maxRotate = 0.9f;

	baseOffset = rootOffset;

}

void FollowCamera::Update(Input* input_){
	DrawImgui();
	ApplyGlobalVariables();

	cameraMove_ = { -input_->GetPadRStick().y * 0.05f,input_->GetPadRStick().x * 0.05f,0.0f };

	if (input_->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_THUMB)){
		destinationAngleY_ = target_->rotate.y;
		destinationAngleX_ = 0.2f;
	}

	destinationAngleX_ += cameraMove_.x;
	destinationAngleY_ += cameraMove_.y;


	if (destinationAngleX_ <= minRotate) {
		destinationAngleX_ = minRotate;
	}
	else if (destinationAngleX_ >= maxRotate) {
		destinationAngleX_ = maxRotate;
	}

	cameraTransform.rotate.y =
		Vector3::LerpShortAngle(cameraTransform.rotate.y, destinationAngleY_, angle_t);
	cameraTransform.rotate.x =
		Vector3::LerpShortAngle(cameraTransform.rotate.x, destinationAngleX_, angle_t);



	if (target_) {
		//追従座標の補完
		interTarget_ = Vector3::Lerp(interTarget_, target_->translate, t);
		//追従対象からカメラまでのオフセット
		Vector3 offset = offsetCalculation(baseOffset);

		//座標をコピーしてオフセット分ずらす
		cameraTransform.translate = interTarget_ + offset;

	}
}

void FollowCamera::Reset(){
	if (target_) {
		//追従座標・角度の初期化
		interTarget_ = target_->translate;
		cameraTransform.rotate.y = target_->rotate.y;
	}
	Vector3 offset = offsetCalculation(baseOffset);
	cameraTransform.translate = interTarget_ + offset;
}

Vector3 FollowCamera::offsetCalculation(const Vector3& offset) const{
	Vector3 offset_ = offset;
	Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(cameraTransform.rotate);

	offset_ = Matrix::GetInstance()->TransformNormal(offset_, newRotateMatrix);

	return offset_;
}

void FollowCamera::SetTarget(const Transform* target){
	target_ = target;
	Reset();

}

void FollowCamera::DrawImgui(){
	ImGui::Begin("カメラ関連");
	ImGui::DragFloat3("カメラ座標", &cameraTransform.translate.x, 0.01f);
	ImGui::DragFloat3("カメラ回転", &cameraTransform.rotate.x, 0.01f);
	ImGui::DragFloat3("カメラのオフセット", &cameraOffset.x, 0.01f);
	ImGui::Text("位置補完レート = %.1f", t);
	ImGui::Text("アングル補完レート = %.1f", angle_t);
	ImGui::End();
}

