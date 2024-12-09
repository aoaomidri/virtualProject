#include "Enemy.h"
#include"ImGuiManager.h"
#include"RandomMaker.h"
#include"Adjustment_Item.h"
#include"LevelLoader/LevelLoader.h"
#include"PostEffect.h"


uint32_t Enemy::nextSerialNumber_ = 0;

uint32_t Enemy::enemyDestroyingNumber_ = 0;

Enemy::Enemy(){
	serialNumber_ = nextSerialNumber_;
	++nextSerialNumber_;
}

Enemy::~Enemy(){
}

void Enemy::ApplyGlobalVariables(){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Enemy";

	nearPlayer_ = adjustment_item->GetfloatValue(groupName, "NearPlayer");
	farPlayer_ = adjustment_item->GetfloatValue(groupName, "FarPlayer");
	lengthJudgment_ = adjustment_item->GetIntValue(groupName, "LengthJudgment");
	moveSpeed_ = adjustment_item->GetfloatValue(groupName, "MoveSpeed");
	dashSpeed_ = adjustment_item->GetfloatValue(groupName, "DownSpeed");
	backSpeed_ = adjustment_item->GetfloatValue(groupName, "BackSpeed");
	enemyLifeMax_ = adjustment_item->GetIntValue(groupName, "EnemyLifeMax");
	freeTimeMax_ = adjustment_item->GetIntValue(groupName, "FreeTimeMax");
	hitBackSpeed_ = adjustment_item->GetfloatValue(groupName, "HitBackSpeed");
	strongHitBackSpeed_ = adjustment_item->GetfloatValue(groupName, "StrongHitBackSpeed");
	/*ノックバック関連*/
	hitEaseStartLeft_ = adjustment_item->GetVector3Value(groupName, "HitEaseLeft");
	hitEaseStartRight_ = adjustment_item->GetVector3Value(groupName, "HitEaseRight");
	hitEaseStartCenter_ = adjustment_item->GetVector3Value(groupName, "HitEaseCenter");
}

void Enemy::Initialize(const Vector3& position){
	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();
	const char* groupName = "Enemy";
	//グループを追加
	adjustment_item->CreateGroup(groupName);
	//アイテムの追加
	adjustment_item->AddItem(groupName, "NearPlayer", nearPlayer_);
	adjustment_item->AddItem(groupName, "FarPlayer", farPlayer_);
	adjustment_item->AddItem(groupName, "LengthJudgment", lengthJudgment_);
	adjustment_item->AddItem(groupName, "MoveSpeed", moveSpeed_);
	adjustment_item->AddItem(groupName, "DownSpeed", dashSpeed_);
	adjustment_item->AddItem(groupName, "BackSpeed", backSpeed_);
	adjustment_item->AddItem(groupName, "EnemyLifeMax", enemyLifeMax_);
	adjustment_item->AddItem(groupName, "FreeTimeMax", freeTimeMax_);
	adjustment_item->AddItem(groupName, "HitBackSpeed", hitBackSpeed_);
	adjustment_item->AddItem(groupName, "StrongHitBackSpeed", strongHitBackSpeed_);
	/*ノックバック関連*/
	adjustment_item->AddItem(groupName, "HitEaseLeft", hitEaseStartLeft_);
	adjustment_item->AddItem(groupName, "HitEaseRight", hitEaseStartRight_);
	adjustment_item->AddItem(groupName, "HitEaseCenter", hitEaseStartCenter_);

	enemyLifeMax_ = adjustment_item->GetIntValue(groupName, "EnemyLifeMax");

	enemyLife_ = enemyLifeMax_;

	auto levelLoader = LevelLoader::GetInstance(); 

	bodyObj_ = std::make_unique<Object3D>();
	bodyObj_->Initialize("Enemy");
	bodyObj_->SetIsLighting(true);
	bodyObj_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());

	partsObj_ = std::make_unique<Object3D>();
	partsObj_->Initialize("EnemyParts");
	partsObj_->SetIsLighting(true);
	partsObj_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());


	collisionObj_ = std::make_unique<Object3D>();
	collisionObj_->Initialize("box");
	
	transform_ = {
		levelLoader->GetLevelObjectTransform("Enemy").scale,
		{0.0f,0.0f,0.0f},
		position
	};
	collisionTransform_ = transform_;

	partsTransform_ = {
		{0.9f,0.9f,0.9f},
		{0.0f,0.0f,1.57f},
		{0.0f,1.7f,7.0f}
	};

	emitter_.count = 10;
	emitter_.transform = {
		levelLoader->GetLevelObjectTransform("Enemy").scale,
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
	emitter_.transform.scale /= 2.0f;

	particle_ = std::make_unique<ParticleBase>();
	particle_->Initialize(emitter_, false);
	particle_->SetIsDraw(false);
	particle_->SetIsBillborad(true);
	particle_->SetIsUpper(true);
	particle_->SetIsAlignedToMovement(false);
	particle_->SetLifeTime(0.3f);
	particle_->SetVelocityRange(Vector2(-30.0f, 30.0f));

	isDead_ = false;
	isNoLife_ = false;
	isParticle_ = true;

	threshold_ = 0.0f;

	freeTime_ = 0;

	collisionTransform_.translate = attackOBB_.center;
	collisionTransform_.scale = attackOBB_.size;
	bodyOBB_.center = transform_.translate;
	bodyOBB_.size = { transform_.scale.x + 3.0f,transform_.scale.y + 2.0f,transform_.scale.z + 3.0f };

	Matrix4x4 enemyRotateMatrix = Matrix::MakeRotateMatrix(transform_.rotate);
	SetOridentatios(bodyOBB_, enemyRotateMatrix);
	SetOridentatios(attackOBB_, enemyRotateMatrix);

	behaviorRequest_ = Behavior::kRoot;

	behavior_ = Behavior::kRoot;

	postureVec_ = { 0.0f,0.0f,-1.0f };
	frontVec_ = { 0.0f,0.0f,-1.0f };
	
	shadow_ = std::make_unique<Sprite>();
	shadow_->Initialize(TextureManager::GetInstance()->Load("resources/texture/shadow.png"));
	shadow_->rotation_.x = 1.57f;
	shadow_->scale_ = { 1.5f,1.5f };
	shadow_->anchorPoint_ = { 0.5f,0.5f };
	shadow_->color_.w = 0.5f;

	if (serialNumber_ % 2 == 0) {
		magnification_ = 1.0f;
	}
	else {
		magnification_ = -1.0f;
	}
	

}

