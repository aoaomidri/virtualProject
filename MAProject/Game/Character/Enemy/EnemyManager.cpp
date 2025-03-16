#include "EnemyManager.h"

EnemyManager::EnemyManager(){

}

void EnemyManager::Initialize(){
	RandomMaker* random = RandomMaker::GetInstance();
	isTutoria_ = true;

	tickets_ = std::make_unique<EnemyAttackTicket>(enemyNum_, 20);

	boss_ = std::make_unique<BossEnemy>();
	boss_->Initialize(bossPos_);
	boss_->SetTarget(targetTrans_);
	boss_->SetTargetMat(targetRotateMat_);
	boss_->Update();
	for (size_t i = 0; i < enemyNum_; i++){
		enemysPos_[i] = Vector3(random->Distribution(-50.0f, 50.0f), 2.5f, random->Distribution(-50.0f, 50.0f));
	}
	for (size_t i = 0; i < enemyNum_; i++) {
		enemy_ = std::make_unique<Enemy>();
		enemy_->Initialize(enemysPos_[i],tickets_.get());
		enemy_->SetTarget(targetTrans_);
		enemy_->SetTargetMat(targetRotateMat_);
		enemy_->Update();
		enemies_.push_back(std::move(enemy_));
	}

	for (size_t i = 0; i < tutorialEnemyNum_; i++) {
		enemy_ = std::make_unique<Enemy>();
		enemy_->Initialize(enemysPos_[i], tickets_.get());
		enemy_->SetTarget(targetTrans_);
		enemy_->SetTargetMat(targetRotateMat_);
		enemy_->Update();
		tutorialEnemies_.push_back(std::move(enemy_));
	}
}

void EnemyManager::Update(){
	if (isTutoria_){
		tutorialEnemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
			if (enemy->GetIsDead()) {
				return true;
			}
			return false;
		});
		for (const auto& enemy : tutorialEnemies_) {
			enemy->SetTimeScale(timeScale_);
			enemy->SetShininess(enemyShininess_);
			enemy->Update();
		}
	}
	else {
		//条件が合致したらリストから排除
		enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
			if (enemy->GetIsDead()) {
				return true;
			}
			return false;
		});

		for (const auto& enemy : enemies_) {
			enemy->SetTimeScale(timeScale_);
			enemy->SetShininess(enemyShininess_);
			enemy->Update();
		}
		boss_->SetTimeScale(timeScale_);
		boss_->SetShininess(enemyShininess_);
		boss_->Update();
	}

	
}

void EnemyManager::Draw(const FollowCamera* camera){
	if (isTutoria_){
		for (const auto& enemy : tutorialEnemies_) {
			enemy->Draw(camera->GetViewProjection());
		}
	}
	else {
		for (const auto& enemy : enemies_) {
			enemy->Draw(camera->GetViewProjection());
		}
		boss_->Draw(camera->GetViewProjection());
	}
}

void EnemyManager::TexDraw(const FollowCamera* camera){
	if (isTutoria_) {
		for (const auto& enemy : tutorialEnemies_) {
			enemy->TexDraw(camera->GetViewProjection().matViewProjection_);
		}
	}
	else {
		for (const auto& enemy : enemies_) {
			enemy->TexDraw(camera->GetViewProjection().matViewProjection_);
		}
		boss_->TexDraw(camera->GetViewProjection().matViewProjection_);
	}	
}

void EnemyManager::ParticleDraw(const FollowCamera* camera, const Vector3& color){
	if (isTutoria_) {
		for (const auto& enemy : tutorialEnemies_) {
			enemy->ParticleDraw(camera->GetViewProjection(), color);
		}
	}
	else {
		for (const auto& enemy : enemies_) {
			enemy->ParticleDraw(camera->GetViewProjection(), color);
		}
		boss_->ParticleDraw(camera->GetViewProjection(), color);
	}	
}

void EnemyManager::DrawImgui(){
	for (const auto& enemy : enemies_) {
		enemy->DrawImgui();
	}
	boss_->DrawImgui();
	tickets_->DrawImgui();
	ImGui::Begin("敵の数");
	ImGui::Text("今の敵の数 = %d", enemies_.size());
	ImGui::Text("今の攻撃の数 = %d", tickets_->GetActiveAttckers());
	if (ImGui::Button("敵を増やす")){
		AddEnemys();
	}	
	ImGui::End();
}

void EnemyManager::AddEnemys(){
	RandomMaker* random = RandomMaker::GetInstance();

	for (size_t i = 0; i < addEnemyNum_; i++) {
		enemysPos_[i] = Vector3(random->Distribution(-50.0f, 50.0f), 2.5f, random->Distribution(-50.0f, 50.0f));
	}
	for (size_t i = 0; i < addEnemyNum_; i++) {
		enemy_ = std::make_unique<Enemy>();
		enemy_->Initialize(enemysPos_[i], tickets_.get());
		enemy_->SetTarget(targetTrans_);
		enemy_->SetTargetMat(targetRotateMat_);		
		enemies_.push_back(std::move(enemy_));
	}
}
