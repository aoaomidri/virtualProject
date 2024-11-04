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

	enemyLifeMax_ = adjustment_item->GetIntValue(groupName, "EnemyLifeMax");

	enemyLife_ = enemyLifeMax_;

	auto levelLoader = LevelLoader::GetInstance(); 

	bodyObj_ = std::make_unique<Object3D>();
	bodyObj_->Initialize("Enemy");

	partsObj_ = std::make_unique<Object3D>();
	partsObj_->Initialize("EnemyParts");

	//collisionObj_ = std::make_unique<Object3D>();
	//collisionObj_->Initialize("box");

	/*for (int i = 0; i < 20; i++){
		particleObj_[i] = std::make_unique<Object3D>();
		particleObj_[i]->Initialize("box");
	}*/

	//boxObj_ = LevelLoader::GetInstance()->GetLevelObject("ENCube");
	
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
	emitter_.transform.scale.x /= 6.0f;

	particle_ = std::make_unique<ParticleBase>();
	particle_->Initialize(emitter_, false);
	particle_->SetIsDraw(false);
	particle_->SetIsBillborad(true);
	particle_->SetIsAlignedToMovement(true);
	particle_->SetLifeTime(0.3f);
	particle_->SetVelocityRange(Vector2(-50.0f, 50.0f));

	isDead_ = false;
	isNoLife_ = false;
	isParticle_ = true;

	threshold_ = 0.0f;

	freeTime_ = 0;

	OBB_ = levelLoader->GetLevelObjectOBB("Enemy");

	collisionTransform_.translate = OBB_.center;
	collisionTransform_.scale = OBB_.size;
	bodyOBB_.center = transform_.translate;
	bodyOBB_.size = { transform_.scale.x + 3.0f,transform_.scale.y + 2.0f,transform_.scale.z + 3.0f };

	Matrix4x4 enemyRotateMatrix = Matrix::MakeRotateMatrix(transform_.rotate);
	SetOridentatios(OBB_, enemyRotateMatrix);
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
		magnification = 1.0f;
	}
	else {
		magnification = -1.0f;
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

	/*for (int i = 0; i < particleNum_; i++) {
		particleTransform_[i].translate += particleVec_[i];
	}*/
	collisionTransform_ = transform_;

	scaleMatrix_ = Matrix::MakeScaleMatrix(transform_.scale);
	transformMatrix_ = Matrix::MakeTranslateMatrix(transform_.translate);

	Matrix4x4 resultRotateMat = rotateMatrix_ * Matrix::MakeRotateMatrix(transform_.rotate);

	matrix_ = Matrix::MakeAffineMatrix(scaleMatrix_, resultRotateMat, transformMatrix_);

	resultRotateMat = Matrix::MakeRotateMatrix(partsTransform_.rotate) * Matrix::MakeRotateMatrix(transform_.rotate);

	partsMatrix_ = Matrix::MakeAffineMatrix(partsTransform_.scale, resultRotateMat, partsTransform_.translate);
	
	/*for (int i = 0; i < particleNum_; i++) {
		particleMatrix_[i] = Matrix::MakeAffineMatrix(particleTransform_[i].scale, particleTransform_[i].rotate, particleTransform_[i].translate);
		
	}*/

	for (auto it = bullets_.begin(); it != bullets_.end(); ++it) {
		(*it)->Update();
	}

	OBB_.center = transform_.translate;
	bodyOBB_.center = transform_.translate;
	if (isNearAttack_){
		Vector3 attackOffset = { 0.0f, 0.0f, 4.0f };
		attackOffset = Matrix::TransformNormal(attackOffset, rotateMatrix_);

		attackOBB_.center = OBB_.center + attackOffset;
	}
	else{
		attackOBB_.center = OBB_.center;
	}

	
	SetOridentatios(OBB_, rotateMatrix_);
	SetOridentatios(bodyOBB_, rotateMatrix_);
	SetOridentatios(attackOBB_, rotateMatrix_);

	
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
	partsObj_->Update(viewProjection);
	partsObj_->Draw();

	/*if (isParticle_){
		for (int i = 0; i < 10; i++) {
			particleObj_[i]->SetMatrix(particleMatrix_[i]);
			particleObj_[i]->Update(viewProjection);
			particleObj_[i]->Draw();
		}
		for (int i = 10; i < 20; i++) {
			particleObj_[i]->SetMatrix(particleMatrix_[i]);
			particleObj_[i]->Update(viewProjection);
			particleObj_[i]->Draw();
		}
	}*/
}