void Enemy::Update(){
	ApplyGlobalVariables();
	bodyObj_->SetDissolve(threshold_);
	partsObj_->SetDissolve(threshold_);

	bullets_.remove_if([](const std::unique_ptr<EnemyBullet>& bullet) {
		if (bullet->IsDead()) {
			return true;
		}
		return false;
	});

	bodyObj_->SetColor(enemyColor_);
	partsObj_->SetColor(enemyColor_);

	MotionUpdate();

	shadow_->position_ = transform_.translate;
	shadow_->position_.y = 1.11f;


	collisionTransform_ = transform_;

	scaleMatrix_ = Matrix::MakeScaleMatrix(transform_.scale);
	transformMatrix_ = Matrix::MakeTranslateMatrix(transform_.translate);

	Matrix4x4 resultRotateMat = Matrix::MakeRotateMatrix(transform_.rotate) * rotateMatrix_;

	matrix_ = Matrix::MakeAffineMatrix(scaleMatrix_, resultRotateMat, transformMatrix_);

	resultRotateMat = Matrix::MakeRotateMatrix(partsTransform_.rotate) * resultRotateMat;

	partsMatrix_ = Matrix::MakeAffineMatrix(partsTransform_.scale, resultRotateMat, partsTransform_.translate);
	
	/*for (int i = 0; i < particleNum_; i++) {
		particleMatrix_[i] = Matrix::MakeAffineMatrix(particleTransform_[i].scale, particleTransform_[i].rotate, particleTransform_[i].translate);
		
	}*/

	for (auto it = bullets_.begin(); it != bullets_.end(); ++it) {
		(*it)->Update();
	}

	bodyOBB_.center = transform_.translate;
	if (isNearAttack_){
		Vector3 attackOffset = { 0.0f, 0.0f, 4.0f };
		attackOffset = Matrix::TransformNormal(attackOffset, rotateMatrix_);

		attackOBB_.center = bodyOBB_.center + attackOffset;
	}
	else{
		attackOBB_.center = bodyOBB_.center;
	}

	SetOridentatios(bodyOBB_, rotateMatrix_);
	SetOridentatios(attackOBB_, rotateMatrix_);

	collisionMatrix_ = Matrix::MakeAffineMatrix(attackOBB_.size, rotateMatrix_, attackOBB_.center);
}

