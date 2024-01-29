#include "Enemy.h"

Enemy::~Enemy(){
}

void Enemy::ApplyGlobalVariables(){

}

void Enemy::Initialize(const Vector3& position){
	enemyModel_ = Model::LoadObjFile("Enemy");

	partsModel_ = Model::LoadObjFile("EnemyParts");

	particleModel_ = Model::LoadObjFile("box");

	bodyObj_ = std::make_unique<Object3D>();
	bodyObj_->Initialize();
	partsObj_ = std::make_unique<Object3D>();
	partsObj_->Initialize();

	for (int i = 0; i < 20; i++){
		particleObj_[i] = std::make_unique<Object3D>();
		particleObj_[i]->Initialize();
	}

	/*collisionModel_ = std::make_unique<Object3D>();
	collisionModel_->Initialize(device, commandList, "box");*/

	transform_ = {
		{0.9f,0.9f,0.9f},
		{0.0f,3.14f,0.0f},
		position
	};

	partsTransform_ = {
		{0.9f,0.9f,0.9f},
		{0.0f,0.0f,1.57f},
		{0.0f,1.7f,7.0f}
	};

	for (int i = 0; i < 20; i++) {
		particleTransform_[i] = {
			{0.1f,0.1f,0.1f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f}
		};
	}


	for (int i = 0; i < 20; i++) {
		particleVec_[i] = {0.0f,0.0f,0.0f};
	}
	
	bodyObj_->SetModel(enemyModel_.get());
	partsObj_->SetModel(partsModel_.get());
	for (int i = 0; i < 20; i++) {
		particleObj_[i]->SetModel(particleModel_.get());
	}
	

	isDead_ = false;
	isParticle_ = true;

	OBB_.center = transform_.translate;
	OBB_.size = transform_.scale;
	Matrix4x4 enemyRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(transform_.rotate);
	SetOridentatios(OBB_, enemyRotateMatrix);

}

void Enemy::Update(){
	if (enemyLife_ > 0) {
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
		partsTransform_.translate.y = transform_.translate.y + 2.7f;
		partsTransform_.translate.z = transform_.translate.z;

		partsTransform_.rotate.x += 0.3f;

	}
	else {
		transform_.translate.y += 0.1f;
		transform_.translate.z += 0.1f;
		transform_.rotate.x += 0.3f;
		partsTransform_.translate.y = transform_.translate.y + 2.7f;
		partsTransform_.translate.z = transform_.translate.z;
		partsTransform_.rotate.x += 0.3f;
		if (transform_.scale.x > 0.0f) {
			transform_.scale.x -= 0.005f;
			transform_.scale.y -= 0.005f;
			transform_.scale.z -= 0.005f;
		}
		else {
			isDead_ = true;
		}
	}
	if (invincibleTime_>0){
		invincibleTime_--;
	}
	for (int i = 0; i < particleNum_; i++) {
		particleTransform_[i].translate += particleVec_[i];
	}

	matrix_ = Matrix::GetInstance()->MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	partsMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(partsTransform_.scale, partsTransform_.rotate, partsTransform_.translate);
	
	for (int i = 0; i < particleNum_; i++) {
		particleMatrix_[i] = Matrix::GetInstance()->MakeAffineMatrix(particleTransform_[i].scale, particleTransform_[i].rotate, particleTransform_[i].translate);
		
	}


	OBB_.center = transform_.translate;
	OBB_.size = transform_.scale;
	Matrix4x4 enemyRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(transform_.rotate);
	SetOridentatios(OBB_, enemyRotateMatrix);
}

void Enemy::Draw(TextureManager* textureManager, const ViewProjection& viewProjection){
	if (isDead_) {
		return;
	}
	bodyObj_->Update(matrix_, viewProjection);
	bodyObj_->Draw(textureManager->SendGPUDescriptorHandle(5));
	
	partsObj_->Update(partsMatrix_, viewProjection);
	partsObj_->Draw(textureManager->SendGPUDescriptorHandle(6));

	if (isParticle_){
		for (int i = 0; i < 10; i++) {
			particleObj_[i]->Update(particleMatrix_[i], viewProjection);
			particleObj_[i]->Draw(textureManager->SendGPUDescriptorHandle(9));
		}
		for (int i = 10; i < 20; i++) {
			particleObj_[i]->Update(particleMatrix_[i], viewProjection);
			particleObj_[i]->Draw(textureManager->SendGPUDescriptorHandle(9));
		}
	}
}

void Enemy::DrawImgui(){

}

void Enemy::onFlootCollision(OBB obb){

}

void Enemy::Respawn(const Vector3& position){
	isDead_ = false;
	enemyLife_ = 10;
	transform_ = {
		{0.9f,0.9f,0.9f},
		{0.0f,3.14f,0.0f},
		position
	};
}

void Enemy::OnCollision(){
	if (invincibleTime_<=0){
		enemyLife_--;
		invincibleTime_ = invincibleTimeBase_;
		ParticleMove();
	}	
	
}

const Vector3 Enemy::GetCenterPos()const{
	const Vector3 offset = { 0.0f,1.0f,0.0f };
	//ワールドに変換
	
	Vector3 world = Matrix::GetInstance()->TransformVec(offset, matrix_);

	return world;

}

void Enemy::ParticleMove(){

	if (enemyLife_ == 2) {
		for (int i = 0; i < 5; i++){
			particleTransform_[i].translate = transform_.translate;

			Vector3 velocity = { 0, 0, 0 };
			float numberX = (rand() % 11 - 5) / 2.0f;
			float numberY = ((rand() % 11) / 2.0f);
			float numberZ = (rand() % 11 - 5) / 2.0f;

			velocity = { numberX / 10.0f, numberY / 10.0f, numberZ / 10.0f };

			particleVec_[i] = velocity;
		}

	}else if (enemyLife_ == 1) {
		for (int i = 5; i < 10; i++) {
			particleTransform_[i].translate = transform_.translate;
			Vector3 velocity = { 0, 0, 0 };
			float numberX = (rand() % 11 - 5) / 2.0f;
			float numberY = static_cast<float>((rand() % 11) / 2.0f);
			float numberZ = (rand() % 11 - 5) / 2.0f;

			velocity = { numberX, numberY / 10.0f, numberZ };

			particleVec_[i] = velocity;
		}

	}else if (enemyLife_ == 0) {
		for (int i = 10; i < particleNum_; i++) {
			particleTransform_[i].translate = transform_.translate;
			Vector3 velocity = { 0, 0, 0 };
			float numberX = (rand() % 11 - 5) / 2.0f;
			float numberY = static_cast<float>((rand() % 11) / 2.0f);
			float numberZ = (rand() % 11 - 5) / 2.0f;

			velocity = { numberX, numberY / 10.0f, numberZ };

			particleVec_[i] = velocity;
		}

	}
	else {
		for (int i = 10; i < particleNum_; i++) {
			particleTransform_[i].translate = transform_.translate;
			Vector3 velocity = { 0, 0, 0 };
			float numberX = (rand() % 11 - 5) / 2.0f;
			float numberY = static_cast<float>((rand() % 11) / 2.0f);
			float numberZ = (rand() % 11 - 5) / 2.0f;

			velocity = { numberX, numberY / 10.0f, numberZ };

			particleVec_[i] = velocity;
		}
	}
	
	
}
