#include "Enemy.h"
uint32_t Enemy::nextSerialNumber_ = 0;

Enemy::Enemy(){
	serialNumber_ = nextSerialNumber_;
	++nextSerialNumber_;
}

Enemy::~Enemy(){
}

void Enemy::ApplyGlobalVariables(){

}

void Enemy::Initialize(const Vector3& position){
	enemyModel_ = Model::LoadObjFile("Enemy");

	partsModel_ = Model::LoadObjFile("EnemyParts");

	particleModel_ = Model::LoadObjFile("box");

	bodyObj_ = std::make_unique<Object3D>();
	bodyObj_->Initialize(enemyModel_.get());
	partsObj_ = std::make_unique<Object3D>();
	partsObj_->Initialize(partsModel_.get());

	collisionObj_ = std::make_unique<Object3D>();
	collisionObj_->Initialize(particleModel_.get());

	for (int i = 0; i < 20; i++){
		particleObj_[i] = std::make_unique<Object3D>();
		particleObj_[i]->Initialize(particleModel_.get());
	}
	
	transform_ = {
		{0.9f,0.9f,0.9f},
		{0.0f,3.14f,0.0f},
		position
	};
	collisionTransform_ = transform_;

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

	isDead_ = false;
	isNoLife_ = false;
	isParticle_ = true;

	OBB_.center = transform_.translate;
	OBB_.size = { transform_.scale.x + 1.0f,transform_.scale.y + 4.0f,transform_.scale.z + 1.0f };
	collisionTransform_.translate = OBB_.center;
	collisionTransform_.scale = OBB_.size;
	bodyOBB_.center = transform_.translate;
	bodyOBB_.size = { transform_.scale.x + 3.0f,transform_.scale.y + 2.0f,transform_.scale.z + 3.0f };

	Matrix4x4 enemyRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(transform_.rotate);
	SetOridentatios(OBB_, enemyRotateMatrix);
	SetOridentatios(bodyOBB_, enemyRotateMatrix);

	behaviorRequest_ = Behavior::kRoot;

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

}

void Enemy::Update(){
	MotionUpdate();

	for (int i = 0; i < particleNum_; i++) {
		particleTransform_[i].translate += particleVec_[i];
	}
	collisionTransform_ = transform_;

	scaleMatrix_ = Matrix::GetInstance()->MakeScaleMatrix(transform_.scale);
	transformMatrix_ = Matrix::GetInstance()->MakeTranslateMatrix(transform_.translate);


	matrix_ = Matrix::GetInstance()->MakeAffineMatrix(scaleMatrix_, rotateMatrix_, transformMatrix_);
	partsMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(partsTransform_.scale, partsTransform_.rotate, partsTransform_.translate);
	
	for (int i = 0; i < particleNum_; i++) {
		particleMatrix_[i] = Matrix::GetInstance()->MakeAffineMatrix(particleTransform_[i].scale, particleTransform_[i].rotate, particleTransform_[i].translate);
		
	}


	OBB_.center = transform_.translate;
	bodyOBB_.center = transform_.translate;
	collisionTransform_.scale = bodyOBB_.size;

	collisionMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(collisionTransform_);
	SetOridentatios(OBB_, rotateMatrix_);
	SetOridentatios(bodyOBB_, rotateMatrix_);
}

void Enemy::Draw(const ViewProjection& viewProjection){
	if (isDead_) {
		return;
	}
	bodyObj_->Update(matrix_, viewProjection);
	bodyObj_->Draw();

#ifdef _DEBUG

	/*collisionObj_->Update(collisionMatrix_, viewProjection);
	collisionObj_->Draw();*/
#endif // _DEBUG

	partsObj_->Update(partsMatrix_, viewProjection);
	partsObj_->Draw();

	if (isParticle_){
		for (int i = 0; i < 10; i++) {
			particleObj_[i]->Update(particleMatrix_[i], viewProjection);
			particleObj_[i]->Draw();
		}
		for (int i = 10; i < 20; i++) {
			particleObj_[i]->Update(particleMatrix_[i], viewProjection);
			particleObj_[i]->Draw();
		}
	}
}

void Enemy::DrawImgui(){

}

void Enemy::onFlootCollision(OBB obb){

}

void Enemy::Respawn(const Vector3& position){
	transform_ = {
		{0.9f,0.9f,0.9f},
		{0.0f,3.14f,0.0f},
		position
	};
	collisionTransform_ = transform_;

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
		particleVec_[i] = { 0.0f,0.0f,0.0f };
	}

	isDead_ = false;
	isNoLife_ = false;
	enemyLife_ = 10;
	isParticle_ = true;

	OBB_.center = transform_.translate;
	OBB_.size = { transform_.scale.x + 1.0f,transform_.scale.y + 4.0f,transform_.scale.z + 1.0f };
	collisionTransform_.translate = OBB_.center;
	collisionTransform_.scale = OBB_.size;
	bodyOBB_.center = transform_.translate;
	bodyOBB_.size = { transform_.scale.x + 3.0f,transform_.scale.y + 2.0f,transform_.scale.z + 3.0f };

	Matrix4x4 enemyRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(transform_.rotate);
	SetOridentatios(OBB_, enemyRotateMatrix);
	SetOridentatios(bodyOBB_, enemyRotateMatrix);

	behaviorRequest_ = Behavior::kRoot;
	behavior_ = Behavior::kRoot;

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

}