void Enemy::Draw(const ViewProjection& viewProjection){

	for (auto it = bullets_.begin(); it != bullets_.end(); ++it) {
		(*it)->Draw(viewProjection);
	}
	

	if (isDead_) {
		return;
	}
	bodyObj_->SetMatrix(matrix_);
	bodyObj_->Update(viewProjection);
	bodyObj_->Draw();

	//boxObj_->Update(viewProjection);
	//boxObj_->Draw();

#ifdef _DEBUG
	/*collisionObj_->SetMatrix(collisionMatrix_);
	collisionObj_->Update(viewProjection);
	collisionObj_->Draw();*/
#endif // _DEBUG
	partsObj_->SetMatrix(partsMatrix_);
	partsObj_->SetTimeScale(timeScale_);
	partsObj_->Update(viewProjection);
	partsObj_->Draw();

	
}

void Enemy::TexDraw(const Matrix4x4& viewProjection){
	shadow_->Draw(viewProjection);
}

void Enemy::ParticleDraw(const ViewProjection& viewProjection, const Vector3& color){
	EulerTransform patTransform = transform_;
	patTransform.translate.y += 1.0f;
	particle_->SetOneColor(color);
	particle_->Update(patTransform, viewProjection);
	particle_->Draw();
}

void Enemy::DrawImgui() {
#ifdef _DEBUG
	ImGui::Begin("敵の変数");
	ImGui::DragFloat3("敵の座標", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("敵の回転", &transform_.rotate.x, 0.01f);
	ImGui::Text("%.2f", rotateMatrix_.RotateAngleYFromMatrix());
	ImGui::DragFloat("プレイヤーとの距離", &playerLength_, 0.1f);
	ImGui::DragFloat("ディゾルブの変数", &threshold_, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("回転攻撃の射程補正変数", &attackLength_, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat3("回転", &slashAngle_.x, 0.01f, 0.0f, 3.14f);
	ImGui::DragFloat4("色", &enemyColor_.x, 0.01f, 0.0f, 1.0f);
	if (ImGui::Button("敵被弾ボタン")){
		OnCollision();
	}
	ImGui::End();
#endif
}

void Enemy::Respawn(const Vector3& position){
	auto levelLoader = LevelLoader::GetInstance();

	transform_ = {
		levelLoader->GetLevelObjectTransform("Enemy").scale,
		{0.0f,3.14f,0.0f},
		position
	};
	collisionTransform_ = transform_;

	partsTransform_ = {
		{0.9f,0.9f,0.9f},
		{0.0f,0.0f,1.57f},
		{0.0f,1.7f,7.0f}
	};

	isDead_ = false;
	isNoLife_ = false;
	isParticle_ = true;
	enemyLife_ = enemyLifeMax_;
	threshold_ = 0.0f;

	freeTime_ = 0;

	bodyOBB_.center = transform_.translate;
	bodyOBB_.size = { transform_.scale.x + 3.0f,transform_.scale.y + 2.0f,transform_.scale.z + 3.0f };
	collisionTransform_.translate = bodyOBB_.center;
	collisionTransform_.scale = bodyOBB_.size;

	Matrix4x4 enemyRotateMatrix = Matrix::MakeRotateMatrix(transform_.rotate);
	SetOridentatios(bodyOBB_, enemyRotateMatrix);
	SetOridentatios(attackOBB_, enemyRotateMatrix);

	behaviorRequest_ = Behavior::kRoot;

	behavior_ = Behavior::kRoot;

	postureVec_ = { 0.0f,0.0f,-1.0f };
	frontVec_ = { 0.0f,0.0f,-1.0f };

}

void Enemy::OnCollision(){
	particle_->SetIsDraw(true);
	enemyLife_--;
	particle_->AddParticle(emitter_);
	behaviorRequest_ = Behavior::kLeaningBack;
}

void Enemy::OnCollisionStrong(){
	particle_->SetIsDraw(true);
	enemyLife_ -= 3;
	particle_->AddParticle(emitter_);
	behaviorRequest_ = Behavior::kLeaningBack;
}

void Enemy::OnCollisionGuard(){
	behaviorRequest_ = Behavior::kLeaningBack;
}

const Vector3 Enemy::GetCenterPos()const{
	const Vector3 offset = { 0.0f,5.0f,0.0f };
	//ワールドに変換
	
	Vector3 world = Matrix::TransformVec(offset, matrix_);

	return world;

}

void Enemy::ParticleMove(){

	
}

void Enemy::MotionUpdate(){
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kBack:
			BehaviorBackInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;
		case Behavior::kRun:
			BehaviorRunInitialize();
			break;
		case Behavior::kFree:
			BehaviorFreeInitialize();
			break;
		case Behavior::kDead:
			BehaviorDeadInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kPreliminalyAction:
			BehaviorPreliminalyActionInitialize();
			break;
		case Behavior::kLeaningBack:
			BehaviorLeaningBackInitialize();
			break;
		}
	}
	// 振る舞いリクエストをリセット
	behaviorRequest_ = std::nullopt;

	switch (behavior_) {
	case Behavior::kRoot:
		RootMotion();
		break;
	case Behavior::kBack:
		BackStep();
		break;
	case Behavior::kDash:
		Dash();
		break;
	case Behavior::kRun:
		EnemyRun();
		break;
	case Behavior::kFree:
		Free();
		break;
	case Behavior::kDead:
		DeadMotion();
		break;
	case Behavior::kAttack:
		AttackMotion();
		break;
	case Behavior::kPreliminalyAction:
		PreliminalyAction();
		break;
	case Behavior::kLeaningBack:
		LeaningBack();
		break;
	}

	/*敵の移動*/
	Vector3 NowPos = transform_.translate;

	if (NowPos.x >= limitPos_.x or NowPos.x <= limitPos_.y) {
		if (NowPos.x > 0){
			transform_.translate.x = positionCoordinate_;
		}
		else {
			transform_.translate.x = -positionCoordinate_;
		}
	}
	if (NowPos.z >= limitPos_.x or NowPos.z <= limitPos_.y) {
		if (NowPos.z > 0) {
			transform_.translate.z = positionCoordinate_;
		}
		else {
			transform_.translate.z = -positionCoordinate_;
		}
	}

	Vector3 lockOnPos = target_->translate;
	Vector3 sub = lockOnPos - transform_.translate;

	playerLength_ = Vector3::Length(sub);

	Matrix4x4 resultRotateMat = Matrix::MakeRotateMatrix(transform_.rotate) * rotateMatrix_;

	/*エネミーのパーツ*/
	Vector3 parts_offset = { 0.0f, 1.5f, 0.0f };
	parts_offset = Matrix::TransformNormal(parts_offset, resultRotateMat);

	partsTransform_.translate = transform_.translate + parts_offset;

	//partsTransform_.rotate.x += 0.3f;

	if (enemyLife_ <= 0 && behavior_ != Behavior::kDead) {
		isNoLife_ = true;
		//PostEffect::GetInstance()->SetPostEffect(PostEffect::EffectType::GrayVignetting);
		behaviorRequest_ = Behavior::kDead;
	}
}



void Enemy::BehaviorRootInitialize(){
	rotateMatrix_ = Matrix::MakeIdentity4x4();
	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };
	farTime_ = 0;
	nearTime_ = 0;
	if (serialNumber_ % 2 == 0) {
		magnification_ = 1.0f;
	}
	else {
		magnification_ = -1.0f;
	}

	isNearAttack_ = false;
}

