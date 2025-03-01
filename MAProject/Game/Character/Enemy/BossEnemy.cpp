#include "BossEnemy.h"
#include"ImGuiManager.h"
#include"LevelLoader/LevelLoader.h"

void BossEnemy::ApplyGlobalVariables(){
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

void BossEnemy::Initialize(const Vector3& position){
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

	bodyObj_ = std::make_unique<Object3D>();
	bodyObj_->Initialize("Enemy");
	bodyObj_->SetIsLighting(true);
	bodyObj_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());

	partsObj_ = std::make_unique<Object3D>();
	partsObj_->Initialize("EnemyParts");
	partsObj_->SetIsLighting(true);
	partsObj_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());

	partsObjRight_ = std::make_unique<Object3D>();
	partsObjRight_->Initialize("EnemyParts");
	partsObjRight_->SetIsLighting(true);
	partsObjRight_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());

	partsObjLeft_ = std::make_unique<Object3D>();
	partsObjLeft_->Initialize("EnemyParts");
	partsObjLeft_->SetIsLighting(true);
	partsObjLeft_->SetDirectionalLight(DirectionalLight::GetInstance()->GetLightData());

	collisionObj_ = std::make_unique<Object3D>();
	collisionObj_->Initialize("box");

	LevelLoader* levelLoader = LevelLoader::GetInstance();
	transform_ = {
		levelLoader->GetLevelObjectTransform("Enemy").scale * bossScale_,
		{0.0f,0.0f,0.0f},
		position
	};
	collisionTransform_ = transform_;

	partsTransform_.scale = kPartsScaleBase_ * bossScale_;
	partsTransform_.rotate.z = kPartsRotateZ_;
	partsRightTransform_.scale = kPartsScaleBase_ * bossScale_;
	partsLeftTransform_.scale = kPartsScaleBase_ * bossScale_;

	emitter_.count = 10;
	emitter_.transform = {
		levelLoader->GetLevelObjectTransform("Enemy").scale* bossScale_,
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
	collisionTransform_.scale = attackOBB_.size * bossScale_;
	bodyOBB_.center = transform_.translate;
	bodyOBB_.size = (transform_.scale + addOBBSize_) * bossScale_;
	parts_offset_Base_ *= bossScale_;


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
	shadow_->scale_ = { 1.5f * bossScale_,1.5f * bossScale_ };
	shadow_->anchorPoint_ = { 0.5f,0.5f };
	shadow_->color_.w = 0.5f;

	if (serialNumber_ % 2 == 0) {
		magnification_ = 1.0f;
	}
	else {
		magnification_ = -1.0f;
	}
	

}

void BossEnemy::Update(){
	ApplyGlobalVariables();
	
	ObjStateSet();
	//行動の更新
	MotionUpdate();
	//影の座標更新
	shadow_->position_ = transform_.translate;
	shadow_->position_.y = kShadowHeightBase_;

	//行列やobbの更新
	collisionTransform_ = transform_;
	//体の部分の更新
	scaleMatrix_ = Matrix::MakeScaleMatrix(transform_.scale);
	transformMatrix_ = Matrix::MakeTranslateMatrix(transform_.translate);
	Matrix4x4 resultRotateMat = Matrix::MakeRotateMatrix(transform_.rotate) * rotateMatrix_;
	matrix_ = Matrix::MakeAffineMatrix(scaleMatrix_, resultRotateMat, transformMatrix_);
	//パーツ部分の更新
	
	Matrix4x4 resultPartsRotateMat = Matrix::MakeRotateMatrix(partsTransform_.rotate) * resultRotateMat;
	partsMatrix_ = Matrix::MakeAffineMatrix(partsTransform_.scale, resultPartsRotateMat, partsTransform_.translate);
	resultPartsRotateMat = Matrix::MakeRotateMatrix(partsRightTransform_.rotate) * resultRotateMat;
	partsRightMatrix_ = Matrix::MakeAffineMatrix(partsRightTransform_.scale, resultPartsRotateMat, partsRightTransform_.translate);
	resultPartsRotateMat = Matrix::MakeRotateMatrix(partsLeftTransform_.rotate) * resultRotateMat;
	partsLeftMatrix_ = Matrix::MakeAffineMatrix(partsLeftTransform_ .scale, resultPartsRotateMat, partsLeftTransform_.translate);

	//obb更新
	bodyOBB_.center = transform_.translate;	
	attackOBB_.center = bodyOBB_.center;	
	SetOridentatios(bodyOBB_, rotateMatrix_);
	SetOridentatios(attackOBB_, rotateMatrix_);
	//当たり描画用の更新
	collisionMatrix_ = Matrix::MakeAffineMatrix(attackOBB_.size, rotateMatrix_, attackOBB_.center);
}

