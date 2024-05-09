#include "Floor.h"

void Floor::Initialize(EulerTransform transform){
	
	
	floorTransform_ = transform;

	floorOBB_.center = floorTransform_.translate;
	floorOBB_.size = floorTransform_.scale;
	Matrix4x4 floorRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(floorTransform_.rotate);
	SetOridentatios(floorOBB_, floorRotateMatrix);

	isDelete_ = false;
}

void Floor::Update(){
	if (isMove_){
		floorTransform_.translate += moveSpeed_ * Magnification;
		moveMax += moveSpeed_.x * Magnification;
		if (moveMax <= -4.0f) {
			Magnification *= -1.0f;
		}
		else if (moveMax >= 4.0f) {
			Magnification *= -1.0f;
		}
	}
	floorOBB_.center = floorTransform_.translate;
	floorOBB_.size = floorTransform_.scale;
	Matrix4x4 floorRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(floorTransform_.rotate);
	SetOridentatios(floorOBB_, floorRotateMatrix);

	floorMatrix_ = 
		Matrix::GetInstance()->MakeAffineMatrix(floorTransform_.scale, floorTransform_.rotate, floorTransform_.translate);
}

void Floor::Draw(Object3D* object,const ViewProjection& viewProjection){
	if (floorModel_ == nullptr) {
		floorModel_ = object;
	}
	

	floorModel_->Update(floorMatrix_, viewProjection);
	floorModel_->Draw();
}

void Floor::DrawImgui(){
	/*ImGui::DragFloat3("床の座標", &floorTransform_.translate.x, 0.01f);
	ImGui::DragFloat3("床の回転", &floorTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("床の大きさ", &floorTransform_.scale.x, 0.01f);
	ImGui::Checkbox("動くかどうか", &isMove_);
	if (ImGui::Button("このオブジェを削除")) {
		isDelete_ = true;
	}*/
}