void Enemy::BehaviorDeadInitialize(){
	Vector3 deadMoveBase = { 0,0.02f,0.1f };
	deadMove_ = Matrix::TransformNormal(deadMoveBase, rotateMatrix_);
	deadMove_ = Vector3::Mutiply(Vector3::Normalize(deadMove_), 0.5f);
	deadMove_.y *= -1.00f;
	deadYAngle_ = Matrix::RotateAngleYFromMatrix(rotateMatrix_);
	transform_.rotate.Clear();
}


void Enemy::RootMotion(){
	frontVec_ = postureVec_;

	Vector3 move = { moveSpeed_ * magnification_ ,0,0 };

	move = Matrix::TransformNormal(move, rotateMatrix_);
	move.y = 0;
	/*敵の移動*/
	Vector3 NextPos = transform_.translate + move_;

	if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
		move_.x = 0;
	}
	if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
		move_.z = 0;
	}

	transform_.translate += move * timeScale_;

	
	if (target_){
		Vector3 lockOnPos = target_->translate;
		Vector3 sub = lockOnPos - transform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		postureVec_ = sub;

		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		rotateMatrix_ = Matrix::Multiply(rotateMatrix_, directionTodirection_);
	}
	//スクリーンに映っていたら距離に応じての行動を行う
	if (isOnScreen_) {

		if (playerLength_ > farPlayer_) {
			farTime_++;
		}
		else if (playerLength_ < nearPlayer_) {
			nearTime_++;
		}
		//指定の秒数近くにいた場合
		if (nearTime_ > lengthJudgment_) {
			//攻撃を準備する
			behaviorRequest_ = Behavior::kPreliminalyAction;			

		}
		//逆に離れていた場合
		else if (farTime_ > lengthJudgment_) {
			if (transform_.translate.x >= limitPos_.x - enemyLimitPos_ or transform_.translate.x <= limitPos_.y + enemyLimitPos_) {
				behaviorRequest_ = Behavior::kRun;
			}
			if (transform_.translate.z >= limitPos_.x - enemyLimitPos_ or transform_.translate.z <= limitPos_.y + enemyLimitPos_) {
				behaviorRequest_ = Behavior::kRun;
			}
			if (behaviorRequest_ == std::nullopt) {
				int i = RandomMaker::DistributionInt(0, 2);
				//分岐
				if (i == 0) {
					behaviorRequest_ = Behavior::kRun;
				}
				else {
					behaviorRequest_ = Behavior::kPreliminalyAction;
				}
			}
		}
	}
	
}

