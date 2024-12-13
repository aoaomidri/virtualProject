#include "EnemyManager.h"

void EnemyManager::Initialize(){
	RandomMaker* random = RandomMaker::GetInstance();


	for (size_t i = 0; i < enemyNum_; i++){
		enemysPos_[i] = Vector3(random->Distribution(-50.0f, 50.0f), 2.5f, random->Distribution(-50.0f, 50.0f));
	}

	for (size_t i = 0; i < enemyNum_; i++) {
		enemy_ = std::make_unique<Enemy>();
		enemy_->Initialize(enemysPos_[i]);
		enemy_->SetTarget(targetTrans_);
		enemy_->SetTargetMat(targetRotateMat_);
		enemy_->Update();
		enemies_.push_back(std::move(enemy_));
	}
}

void EnemyManager::Update(){
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

}

void EnemyManager::Draw(const FollowCamera* camera){
	for (const auto& enemy : enemies_) {
		enemy->Draw(camera->GetViewProjection());
	}
}

void EnemyManager::TexDraw(const FollowCamera* camera){
	for (const auto& enemy : enemies_) {
		enemy->TexDraw(camera->GetViewProjection().matViewProjection_);
	}
}

void EnemyManager::ParticleDraw(const FollowCamera* camera, const Vector3& color){
	for (const auto& enemy : enemies_) {
		enemy->ParticleDraw(camera->GetViewProjection(), color);
	}
}

void EnemyManager::DrawImgui(){
	for (const auto& enemy : enemies_) {
		enemy->DrawImgui();
	}
}
