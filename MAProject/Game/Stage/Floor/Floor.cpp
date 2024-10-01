#include "Floor.h"

void Floor::Initialize(EulerTransform transform){
	floorObj_ = std::make_unique<Object3D>();
	floorObj_->Initialize("Floor");
	
	floorTransform_ = transform;

	floorOBB_.center = floorTransform_.translate;
	floorOBB_.size = floorTransform_.scale;
	Matrix4x4 floorRotateMatrix = Matrix::MakeRotateMatrix(floorTransform_.rotate);
	SetOridentatios(floorOBB_, floorRotateMatrix);

	floorObj_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());
	

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
	Matrix4x4 floorRotateMatrix = Matrix::MakeRotateMatrix(floorTransform_.rotate);
	SetOridentatios(floorOBB_, floorRotateMatrix);

	floorObj_->transform_ = floorTransform_;
}

void Floor::Draw(const ViewProjection& viewProjection){

	floorObj_->Update(viewProjection);
	floorObj_->Draw();
}

void Floor::DrawImgui(){
	floorObj_->DrawImgui("床");
}


