#include "Particle.h"

Particle::~Particle(){
	delete model_;
}

void Particle::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){
	model_ = new Object3D();
	model_->Initialize(device, commandList, "box");
}

void Particle::Update(){
	matrix_ = Matrix::GetInstance()->MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
}

void Particle::Draw(TextureManager* textureManager, const ViewProjection& viewProjection){
	model_->Update(matrix_, viewProjection);
	model_->Draw(textureManager->SendGPUDescriptorHandle(9));
}