void Enemy::BehaviorBackInitialize(){
	dashTimer_ = 0;
}

void Enemy::BackStep(){
	Matrix4x4 newRotateMatrix_ = rotateMatrix_;
	move_ = { 0, 0, backSpeed_ };

	move_ = Matrix::TransformNormal(move_, newRotateMatrix_);

	/*敵の移動*/
	Vector3 NextPos = transform_.translate + move_;

	if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
		move_.x = 0;
	}
	if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
		move_.z = 0;
	}

	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 8;

	
	transform_.translate += move_ * timeScale_;
	

	//既定の時間経過で通常状態に戻る
	if (++dashTimer_ >= behaviorDashTime) {
		behaviorRequest_ = Behavior::kFree;
	}
}

void Enemy::BehaviorDashInitialize(){
	dashRotateMatrix_ = rotateMatrix_;
	dashTimer_ = 0;
}

void Enemy::Dash(){
	move_ = { 0, 0, dashSpeed_ };

	move_ = Matrix::TransformNormal(move_, dashRotateMatrix_);

	/*敵の移動*/
	Vector3 NextPos = transform_.translate + move_;

	if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
		move_.x = 0;
	}
	if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
		move_.z = 0;
	}

	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 30;	
	
	attackOBB_.size = bodyOBB_.size * collisionScale_;
	transform_.translate += move_ * timeScale_;
	

	//既定の時間経過で通常状態に戻る
	if (++dashTimer_ >= behaviorDashTime) {
		attackOBB_.size = { 0,0,0 };
		behaviorRequest_ = Behavior::kFree;
	}
}

void Enemy::BehaviorRunInitialize(){
	rotateMatrix_ = Matrix::MakeIdentity4x4();
	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };
	farTime_ = 0;
	nearTime_ = 0;
	magnification_ = 1.0f;
}

void Enemy::EnemyRun(){
	frontVec_ = postureVec_;

	Vector3 move = { 0,0,(magnification_ * dashSpeed_) / 6.0f };

	move = Matrix::TransformNormal(move, rotateMatrix_);
	move.y = 0;
	/*敵の移動*/
	Vector3 NextPos = transform_.translate + move_;

	if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
		move_.x = 0;
	}
	if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
		move_.z = 0;
	}

	transform_.translate += move * timeScale_;


	if (target_) {
		Vector3 lockOnPos = target_->translate;
		Vector3 sub = lockOnPos - transform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		postureVec_ = sub;

		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));

		rotateMatrix_ = Matrix::Multiply(rotateMatrix_, directionTodirection_);
	}
	if (playerLength_ < 15.0f) {
		int i = 0;
		
		if (i == 0) {
			behaviorRequest_ = Behavior::kFree;
		}
		else {
			behaviorRequest_ = Behavior::kPreliminalyAction;
		}
	}
}