void BossEnemy::Draw(const ViewProjection& viewProjection){
	if (isDead_) {
		return;
	}
	bodyObj_->SetMatrix(matrix_);
	bodyObj_->Update(viewProjection);
	bodyObj_->Draw();
#ifdef _DEBUG
	
#endif // _DEBUG
	partsObj_->SetMatrix(partsMatrix_);
	partsObj_->SetTimeScale(timeScale_);
	partsObj_->Update(viewProjection);
	partsObj_->Draw();	

	partsObjLeft_->SetMatrix(partsLeftMatrix_);
	partsObjLeft_->SetTimeScale(timeScale_);
	partsObjLeft_->Update(viewProjection);
	partsObjLeft_->Draw();

	partsObjRight_->SetMatrix(partsRightMatrix_);
	partsObjRight_->SetTimeScale(timeScale_);
	partsObjRight_->Update(viewProjection);
	partsObjRight_->Draw();
}

void BossEnemy::TexDraw(const Matrix4x4& viewProjection){
	shadow_->Draw(viewProjection);
}

void BossEnemy::ParticleDraw(const ViewProjection& viewProjection, const Vector3& color){
	EulerTransform patTransform = transform_;
	patTransform.translate.y += 1.0f;
	particle_->SetOneColor(color);
	particle_->Update(patTransform, viewProjection);
	particle_->Draw();
}

void BossEnemy::DrawImgui() {
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

void BossEnemy::OnCollision(){
	particle_->SetIsDraw(true);
	enemyLife_ -= damege_;
	particle_->AddParticle(emitter_);
	behaviorRequest_ = Behavior::kLeaningBack;
}

void BossEnemy::OnCollisionStrong(){
	particle_->SetIsDraw(true);
	enemyLife_ -= strongDamege_;
	particle_->AddParticle(emitter_);
	behaviorRequest_ = Behavior::kLeaningBack;
}

void BossEnemy::OnCollisionGuard(){
	behaviorRequest_ = Behavior::kLeaningBack;
}

const Vector3 BossEnemy::GetCenterPos()const{
	const Vector3 offset = { 0.0f,5.0f,0.0f };
	//ワールドに変換
	Vector3 world = Matrix::TransformVec(offset, matrix_);
	return world;
}

void BossEnemy::ObjStateSet(){
	//オブジェの内部の値をセット
	bodyObj_->SetDissolve(threshold_);
	partsObj_->SetDissolve(threshold_);
	partsObjLeft_->SetDissolve(threshold_);
	partsObjRight_->SetDissolve(threshold_);

	bodyObj_->SetColor(enemyColor_);
	partsObj_->SetColor(enemyColor_);
	partsObjLeft_->SetColor(enemyColor_);
	partsObjRight_->SetColor(enemyColor_);
}

void BossEnemy::PartsCalculation(){
	Matrix4x4 resultRotateMat = Matrix::MakeRotateMatrix(transform_.rotate) * rotateMatrix_;
	/*エネミーの頭パーツ*/
	parts_offset_ = Matrix::TransformNormal(parts_offset_Base_, resultRotateMat);
	partsTransform_.translate = transform_.translate + parts_offset_;

	resultRotateMat = Matrix::MakeRotateMatrix(transform_.rotate) * rotateMatrix_;
	/*エネミーの右側パーツ*/
	parts_offsetRight_ = Matrix::TransformNormal(parts_offset_BaseRight_, resultRotateMat);
	partsRightTransform_.translate = transform_.translate + parts_offsetRight_;

	resultRotateMat = Matrix::MakeRotateMatrix(transform_.rotate) * rotateMatrix_;
	/*エネミーの左側パーツ*/
	parts_offsetLeft_ = Matrix::TransformNormal(parts_offset_BaseLeft_, resultRotateMat);
	partsLeftTransform_.translate = transform_.translate + parts_offsetLeft_;
}

void BossEnemy::ParticleMove(){
}

void BossEnemy::MotionUpdate(){
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
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
		//RootMotion();
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
	//移動の制限
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
	//プレイヤーに顔を向けるように
	Vector3 lockOnPos = target_->translate;
	Vector3 sub = lockOnPos - transform_.translate;

	playerLength_ = Vector3::Length(sub);

	PartsCalculation();

	if (enemyLife_ <= 0 && behavior_ != Behavior::kDead) {
		isNoLife_ = true;
		behaviorRequest_ = Behavior::kDead;
	}
}

void BossEnemy::BehaviorRootInitialize(){
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
	if (isAttack_ == true) {
		isAttack_ = false;
	}
	isNearAttack_ = false;
}

void BossEnemy::BehaviorDeadInitialize(){	
	deadMove_ = Matrix::TransformNormal(deadMoveBase_, rotateMatrix_);
	deadMove_ = Vector3::Mutiply(Vector3::Normalize(deadMove_), deadMoveSpeed_);
	deadMove_.y *= -1.00f;
	deadYAngle_ = Matrix::RotateAngleYFromMatrix(rotateMatrix_);
	transform_.rotate.Clear();
	if (isAttack_ == true){
		isAttack_ = false;
	}
}


void BossEnemy::RootMotion(){
	//方向ベクトルの更新
	frontVec_ = postureVec_;
	Vector3 move = { moveSpeed_ * magnification_ ,0,0 };
	move = Matrix::TransformNormal(move, rotateMatrix_);
	move.y = 0;
	/*敵の移動*/
	Vector3 NextPos = transform_.translate + move_;
	//移動の制限
	if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
		move_.x = 0;
	}
	if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
		move_.z = 0;
	}

	transform_.translate += move * timeScale_;

	//ターゲットに顔を向ける
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
			behaviorRequest_ = Behavior::kRun;			
		}
		//逆に離れていた場合
		else if (farTime_ > lengthJudgmentFar_) {
			if (transform_.translate.x >= limitPos_.x - enemyLimitPos_ or transform_.translate.x <= limitPos_.y + enemyLimitPos_) {
				behaviorRequest_ = Behavior::kRun;
			}
			if (transform_.translate.z >= limitPos_.x - enemyLimitPos_ or transform_.translate.z <= limitPos_.y + enemyLimitPos_) {
				behaviorRequest_ = Behavior::kRun;
			}
			if (behaviorRequest_ == std::nullopt) {				
				behaviorRequest_ = Behavior::kRun;			
			}
		}
	}
	
}


