#include "LockOn.h"

void LockOn::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){
	lockOnMark_ = std::make_unique<Sprite>();
	lockOnMark_->Initialize(device, commandList);
}

void LockOn::Update(){

}

void LockOn::Draw(TextureManager* textureManager){

}