void Enemy::BehaviorFreeInitialize(){
	freeTime_ = 0;
	enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };
}

void Enemy::Free(){
	if (++freeTime_ > freeTimeMax_) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Enemy::BehaviorPreliminalyActionInitialize(){
	enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };
}

void Enemy::PreliminalyAction(){
	
	behaviorRequest_ = Behavior::kAttack;
	
}

void Enemy::BehaviorLeaningBackInitialize(){
	enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };

	attackOBB_.size = { 0.0f,0.0f,0.0f };
	bodyObj_->SetTexture(enemyHitTexPath_);

	playerMat_ = *targetRotateMat_;

	switch (type_){
	case HitRecord::Left:
		transform_.rotate = hitEaseStartLeft_;
		knockBackEaseStart_ = hitEaseStartLeft_;
		break;
	case HitRecord::Right:
		transform_.rotate = hitEaseStartRight_;
		knockBackEaseStart_ = hitEaseStartRight_;
		break;
	case HitRecord::Center:
		transform_.rotate = hitEaseStartCenter_;
		knockBackEaseStart_ = hitEaseStartCenter_;
		break;
	case HitRecord::Guard:
		transform_.rotate = hitEaseStartCenter_;
		knockBackEaseStart_ = hitEaseStartCenter_;
		break;
	case HitRecord::Strong:
		transform_.rotate = hitEaseStartStrong_;
		knockBackEaseStart_ = hitEaseStartStrong_;
		downVector_ = { 0.0f };
		downVector_.y += jumpPower_;
		break;
	case HitRecord::Few:
		transform_.rotate = hitEaseStartCenter_;
		knockBackEaseStart_ = hitEaseStartCenter_;
		break;
	default:
		transform_.rotate = hitEaseStartLeft_;
		break;
	}
	
	rotateEaseT_ = 0.0f;
}

void Enemy::LeaningBack(){
	if (type_ == HitRecord::Center || type_ == HitRecord::Strong) {
		move_ = { 0, 0, backSpeed_ * strongHitBackSpeed_ };
	}
	else if (type_ == HitRecord::Few){
		move_ = { 0, 0, backSpeed_ * fewHitBackSpeed_ };
	}
	else {
		move_ = { 0, 0, backSpeed_ * hitBackSpeed_ };
	}
	move_ = Matrix::TransformNormal(move_, playerMat_);
	move_.y = 0;
	/*敵の移動*/
	Vector3 NextPos = transform_.translate + move_;

	if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
		move_.x = 0;
	}
	if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
		move_.z = 0;
	}

	
	downVector_.y += downSpeed_ * timeScale_;
		
	transform_.translate.y += downVector_.y * timeScale_;

	if (transform_.translate.y < 2.5f){
		transform_.translate.y = 2.5f;
	}

	transform_.translate += move_;

	rotateEaseT_ += addRotateEaseT_;

	if (rotateEaseT_ >= 1.0f) {
		rotateEaseT_ = 1.0f;
	}

	transform_.rotate.x = ease_.Easing(Ease::EaseName::EaseInBack, knockBackEaseStart_.x, 0.0f, rotateEaseT_);
	transform_.rotate.z = ease_.Easing(Ease::EaseName::EaseInBack, knockBackEaseStart_.z, 0.0f, rotateEaseT_);

	if (rotateEaseT_ >= 1.0f && transform_.translate.y == 2.5f) {
		bodyObj_->SetTexture(enemyTexPath_);
		behaviorRequest_ = Behavior::kRoot;
	}

}

void Enemy::DeadMotion(){
	transform_.translate -= deadMove_ * timeScale_;
	transform_.rotate.x += 0.3f*timeScale_;	
	Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(transform_.rotate) * rotateMatrix_;
	Vector3 parts_offset = { 0.0f, 2.7f, 0.0f };
	//Vector3 R_parts_offset = { -7.0f, 7.0f, 0.0f };
	parts_offset = Matrix::TransformNormal(parts_offset, newRotateMatrix);

	partsTransform_.translate = transform_.translate + parts_offset;
	
	//partsTransform_.rotate.x += 0.3f;
	
	if (threshold_ < 1.0f) {
		threshold_ += 0.0075f * timeScale_;
	}
	else {
		isDead_ = true;
	}

	
}