void Enemy::TexDraw(const Matrix4x4& viewProjection){
	shadow_->Draw(viewProjection);
}

void Enemy::ParticleDraw(const ViewProjection& viewProjection, const Vector3& color){

	particle_->SetOneColor(color);
	particle_->Update(transform_, viewProjection);
	particle_->Draw();
}

void Enemy::DrawImgui() {
#ifdef _DEBUG
	ImGui::Begin("敵の変数");
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

void Enemy::onFlootCollision(OBB obb){

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

	/*for (int i = 0; i < 20; i++) {
		particleTransform_[i] = {
			{0.1f,0.1f,0.1f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f}
		};
	}


	for (int i = 0; i < 20; i++) {
		particleVec_[i] = { 0.0f,0.0f,0.0f };
	}*/

	isDead_ = false;
	isNoLife_ = false;
	isParticle_ = true;
	enemyLife_ = enemyLifeMax_;
	threshold_ = 0.0f;

	freeTime_ = 0;

	OBB_ = levelLoader->GetLevelObjectOBB("Enemy");

	collisionTransform_.translate = OBB_.center;
	collisionTransform_.scale = OBB_.size;
	bodyOBB_.center = transform_.translate;
	bodyOBB_.size = { transform_.scale.x + 3.0f,transform_.scale.y + 2.0f,transform_.scale.z + 3.0f };

	Matrix4x4 enemyRotateMatrix = Matrix::MakeRotateMatrix(transform_.rotate);
	SetOridentatios(OBB_, enemyRotateMatrix);
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
	ParticleMove();
	particle_->AddParticle(emitter_);
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

	Vector3 lockOnPos = target_->translate;
	Vector3 sub = lockOnPos - transform_.translate;

	playerLength_ = Vector3::Length(sub);

	Matrix4x4 resultRotateMat = rotateMatrix_ * Matrix::MakeRotateMatrix(transform_.rotate);

	/*エネミーのパーツ*/
	Vector3 parts_offset = { 0.0f, 3.0f, 0.0f };
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
		magnification = 1.0f;
	}
	else {
		magnification = -1.0f;
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
	transform_.rotate.y = deadYAngle_;
}


void Enemy::RootMotion(){
	frontVec_ = postureVec_;

	Vector3 move = { moveSpeed_ * magnification ,0,0 };

	move = Matrix::TransformNormal(move, rotateMatrix_);
	move.y = 0;
	/*敵の移動*/
	Vector3 NextPos = transform_.translate + move_;

	if (NextPos.x >= 95.0f or NextPos.x <= -95.0f) {
		move_.x = 0;
	}
	if (NextPos.z >= 95.0f or NextPos.z <= -95.0f) {
		move_.z = 0;
	}

	transform_.translate += move;

	
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

	if (playerLength_ > farPlayer_) {
		farTime_++;
	}
	else if (playerLength_ < nearPlayer_) {
		nearTime_++;
	}
	if (nearTime_ > lengthJudgment_) {
		int i = RandomMaker::DistributionInt(0, 2);
		/*if (i == 0) {
			behaviorRequest_ = Behavior::kBack;
		}*/
		

	}
	else if (farTime_ > lengthJudgment_) {
		int i = 0;
		if (i == 0) {
			behaviorRequest_ = Behavior::kRun;
		}
		else {
			behaviorRequest_ = Behavior::kPreliminalyAction;
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

	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 8;

	
	transform_.translate += move_;
	

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

	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 30;

	
	
	attackOBB_.size = OBB_.size * 2.8f;
	transform_.translate += move_;
	

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
	magnification = 1.0f;
}

void Enemy::EnemyRun(){
	frontVec_ = postureVec_;

	Vector3 move = { 0,0,(magnification * dashSpeed_) / 6.0f };

	move = Matrix::TransformNormal(move, rotateMatrix_);
	move.y = 0;
	/*敵の移動*/
	Vector3 NextPos = transform_.translate + move_;

	if (NextPos.x >= 95.0f or NextPos.x <= -95.0f) {
		move_.x = 0;
	}
	if (NextPos.z >= 95.0f or NextPos.z <= -95.0f) {
		move_.z = 0;
	}

	transform_.translate += move;


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
	enemyColor_.y -= 0.02f;
	enemyColor_.z = enemyColor_.y;

	if (enemyColor_.y <= 0.2f){
		behaviorRequest_ = Behavior::kAttack;
	}
}

void Enemy::BehaviorLeaningBackInitialize(){
	
	transform_.rotate = hitEaseStart_;
	rotateEaseT_ = 0.0f;
}

void Enemy::LeaningBack(){
	move_ = { 0, 0, backSpeed_ * hitBackSpeed_ };

	move_ = Matrix::TransformNormal(move_, *targetRotateMat_);
	move_.y = 0;
	transform_.translate += move_;

	rotateEaseT_ += addRotateEaseT_;

	if (rotateEaseT_ >= 1.0f) {
		rotateEaseT_ = 1.0f;
	}

	transform_.rotate.x = ease_.Easing(Ease::EaseName::EaseInBack, hitEaseStart_.x, 0.0f, rotateEaseT_);
	transform_.rotate.z = ease_.Easing(Ease::EaseName::EaseInBack, hitEaseStart_.z, 0.0f, rotateEaseT_);

	if (rotateEaseT_ >= 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
	}

}

void Enemy::DeadMotion(){
	transform_.translate -= deadMove_;
	transform_.rotate.x += 0.3f;	
	Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(transform_.rotate);
	Vector3 parts_offset = { 0.0f, 2.7f, 0.0f };
	//Vector3 R_parts_offset = { -7.0f, 7.0f, 0.0f };
	parts_offset = Matrix::TransformNormal(parts_offset, newRotateMatrix);

	partsTransform_.translate = transform_.translate + parts_offset;
	
	//partsTransform_.rotate.x += 0.3f;
	
	if (threshold_ < 1.0f) {
		threshold_ += 0.0075f;
	}
	else {
		isDead_ = true;
	}

	
}

void Enemy::BehaviorAttackInitialize() {
	enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };

	int i = RandomMaker::DistributionInt(0, 3);
	//一旦雑魚想定なので突進攻撃だけにする

	/*if (i == 0) {
		ATBehaviorRequest_ = AttackBehavior::kXAttack;
	}

	else if (i == 1) {
		ATBehaviorRequest_ = AttackBehavior::kRotateAttack;
	}
	else if (i == 2) {
		ATBehaviorRequest_ = AttackBehavior::kTriple;
	}
	else */{
		ATBehaviorRequest_ = AttackBehavior::kTackle;
	}

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

	Vector3 move = { 0,0,moveSpeed_ * magnification * dashSpeed_ * 5.0f };

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
			attackOBB_.size = OBB_.size * 2.4f;
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
		transform_.translate += move;
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

	if (directionTime_ <= 0) {
		if (attackTransitionTime_ <= 0) {
			move_ = { 0, 0, dashSpeed_ };

			move_ = Matrix::TransformNormal(move_, dashRotateMatrix_);

			//ダッシュの時間<frame>
			const uint32_t behaviorDashTime = 30;



			attackOBB_.size = OBB_.size * 2.4f;
			transform_.translate += move_;


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
		directionTime_--;
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
		easeT_ += (1.0f / 60.0f);
		attackOBB_.size = OBB_.size * 2.4f;
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
