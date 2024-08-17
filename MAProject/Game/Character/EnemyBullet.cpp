#include "EnemyBullet.h"

void EnemyBullet::Initialize(const Vector3& position, const Vector3& velocity){
	obj_ = std::make_unique<Object3D>();

	obj_->Initialize("box");

	obj_->transform_.translate = position;

	obj_->transform_.scale = scale_;

	velocity_ = velocity;

}

void EnemyBullet::Update(){
	auto& trans = obj_->transform_.translate;

	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	if (obj_->transform_.translate.y < -15.0f) {
		isDead_ = true;
	}

	trans += velocity_;

	obb_.center = trans;
	obb_.size = scale_;
}

void EnemyBullet::Draw(const ViewProjection& viewProjection){
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