void Enemy::BehaviorAttackInitialize() {
	enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };
	
	ATBehaviorRequest_ = AttackBehavior::kTackle;
	

}

void Enemy::AttackMotion() {
	if (ATBehaviorRequest_) {
		// 振る舞いを変更する
		ATBehavior_ = ATBehaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (ATBehavior_) {
		case AttackBehavior::kTriple:
			AttackBehaviorTripleInitialize();
			break;
		case AttackBehavior::kTackle:
			AttackBehaviorTackleInitialize();
			break;
		case AttackBehavior::kRotateAttack:
			AttackBehaviorRotateAttackInitialize();
			break;
		case AttackBehavior::kXAttack:
			AttackBehaviorDoubleSlashInitialize();
			break;
		case AttackBehavior::kNone:
			BehaviorRootInitialize();
			break;

	
		}
	}
	// 振る舞いリクエストをリセット
	ATBehaviorRequest_ = std::nullopt;

	switch (ATBehavior_) {
	case AttackBehavior::kTriple:
		TripleAttack();
		break;
	case AttackBehavior::kTackle:
		Tackle();
		break;
	case AttackBehavior::kRotateAttack:
		RotateAttack();
		break;
	case AttackBehavior::kXAttack:
		DoubleSlash();
		break;
	case AttackBehavior::kNone:
		behaviorRequest_ = Behavior::kFree;
		break;
	}
}

void Enemy::AttackBehaviorTripleInitialize(){
	posContainer_.fill(Vector3());

	attackDistance_ = distanceTime_;

	isAttackEnd_ = true;

	isMaxContext_ = false;

	attackCount_ = 0;

}

void Enemy::TripleAttack(){
	if (isAttackEnd_ and !isMaxContext_) {
		if (target_) {
			Vector3 lockOnPos = target_->translate;

			posContainer_[attackCount_] = lockOnPos;

			isMaxContext_ = std::all_of(posContainer_.begin(), posContainer_.end(), [](Vector3 i) { return i.isNotIdentity(); });

			isAttackEnd_ = false;
		}
	}

	frontVec_ = postureVec_;

	Vector3 move = { 0,0,moveSpeed_ * magnification_ * dashSpeed_ * 5.0f };

	move = Matrix::TransformNormal(move, rotateMatrix_);
	move.y = 0;
	
	Vector3 lockOnPos = posContainer_[attackCount_];
	Vector3 sub = lockOnPos - transform_.translate;
	sub.y = 0;
	float PELength = Vector3::Length(sub);
	sub = Vector3::Normalize(sub);
	postureVec_ = sub;

	Matrix4x4 directionTodirection_;
	directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));

	rotateMatrix_ = Matrix::Multiply(rotateMatrix_, directionTodirection_);
	if (PELength < 15.0f) {
		move = { 0 };
		isNearAttack_ = true;
		if (attackDistance_ == 0) {
			attackOBB_.size = { 0,0,0 };
			attackCount_++;
			isAttackEnd_ = true;
			attackDistance_ = distanceTime_;
		}
		else {
			attackOBB_.size = bodyOBB_.size * 2.4f;
			attackDistance_ -= 1;
		}

	}
	else {
		/*敵の移動*/
		Vector3 NextPos = transform_.translate + move_;

		if (NextPos.x >= 95.0f or NextPos.x <= -95.0f) {
			move_.x = 0;
		}
		if (NextPos.z >= 95.0f or NextPos.z <= -95.0f) {
			move_.z = 0;
		}
		transform_.translate += move * timeScale_;
	}

	if (isMaxContext_ and isAttackEnd_){
		behaviorRequest_ = Behavior::kFree;
	}

}

void Enemy::AttackBehaviorTackleInitialize(){
	directionTime_ = 90;

	attackTransitionTime_ = 30;

	dashTimer_ = 0;
}

