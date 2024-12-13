#include "LockOn.h"

void LockOn::Initialize(){
	LockOnObj_ = std::make_unique<Object3D>();
	LockOnObj_->Initialize("LockOn");

	lockOnTransfrom_ = {
		{0.5f,0.5f,0.5f},
		{0.0f,0.0f,0.0f},
		{1.0f,1.0f,1.0f}
	};
}

void LockOn::Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, Input* input,const ViewingFrustum& viewingFrustum, const bool isAvoid, const uint32_t serialNumber){
	
	if (input->GetPadButtonTriger(XINPUT_GAMEPAD_START)||input->Trigerkey(DIK_L)) {
		if (autoLockOn_){
			autoLockOn_ = false;
			
			target_ = nullptr;
		}
		else {
			autoLockOn_ = true;
			isLockOn_ = true;
		}		
	}
	if (autoLockOn_) {
		if (input->GetPadButtonTriger(XINPUT_GAMEPAD_LEFT_SHOULDER) || input->Trigerkey(DIK_L)) {
			isLockOn_ = true;
		}
	}

	if (target_){
		//ロックオンしていたらの処理群

		//解除する
		if (input->GetPadButtonTriger(XINPUT_GAMEPAD_LEFT_SHOULDER) || input->Trigerkey(DIK_L)) {
			target_ = nullptr;
			isLockOn_ = false;
		}
		else if (!InTarget(target_->GetBodyOBB(), viewingFrustum)) {
			target_ = nullptr;
		}
		else if (!autoLockOn_ && input->GetPadButtonTriger(XINPUT_GAMEPAD_DPAD_RIGHT) || input->Trigerkey(DIK_L)) {
			//次の対象へ移る
			++it_;
			if (it_ == targets_.end()) {
				it_ = targets_.begin();
			}
			target_ = it_->second;
		}
		else if (target_->GetIsNoLife()) {
			target_ = nullptr;
		}
	}	
	else {
		//ジャスト回避したときサーチする
		if (isAvoid){
			avoidSearch(enemies, viewprojection, serialNumber);
		}
		
	}
	//レティクルの更新
	if (target_){
		lockOnTransfrom_.translate = target_->GetCenterPos();

		LockOnObj_->transform_ = lockOnTransfrom_;

		LockOnObj_->Update(viewprojection);
	}

}

void LockOn::Draw(){
	if (target_){
		LockOnObj_->Draw();
	}
	
}

void LockOn::DrawImgui(){
#ifdef _DEBUG
	LockOnObj_->DrawImgui("ロックオン");
	ImGui::Begin("ロックオンのサブステータス");
	ImGui::DragFloat3("現状の大きさ", &lockOnTransfrom_.scale.x, 0.1f, 0.0f, 10.0f);
	ImGui::DragFloat3("現状のポジション", &lockOnTransfrom_.translate.x, 0.1f, 0.0f, 10.0f);
	ImGui::End();
#endif
}

void LockOn::search(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, const ViewingFrustum& viewingFrustum){
	targets_.clear();
	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		if (enemy->GetIsDead()){
			continue;
		}
		Vector3 positionWorld = enemy->GetCenterPos();
		//ワールドビュー座標変換
		Vector3 positionView = Matrix::TransformVec(positionWorld, viewprojection.matView_);

		if (IsCollisionOBBViewFrustum(enemy->GetBodyOBB(), viewingFrustum)){
			targets_.emplace_back(std::make_pair(positionView.z, enemy.get()));
		}		
	}
	target_ = nullptr;
	if (!targets_.empty()) {
		//距離で昇順にソート
		targets_.sort([](auto& pair, auto& pair2) {return pair.first < pair2.first; });
		//ソートの結果一番近い敵をロックオン対象とする
		length_ = targets_.front().first;
		target_ = targets_.front().second;
		it_ = targets_.begin();
	}

}

void LockOn::avoidSearch(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, const uint32_t serialNumber){
	targets_.clear();
	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		if (enemy->GetIsDead()) {
			continue;
		}
		Vector3 positionWorld = enemy->GetCenterPos();

		//ワールドビュー座標変換
		Vector3 positionView = Matrix::TransformVec(positionWorld, viewprojection.matView_);
		
		targets_.emplace_back(std::make_pair(positionView.z, enemy.get()));
		
	}
	target_ = nullptr;
	if (!targets_.empty()) {
		//ナンバーと比較し一致していたらそいつをロックオンする
		for (const std::unique_ptr<Enemy>& enemy : enemies) {
			if (enemy->GetIsDead()) {
				continue;
			}
			if (enemy->GetSerialNumber() == serialNumber){
				target_ = enemy.get();
			}
			
		}	
		
	}
}

bool LockOn::InTarget(const OBB enemyOBB, const ViewingFrustum& viewingFrustum){
	return (IsCollisionOBBViewFrustum(enemyOBB, viewingFrustum));
}


void LockOn::TargetReset(){
	target_ = nullptr;
}

Vector3 LockOn::GetTargetPosition() const{
	if (target_){
		return target_->GetCenterPos();
	}
	return Vector3();
}