void BossEnemy::BehaviorRunInitialize(){
	rotateMatrix_ = Matrix::MakeIdentity4x4();
	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };
	farTime_ = 0;
	nearTime_ = 0;
	magnification_ = 1.0f;
}

void BossEnemy::EnemyRun(){
	frontVec_ = postureVec_;

	Vector3 move = { 0,0,(magnification_ * dashSpeed_) / runMagnification_ };

	move = Matrix::TransformNormal(move, rotateMatrix_);
	move.y = 0;
	/*敵の移動*/
	Vector3 NextPos = transform_.translate + move_;
	//移動の制限
	if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
		move_.x = 0;
	}
	if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
		move_.z = 0;
	}

	transform_.translate += move * timeScale_;

	//ターゲットに顔を向ける
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
	//一定距離近づいたら別の行動
	if (playerLength_ < nearPlayer_) {
		int i = 0;
		
		if (i == 0) {
			behaviorRequest_ = Behavior::kFree;
		}
		else {
			behaviorRequest_ = Behavior::kPreliminalyAction;
		}
	}
}

void BossEnemy::BehaviorFreeInitialize(){
	
	freeTime_ = 0;
	enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };
}

void BossEnemy::Free(){
	if (++freeTime_ > freeTimeMax_) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void BossEnemy::BehaviorPreliminalyActionInitialize(){
	enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };
}

void BossEnemy::PreliminalyAction(){
	
	behaviorRequest_ = Behavior::kAttack;
	
}

