#include "Floor.h"

void Floor::Initialize(EulerTransform transform){
	floorObj_ = std::make_unique<Object3D>();
	floorObj_->Initialize("Floor");
	
	floorTransform_ = transform;

	floorOBB_.center = floorTransform_.translate;
	floorOBB_.size = { 1000.0f,1.0f,1000.0f };
	Matrix4x4 floorRotateMatrix = Matrix::MakeRotateMatrix(floorTransform_.rotate);
	SetOridentatios(floorOBB_, floorRotateMatrix);

	floorObj_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());
	

	isDelete_ = false;
}

void Floor::Update(){
	
	floorOBB_.center = floorTransform_.translate;
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