void Enemy::OnCollision(){
	enemyLife_--;
	ParticleMove();
	
}

const Vector3 Enemy::GetCenterPos()const{
	const Vector3 offset = { 0.0f,5.0f,0.0f };
	//ワールドに変換
	
	Vector3 world = Matrix::GetInstance()->TransformVec(offset, matrix_);

	return world;

}

void Enemy::ParticleMove(){

	if (enemyLife_ <= 1) {
		for (int i = 0; i < 10; i++) {
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

void Enemy::MotionUpdate(){
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kFirst:
			BehaviorFirstInitialize();
			break;
		case Behavior::kSecond:
			BehaviorSecondInitialize();
			break;
		case Behavior::kThird:
			BehaviorThirdInitialize();
			break;
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kDead:
			BehaviorDeadInitialize();
			break;
		}
	}
	// 振る舞いリクエストをリセット
	behaviorRequest_ = std::nullopt;

	switch (behavior_) {
	case Behavior::kFirst:
	default:
		FirstMotion();
		break;
	case Behavior::kSecond:
		SecondMotion();
		break;
	case Behavior::kThird:
		ThirdMotion();
		break;
	case Behavior::kRoot:
		RootMotion();
		break;
	case Behavior::kDead:
		DeadMotion();
		break;
	}

	if (enemyLife_ <= 0 && behavior_ != Behavior::kDead) {
		isNoLife_ = true;
		behaviorRequest_ = Behavior::kDead;
	}
}

void Enemy::BehaviorFirstInitialize(){

}

void Enemy::BehaviorSecondInitialize(){

}

void Enemy::BehaviorThirdInitialize(){

}

void Enemy::BehaviorRootInitialize(){
	rotateMatrix_ = Matrix::GetInstance()->MakeIdentity4x4();
}

void Enemy::BehaviorDeadInitialize(){
	Vector3 deadMoveBase = { 0,0.02f,0.1f };
	deadMove_ = Matrix::GetInstance()->TransformNormal(deadMoveBase, rotateMatrix_);
	deadMove_ = Vector3::Mutiply(Vector3::Normalize(deadMove_), 0.5f);
	deadMove_.y *= -1.00f;
	deadYAngle_ = Matrix::GetInstance()->RotateAngleYFromMatrix(rotateMatrix_);
	transform_.rotate.y = deadYAngle_;
}

void Enemy::FirstMotion(){

}

void Enemy::SecondMotion(){

}

void Enemy::ThirdMotion(){

}

void Enemy::RootMotion(){
	frontVec_ = postureVec_;

	Vector3 move = { moveSpeed_ * magnification ,0,0 };

	move = Matrix::GetInstance()->TransformNormal(move, rotateMatrix_);
	move.y = 0;
	/*敵の移動*/
	transform_.translate += move;

	
	if (target_){
		Vector3 lockOnPos = target_->translate;
		Vector3 sub = lockOnPos - transform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		postureVec_ = sub;

		Matrix4x4 directionTodirection_ = Matrix::GetInstance()->DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		rotateMatrix_ = Matrix::GetInstance()->Multiply(rotateMatrix_, directionTodirection_);
	}
	

	/*エネミーのパーツ*/
	Vector3 parts_offset = { 0.0f, 2.7f, 0.0f };
	//Vector3 R_parts_offset = { -7.0f, 7.0f, 0.0f };
	parts_offset = Matrix::GetInstance()->TransformNormal(parts_offset, rotateMatrix_);

	partsTransform_.translate = transform_.translate + parts_offset;

	partsTransform_.rotate.x += 0.3f;
}

void Enemy::DeadMotion(){
	transform_.translate -= deadMove_;
	transform_.rotate.x += 0.3f;	
	Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(transform_.rotate);
	rotateMatrix_ = newRotateMatrix;
	Vector3 parts_offset = { 0.0f, 2.7f, 0.0f };
	//Vector3 R_parts_offset = { -7.0f, 7.0f, 0.0f };
	parts_offset = Matrix::GetInstance()->TransformNormal(parts_offset, rotateMatrix_);

	partsTransform_.translate = transform_.translate + parts_offset;
	
	partsTransform_.rotate.x += 0.3f;
	if (partsTransform_.scale.x > 0.0f) {
		partsTransform_.scale -= 0.0075f;
	}
	if (transform_.scale.x > 0.0f) {
		transform_.scale -= 0.0075f;
	}
	else {
		isDead_ = true;
	}
}