void BossEnemy::BehaviorLeaningBackInitialize(){
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
	case HitRecord::Floating:
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

void BossEnemy::LeaningBack(){
	//ヒットバックのタイプによって距離を分岐
	if (type_ == HitRecord::Center || type_ == HitRecord::Strong) {
		move_ = { 0, 0, backSpeed_ * strongHitBackSpeed_ };
	}
	else if (type_ == HitRecord::Few){
		move_ = { 0, 0, backSpeed_ * fewHitBackSpeed_ };
	}
	else if (type_ == HitRecord::Floating) {
		move_ = { 0, 0, 0 };
	}
	else {
		move_ = { 0, 0, backSpeed_ * hitBackSpeed_ };
	}
	move_ = Matrix::TransformNormal(move_, playerMat_);
	move_.y = 0;
	/*敵の移動*/
	Vector3 NextPos = transform_.translate + move_;
	//移動の制限
	if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
		move_.x = 0;
	}
	if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
		move_.z = 0;
	}

	//重力系の処理
	downVector_.y += downSpeed_ * timeScale_;
		
	transform_.translate.y += downVector_.y * timeScale_;

	if (transform_.translate.y < kTranslateHeight_){
		transform_.translate.y = kTranslateHeight_;
	}

	transform_.translate += move_;

	rotateEaseT_ += addRotateEaseT_;

	if (rotateEaseT_ >= 1.0f) {
		rotateEaseT_ = 1.0f;
	}
	//敵自体の回転をイージング
	transform_.rotate.x = ease_.Easing(Ease::EaseName::EaseInBack, knockBackEaseStart_.x, 0.0f, rotateEaseT_);
	transform_.rotate.z = ease_.Easing(Ease::EaseName::EaseInBack, knockBackEaseStart_.z, 0.0f, rotateEaseT_);
	//元の体勢かつ地面についていたら次の行動へ
	if (rotateEaseT_ >= 1.0f && transform_.translate.y == kTranslateHeight_) {
		bodyObj_->SetTexture(enemyTexPath_);
		behaviorRequest_ = Behavior::kRoot;
	}

}

void BossEnemy::DeadMotion(){

	//回転しながら吹っ飛ぶ動き
	transform_.translate -= deadMove_ * timeScale_;
	transform_.rotate.x += deadRotateSpeed_ * timeScale_;
	Matrix4x4 newRotateMatrix = Matrix::MakeRotateMatrix(transform_.rotate) * rotateMatrix_;
	
	parts_offset_ = Matrix::TransformNormal(parts_offset_Base_, newRotateMatrix);

	partsTransform_.translate = transform_.translate + parts_offset_;
	
	//partsTransform_.rotate.x += 0.3f;
	//ディゾルブを掛けて消滅完全に消えたら死亡フラグを立てる
	if (threshold_ < 1.0f) {
		threshold_ += thresholdSpeed_ * timeScale_;
	}
	else {
		isDead_ = true;
	}

	
}

void BossEnemy::BehaviorAttackInitialize() {
	enemyColor_ = { 1.0f,1.0f,1.0f,1.0f };
	
	ATBehaviorRequest_ = AttackBehavior::kTackle;
	

}

void BossEnemy::AttackMotion() {
	if (ATBehaviorRequest_) {
		// 振る舞いを変更する
		ATBehavior_ = ATBehaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (ATBehavior_) {
		case AttackBehavior::kTackle:
			AttackBehaviorTackleInitialize();
			break;
		case AttackBehavior::kNone:
			BehaviorRootInitialize();
			break;	
		}
	}
	// 振る舞いリクエストをリセット
	ATBehaviorRequest_ = std::nullopt;

	switch (ATBehavior_) {
	case AttackBehavior::kTackle:
		Tackle();
		break;
	case AttackBehavior::kNone:
		behaviorRequest_ = Behavior::kFree;
		break;
	}
}

void BossEnemy::AttackBehaviorTackleInitialize(){
	directionTime_ = directionTimeBase_;

	attackTransitionTime_ = attackTransitionTimeBase_;

	dashTimer_ = 0;
}

void BossEnemy::Tackle(){
	frontVec_ = postureVec_;
	//ターゲットに顔を向ける
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
	//敵の色が真っ赤になったら突進開始
	if (enemyColor_.y <= colorLimit_) {
		if (attackTransitionTime_ <= 0) {
			move_ = { 0, 0, dashSpeed_ };

			move_ = Matrix::TransformNormal(move_, dashRotateMatrix_);

			/*敵の移動*/
			Vector3 NextPos = transform_.translate + move_;
			//移動の制限
			if (NextPos.x >= limitPos_.x or NextPos.x <= limitPos_.y) {
				move_.x = 0;
			}
			if (NextPos.z >= limitPos_.x or NextPos.z <= limitPos_.y) {
				move_.z = 0;
			}

			transform_.translate += move_* timeScale_;
			attackOBB_.size = bodyOBB_.size * collisionScale_;
			
			//既定の時間経過で通常状態に戻る
			if (++dashTimer_ >= kDashTime_) {
				attackOBB_.size = { 0,0,0 };
				behaviorRequest_ = Behavior::kFree;
			}
		}
		else {
			attackTransitionTime_--;
		}		
	}
	else {
		//突進準備中
		dashRotateMatrix_ = rotateMatrix_;

		enemyColor_.y -= colorSpeed_ * timeScale_;
		enemyColor_.z = enemyColor_.y;

		//directionTime_--;
	}
	
}
