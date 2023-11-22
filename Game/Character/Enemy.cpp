#include "Enemy.h"

void Enemy::ApplyGlobalVariables(){

}

void Enemy::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){
	model_ = std::make_unique<Object3D>();
	model_->Initialize(device, commandList, "Enemy");

	partsModel_ = std::make_unique<Object3D>();
	partsModel_->Initialize(device, commandList, "EnemyParts");

	collisionModel_ = std::make_unique<Object3D>();
	collisionModel_->Initialize(device, commandList, "box");

	transform_ = {
		{0.3f,0.3f,0.3f},
		{0.0f,3.14f,0.0f},
		{0.0f,0.8f,7.0f}
	};

	partsTransform_ = {
		{0.3f,0.3f,0.3f},
		{0.0f,0.0f,1.57f},
		{0.0f,1.7f,7.0f}
	};

	isDead_ = false;


	OBB_.center = transform_.translate;
	OBB_.size = transform_.scale;
	Matrix4x4 enemyRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(transform_.rotate);
	SetOridentatios(OBB_, enemyRotateMatrix);

}

void Enemy::Update(){
	if (isDead_) {
		return;
	}
	/*敵の移動*/
	transform_.translate.x += moveSpeed_ * magnification;

	if (transform_.translate.x <= -2.0f) {
		magnification *= -1.0f;
	}
	else if (transform_.translate.x >= 2.0f) {
		magnification *= -1.0f;
	}
	/*エネミーのパーツ*/
	partsTransform_.translate.x = transform_.translate.x;
	partsTransform_.translate.y = transform_.translate.y + 0.9f;
	partsTransform_.translate.z = transform_.translate.z;

	partsTransform_.rotate.x += 0.3f;

	matrix_ = Matrix::GetInstance()->MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	partsMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(partsTransform_.scale, partsTransform_.rotate, partsTransform_.translate);

	OBB_.center = transform_.translate;
	OBB_.size = transform_.scale;
	Matrix4x4 enemyRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(transform_.rotate);
	SetOridentatios(OBB_, enemyRotateMatrix);


}

void Enemy::Draw(TextureManager* textureManager, const ViewProjection& viewProjection){
	if (isDead_) {
		return;
	}
	model_->Update(matrix_, viewProjection);
	model_->Draw(textureManager->SendGPUDescriptorHandle(5));

	partsModel_->Update(partsMatrix_, viewProjection);
	partsModel_->Draw(textureManager->SendGPUDescriptorHandle(6));
}

void Enemy::DrawImgui(){

}

void Enemy::onFlootCollision(OBB obb){

}

void Enemy::Respawn(){
	isDead_ = false;
	transform_.translate = { 0.0f,0.8f,7.0f };
}
