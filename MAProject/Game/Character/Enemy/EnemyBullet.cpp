#include "EnemyBullet.h"

void EnemyBullet::Initialize(const EulerTransform& trans, const Vector3& velocity){
	obj_ = std::make_unique<Object3D>();

	obj_->Initialize("box");

	obj_->transform_ = trans;

	velocity_ = velocity;

}

void EnemyBullet::Update(){
	auto& trans = obj_->transform_.translate;

	auto& rotate = obj_->transform_.rotate;

	auto* mat = Matrix::GetInstance();

	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	if (obj_->transform_.translate.y < -15.0f) {
		isDead_ = true;
	}

	trans += velocity_;

	transMat_ = mat->MakeTranslateMatrix(trans);

	rotateZMat_ = mat->MakeRotateMatrixZ(rotate);

	rotateYMat_ = mat->MakeRotateMatrixY(rotate);

	rotateZInvMat_ - mat->Inverce(rotateZMat_);

	rotateMat_ = rotateZInvMat_ * rotateYMat_ * rotateZMat_;

	scaleMat_ = mat->MakeTranslateMatrix(obj_->transform_.scale);

	numMat_ = mat->MakeAffineMatrix(transMat_, rotateMat_, scaleMat_);

	obb_.center = trans;
	obb_.size = obj_->transform_.scale;
}

void EnemyBullet::Draw(const ViewProjection& viewProjection){
	obj_->SetMatrix(numMat_);
	obj_->Update(viewProjection);
	obj_->Draw();
}

Vector3 EnemyBullet::GetWorldPosition(){

	Vector3 worldPos(0, 0, 0);

	worldPos.x = obj_->transform_.translate.x;
	worldPos.y = obj_->transform_.translate.y;
	worldPos.z = obj_->transform_.translate.z;

	return worldPos;
}

void EnemyBullet::OnCollision(){
	isDead_ = true;
}