void Enemy::Tackle(){
	frontVec_ = postureVec_;
	if (target_) {
		Vector3 lockOnPos = target_->translate;
		Vector3 sub = lockOnPos - transform_.translate;
		sub.y = 0;
		sub = Vector3::Normalize(sub);
		postureVec_ = sub;

		Matrix4x4 directionTodirection_;
		directionTodirection_.DirectionToDirection(Vector3::Normalize(frontVec_), Vector3::Normalize(postureVec_));
		rotateMatrix_ = Matrix::Multiply(rotateMatrix_, directionTodirection_);

	}

	if (enemyColor_.y <= 0.2f) {
		if (attackTransitionTime_ <= 0) {
			move_ = { 0, 0, dashSpeed_ };

			move_ = Matrix::TransformNormal(move_, dashRotateMatrix_);

			/*敵の移動*/
			Vector3 NextPos = transform_.translate + move_;

			if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
				move_.x = 0;
			}
			if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
				move_.z = 0;
			}

			//ダッシュの時間<frame>
			const uint32_t behaviorDashTime = 30;

			transform_.translate += move_* timeScale_;
			attackOBB_.size = bodyOBB_.size * collisionScale_;
			
			//既定の時間経過で通常状態に戻る
			if (++dashTimer_ >= behaviorDashTime) {
				attackOBB_.size = { 0,0,0 };
				behaviorRequest_ = Behavior::kFree;
			}
		}
		else {
			attackTransitionTime_--;
		}		
	}
	else {
		
		dashRotateMatrix_ = rotateMatrix_;

		enemyColor_.y -= 0.02f * timeScale_;
		enemyColor_.z = enemyColor_.y;

		//directionTime_--;
	}
	
}

void Enemy::AttackBehaviorRotateAttackInitialize(){
	posContainer_.fill(Vector3());

	attackBasePos_ = Vector3();

	attackDistance_ = distanceTime_;

	isAttackEnd_ = true;

	isMaxContext_ = false;

	attackCount_ = 0;

	easeT_ = 0;
}

void Enemy::RotateAttack(){
	if (isAttackEnd_ and !isMaxContext_) {
		if (target_) {
			Vector3 lockOnPos = target_->translate;

			Vector3 sub = lockOnPos - transform_.translate;
			sub.y = 0;
			sub = Vector3::Normalize(sub);

			sub *= attackLength_;

			posContainer_[attackCount_] = transform_.translate + sub;

			isMaxContext_ = std::all_of(posContainer_.begin(), posContainer_.end(), [](Vector3 i) { return i.isNotIdentity(); });

			attackBasePos_ = transform_.translate;


			isAttackEnd_ = false;
		}
	}

	frontVec_ = postureVec_;

	Vector3 lockOnPos = posContainer_[attackCount_];
	Vector3 sub = lockOnPos - attackBasePos_;
	sub.y = 0;
	sub = Vector3::Normalize(sub);
	postureVec_ = sub;

	Matrix4x4 rotateMat;
	rotateMat.MakeRotateMatrixY({ 0.0f,attackRotate_,0.0f });

	rotateMatrix_ = Matrix::Multiply(rotateMatrix_, rotateMat);

	transform_.translate = ease_.Easing(Ease::EaseName::EaseOutQuart, attackBasePos_, posContainer_[attackCount_], easeT_);

	attackRotate_ = ease_.Easing(Ease::EaseName::EaseOutQuart, 0.0f, (3.14f * 4.0f), easeT_);

	if (easeT_ < 1.0f) {		
		easeT_ += (1.0f / 60.0f) * timeScale_;
		attackOBB_.size = bodyOBB_.size * 2.4f;
	}
	else {
		attackOBB_.size = { 0.0f,0.0f,0.0f };
		attackCount_++;
		isAttackEnd_ = true;
		attackDistance_ = distanceTime_;
		easeT_ = 0;
	}
	

	if (isMaxContext_ and isAttackEnd_) {
		behaviorRequest_ = Behavior::kBack;
	}
}

void Enemy::AttackBehaviorDoubleSlashInitialize(){
	Vector3 lockOnPos = target_->translate;

	Vector3 sub = lockOnPos - transform_.translate;
	sub.y = 0;
	sub = Vector3::Normalize(sub);

	bulletSpeed_ = 1.0f;

	sub *= bulletSpeed_;

	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();

	EulerTransform trans{};

	float y = rotateMatrix_.RotateAngleYFromMatrix();	

	trans = transform_;

	trans.rotate.y = y;

	//trans.rotate.z = (3.14f / 4.0f);

	trans.rotate.x = 0;

	trans.scale.y *= 5.0f;
	newBullet->Initialize(trans, sub);

	bullets_.emplace_back(std::move(newBullet));

	
}

void Enemy::DoubleSlash(){
	behaviorRequest_ = Behavior::kFree;
}
