#include "LockOn.h"

void LockOn::Initialize(){
	LockOnObj_ = std::make_unique<Object3D>();
	LockOnObj_->Initialize("LockOn");

	lockOnTransfrom_ = {
		{0.5f,0.5f,0.5f},
		{0.0f,0.0f,0.0f},
		{1.0f,1.0f,1.0f}
	};
	/*lockOnMark_ = std::make_unique<Sprite>();
	lockOnMark_->Initialize(21);
	
	lockOnMark_->SetLeftTop({ 0,0 });
	lockOnMark_->SetAnchorPoint({ 0.5f,0.5f });
	lockOnMark_->SetSize({ 64.0f,64.0f });
	lockOnMark_->SetScale({ 48.0f,48.0f });*/
}

void LockOn::Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, Input* input,const ViewingFrustum& viewingFrustum){
	
	if (input->GetPadButtonTriger(XINPUT_GAMEPAD_START)||input->Trigerkey(DIK_R)) {
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
		//lockOnMark_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		if (input->GetPadButtonTriger(XINPUT_GAMEPAD_LEFT_SHOULDER) || input->Trigerkey(DIK_R)) {
			isLockOn_ = true;
		}
	}
	else {
		//lockOnMark_->SetColor({ 0.0f,1.0f,1.0f,1.0f });
	}

	if (target_){
		if (input->GetPadButtonTriger(XINPUT_GAMEPAD_LEFT_SHOULDER) || input->Trigerkey(DIK_R)) {
			target_ = nullptr;
			isLockOn_ = false;
		}
		else if (!InTarget(target_->GetOBB(), viewprojection, viewingFrustum)) {
			target_ = nullptr;
		}
		else if (!autoLockOn_ && input->GetPadButtonTriger(XINPUT_GAMEPAD_DPAD_RIGHT) || input->Trigerkey(DIK_R)) {
			
			++it;
			if (it == targets.end()) {
				it = targets.begin();
			}
			target_ = it->second;
		}
		else if (target_->GetIsNoLife()) {
			target_ = nullptr;
		}
	}	
	else {
		if (!autoLockOn_){
			if (input->GetPadButtonTriger(XINPUT_GAMEPAD_LEFT_SHOULDER) || input->Trigerkey(DIK_R)) {
				search(enemies, viewprojection, viewingFrustum);
			}


		}
		else if(autoLockOn_){
			if (isLockOn_){
				search(enemies, viewprojection, viewingFrustum);
			}
			
		}
		
	}

	if (target_){
		
		//// ビューポート行列
		//Matrix4x4 matViewport =
		//	Matrix::MakeViewportMatrix(0, 0, 1280, 720, 0, 1);

		//// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		//Matrix4x4 matViewProjectionViewport =
		//	Matrix::Multiply(viewprojection.matViewProjection_, matViewport);

		//// ワールド->スクリーン座標変換(ここで3Dから2Dになる)
		//Vector3 screenPos3 = Matrix::TransformVec(target_->GetCenterPos(), matViewProjectionViewport);

		//screenPos_ = { screenPos3.x,screenPos3.y };

		//// スプライトのレティクルに座標設定
		////sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
		//lockOnMark_->SetPosition(screenPos_);
		lockOnTransfrom_.translate = target_->GetCenterPos();

		LockOnObj_->transform_ = lockOnTransfrom_;

		LockOnObj_->Update(viewprojection);
	}

	
	/*ImGui::Begin("距離");
	ImGui::DragFloat("最短距離", &length, 0.1f);
	ImGui::End();*/
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
	targets.clear();
	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		if (enemy->GetIsDead()){
			continue;
		}
		Vector3 positionWorld = enemy->GetCenterPos();
		//ワールドビュー座標変換
		Vector3 positionView = Matrix::TransformVec(positionWorld, viewprojection.matView_);

		if (IsCollisionOBBViewFrustum(enemy->GetOBB(), viewingFrustum)){
			targets.emplace_back(std::make_pair(positionView.z, enemy.get()));
		}		
	}
	target_ = nullptr;
	if (!targets.empty()) {
		//距離で昇順にソート
		targets.sort([](auto& pair, auto& pair2) {return pair.first < pair2.first; });
		//ソートの結果一番近い敵をロックオン対象とする
		length = targets.front().first;
		target_ = targets.front().second;
		it = targets.begin();
	}

}

bool LockOn::InTarget(const OBB enemyOBB, const ViewProjection& viewprojection, const ViewingFrustum& viewingFrustum){
	return (IsCollisionOBBViewFrustum(enemyOBB, viewingFrustum));
}


void LockOn::TargetReset(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewprojection, const ViewingFrustum& viewingFrustum){
	targets.clear();
	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		if (enemy->GetIsDead()) {
			continue;
		}
		Vector3 positionWorld = enemy->GetCenterPos();
		//ワールドビュー座標変換
		Vector3 positionView = Matrix::TransformVec(positionWorld, viewprojection.matView_);

		if (IsCollisionOBBViewFrustum(enemy->GetOBB(), viewingFrustum)) {
			targets.emplace_back(std::make_pair(positionView.z, enemy.get()));
		}
	}
	it = targets.begin();
}

Vector3 LockOn::GetTargetPosition() const{
	if (target_){
		return target_->GetCenterPos();
	}
	return Vector3();
}

bool LockOn::IsCollisionViewFrustum(const OBB& obb, const ViewingFrustum& viewingFrustum) {
	
	/*ステップ1視錐台の生成*/
	// 頂点の個数
	const int OBBVertex = 8;
	const int FrustumVertex = 8;

	// 法線の個数
	const int normalLine = 6;

	// 当たる距離
	const float CollisionDistance = 0.00f;

	// それぞれの幅
	Vector2 nearPlane{};
	Vector2 farPlane{};
	// 面の頂点
	Vector3 nearPlanePoints_[4] = { 0 };
	Vector3 farPlanePoints_[4] = { 0 };
	// 視錐台の行列
	Matrix4x4 FrustumMatWorld = Matrix::MakeAffineMatrix(
		{ 1.0f, 1.0f, 1.0f }, { viewingFrustum.rotate_ }, { viewingFrustum.translation_ });

	// 向きベクトルnear
	Vector3 directionNear = Vector3::Normalize(viewingFrustum.direction);
	directionNear = directionNear * viewingFrustum.nearZ;
	// 向きベクトルfar
	Vector3 directionFar = Vector3::Normalize(viewingFrustum.direction);
	directionFar = directionFar * viewingFrustum.farZ;

	// 近平面の縦横
	nearPlane.y = Vector3::Length(directionNear) * std::tan(viewingFrustum.verticalFOV / 2);
	nearPlane.x = nearPlane.y * viewingFrustum.aspectRatio;
	// 遠平面の縦横
	farPlane.y = Vector3::Length(directionFar) * std::tan(viewingFrustum.verticalFOV / 2);
	farPlane.x = farPlane.y * viewingFrustum.aspectRatio;

	nearPlanePoints_[0] = {
		directionNear.x + -nearPlane.x, directionNear.y + nearPlane.y, directionNear.z }; // 左上
	nearPlanePoints_[1] = {
		directionNear.x + nearPlane.x, directionNear.y + nearPlane.y, directionNear.z }; // 右上
	nearPlanePoints_[2] = {
		directionNear.x + -nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z }; // 左下
	nearPlanePoints_[3] = {
		directionNear.x + nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z }; // 右下

	farPlanePoints_[0] = {
		directionFar.x + -farPlane.x, directionFar.y + farPlane.y, directionFar.z }; // 左上
	farPlanePoints_[1] = {
		directionFar.x + farPlane.x, directionFar.y + farPlane.y, directionFar.z }; // 右上
	farPlanePoints_[2] = {
		directionFar.x + -farPlane.x, directionFar.y + -farPlane.y, directionFar.z }; // 左下
	farPlanePoints_[3] = {
		directionFar.x + farPlane.x, directionFar.y + -farPlane.y, directionFar.z }; // 右下

	// 頂点
	Vector3 FrustumPoints[FrustumVertex]{ 0 };
	// near
	FrustumPoints[0] = nearPlanePoints_[0];
	FrustumPoints[1] = nearPlanePoints_[1];
	FrustumPoints[2] = nearPlanePoints_[2];
	FrustumPoints[3] = nearPlanePoints_[3];
	// far
	FrustumPoints[4] = farPlanePoints_[0];
	FrustumPoints[5] = farPlanePoints_[1];
	FrustumPoints[6] = farPlanePoints_[2];
	FrustumPoints[7] = farPlanePoints_[3];

	/*ステップ2 OBBの生成*/

	Vector3 obbPoints[OBBVertex]{};

	// obbの行列
	Matrix4x4 worldMatrix = {
		{obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0},
		{obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0},
		{obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0},
		{obb.center.x,          obb.center.y,          obb.center.z,          1} };

	obbPoints[0] = obb.size * -1;
	obbPoints[1] = { obb.size.x * -1, obb.size.y * -1, obb.size.z };
	obbPoints[2] = { obb.size.x, obb.size.y * -1, obb.size.z * -1 };
	obbPoints[3] = { obb.size.x, obb.size.y * -1, obb.size.z };
	obbPoints[4] = { obb.size.x * -1, obb.size.y, obb.size.z * -1 };
	obbPoints[5] = { obb.size.x * -1, obb.size.y, obb.size.z };
	obbPoints[6] = { obb.size.x, obb.size.y, obb.size.z * -1 };
	obbPoints[7] = obb.size;

	for (int i = 0; i < OBBVertex; i++) {
		obbPoints[i] = Matrix::TransformNormal(obbPoints[i], worldMatrix);
		obbPoints[i] = obb.center + obbPoints[i];
	}

	/*ステップ3 OBBを視錐台のローカル座標に変換*/
	// 視錐台の逆行列
	Matrix4x4 FrustumInverceMat = Matrix::Inverce(FrustumMatWorld);
	for (int i = 0; i < OBBVertex; i++) {
		obbPoints[i] = Matrix::TransformVec(obbPoints[i], FrustumInverceMat);
	}

	/*ステップ4 当たり判定*/
	// near面から
	Vector3 v01 = FrustumPoints[1] - FrustumPoints[0];
	Vector3 v12 = FrustumPoints[2] - FrustumPoints[1];

	// far
	Vector3 v65 = FrustumPoints[5] - FrustumPoints[6];
	Vector3 v54 = FrustumPoints[4] - FrustumPoints[5];

	// left
	Vector3 v02 = FrustumPoints[2] - FrustumPoints[0];
	Vector3 v26 = FrustumPoints[6] - FrustumPoints[2];

	// right
	Vector3 v53 = FrustumPoints[3] - FrustumPoints[5];
	Vector3 v31 = FrustumPoints[1] - FrustumPoints[3];

	// up
	Vector3 v41 = FrustumPoints[1] - FrustumPoints[4];
	Vector3 v10 = FrustumPoints[0] - FrustumPoints[1];

	// down
	Vector3 v23 = FrustumPoints[3] - FrustumPoints[2];
	Vector3 v36 = FrustumPoints[6] - FrustumPoints[3];

	Vector3 normal[normalLine] = {};

	float distance[48] = {};
	// near
	normal[0] = Vector3::Normalize(Vector3::Cross(v01, v12));
	// far
	normal[1] = Vector3::Normalize(Vector3::Cross(v65, v54));
	// left
	normal[2] = Vector3::Normalize(Vector3::Cross(v02, v26));
	// right
	normal[3] = Vector3::Normalize(Vector3::Cross(v53, v31));
	// up
	normal[4] = Vector3::Normalize(Vector3::Cross(v41, v10));
	// down
	normal[5] = Vector3::Normalize(Vector3::Cross(v23, v36));
	for (int i = 0; i < 8; i++) {
		distance[0 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[0], normal[0]);
		distance[1 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[4], normal[1]);
		distance[2 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[0], normal[2]);
		distance[3 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[1], normal[3]);
		distance[4 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[0], normal[4]);
		distance[5 + i * normalLine] = Vector3::Dot(obbPoints[i] - FrustumPoints[2], normal[5]);
	}

	for (int i = 0; i < 8; i++) {
		if (distance[0 + i * normalLine] <= CollisionDistance &&
			distance[1 + i * normalLine] <= CollisionDistance &&
			distance[2 + i * normalLine] <= CollisionDistance &&
			distance[3 + i * normalLine] <= CollisionDistance &&
			distance[4 + i * normalLine] <= CollisionDistance &&
			distance[5 + i * normalLine] <= CollisionDistance) {
			return true;
		}
	}

	return false;
}

bool LockOn::IsCollisionOBB(const OBB& obb, const ViewingFrustum& viewingFrustum) {
	/*ステップ1視錐台の生成*/
	// 頂点の個数
	const int OBBVertex = 8;
	const int FrustumVertex = 8;

	// 法線の個数
	const int normalLine = 6;

	// 当たる距離
	const float CollisionDistance = 0.00f;

	// それぞれの幅
	Vector2 nearPlane{};
	Vector2 farPlane{};
	// 面の頂点
	Vector3 nearPlanePoints_[4] = { 0 };
	Vector3 farPlanePoints_[4] = { 0 };
	// 視錐台の行列
	Matrix4x4 FrustumMatWorld = Matrix::MakeAffineMatrix(
		{ 1.0f, 1.0f, 1.0f }, { viewingFrustum.rotate_ }, { viewingFrustum.translation_ });

	// 向きベクトルnear
	Vector3 directionNear = Vector3::Normalize(viewingFrustum.direction);
	directionNear = directionNear * viewingFrustum.nearZ;
	// 向きベクトルfar
	Vector3 directionFar = Vector3::Normalize(viewingFrustum.direction);
	directionFar = directionFar * viewingFrustum.farZ;

	// 近平面の縦横
	nearPlane.y = Vector3::Length(directionNear) * std::tan(viewingFrustum.verticalFOV / 2);
	nearPlane.x = nearPlane.y * viewingFrustum.aspectRatio;
	// 遠平面の縦横
	farPlane.y = Vector3::Length(directionFar) * std::tan(viewingFrustum.verticalFOV / 2);
	farPlane.x = farPlane.y * viewingFrustum.aspectRatio;

	nearPlanePoints_[0] = {
		directionNear.x + -nearPlane.x, directionNear.y + nearPlane.y, directionNear.z }; // 左上
	nearPlanePoints_[1] = {
		directionNear.x + nearPlane.x, directionNear.y + nearPlane.y, directionNear.z }; // 右上
	nearPlanePoints_[2] = {
		directionNear.x + -nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z }; // 左下
	nearPlanePoints_[3] = {
		directionNear.x + nearPlane.x, directionNear.y + -nearPlane.y, directionNear.z }; // 右下

	farPlanePoints_[0] = {
		directionFar.x + -farPlane.x, directionFar.y + farPlane.y, directionFar.z }; // 左上
	farPlanePoints_[1] = {
		directionFar.x + farPlane.x, directionFar.y + farPlane.y, directionFar.z }; // 右上
	farPlanePoints_[2] = {
		directionFar.x + -farPlane.x, directionFar.y + -farPlane.y, directionFar.z }; // 左下
	farPlanePoints_[3] = {
		directionFar.x + farPlane.x, directionFar.y + -farPlane.y, directionFar.z }; // 右下

	for (int i = 0; i < 4; i++) {
		nearPlanePoints_[i] = Matrix::TransformNormal(nearPlanePoints_[i], FrustumMatWorld);
		farPlanePoints_[i] = Matrix::TransformNormal(farPlanePoints_[i], FrustumMatWorld);
	}

	// 頂点
	Vector3 FrustumPoints[FrustumVertex]{ 0 };
	// near
	FrustumPoints[0] = viewingFrustum.translation_ + nearPlanePoints_[0];
	FrustumPoints[1] = viewingFrustum.translation_ + nearPlanePoints_[1];
	FrustumPoints[2] = viewingFrustum.translation_ + nearPlanePoints_[2];
	FrustumPoints[3] = viewingFrustum.translation_ + nearPlanePoints_[3];
	// far
	FrustumPoints[4] = viewingFrustum.translation_ + farPlanePoints_[0];
	FrustumPoints[5] = viewingFrustum.translation_ + farPlanePoints_[1];
	FrustumPoints[6] = viewingFrustum.translation_ + farPlanePoints_[2];
	FrustumPoints[7] = viewingFrustum.translation_ + farPlanePoints_[3];

	/*ステップ2 OBBの生成*/

	Vector3 obbPoints[OBBVertex]{};

	// obbの行列
	Matrix4x4 worldMatrix = {
		{obb.orientations[0].x, obb.orientations[0].y, obb.orientations[0].z, 0},
		{obb.orientations[1].x, obb.orientations[1].y, obb.orientations[1].z, 0},
		{obb.orientations[2].x, obb.orientations[2].y, obb.orientations[2].z, 0},
		{obb.center.x,          obb.center.y,          obb.center.z,          1} };

	// 手前
	obbPoints[0] = { obb.size.x * -1, obb.size.y, obb.size.z * -1 }; // 左上
	obbPoints[1] = { obb.size.x, obb.size.y, obb.size.z * -1 };      // 右上
	obbPoints[2] = obb.size * -1;                                  // 左下
	obbPoints[3] = { obb.size.x, obb.size.y * -1, obb.size.z * -1 }; // 右下
	// 奥
	obbPoints[4] = { obb.size.x * -1, obb.size.y, obb.size.z };      // 左上
	obbPoints[5] = obb.size;                                       // 右上
	obbPoints[6] = { obb.size.x * -1, obb.size.y * -1, obb.size.z }; // 左下
	obbPoints[7] = { obb.size.x, obb.size.y * -1, obb.size.z };      // 右下

	/*ステップ3 視錐台をOBBのローカル座標に変換*/
	// OBBの逆行列
	Matrix4x4 OBBInverceMat = Matrix::Inverce(worldMatrix);
	for (int i = 0; i < OBBVertex; i++) {
		FrustumPoints[i] = Matrix::TransformVec(FrustumPoints[i], OBBInverceMat);
	}

	/*ステップ4 当たり判定*/
	// near面から
	Vector3 v01 = obbPoints[1] - obbPoints[0];
	Vector3 v12 = obbPoints[2] - obbPoints[1];

	// far
	Vector3 v65 = obbPoints[5] - obbPoints[6];
	Vector3 v54 = obbPoints[4] - obbPoints[5];

	// left
	Vector3 v02 = obbPoints[2] - obbPoints[0];
	Vector3 v26 = obbPoints[6] - obbPoints[2];

	// right
	Vector3 v53 = obbPoints[3] - obbPoints[5];
	Vector3 v31 = obbPoints[1] - obbPoints[3];

	// up
	Vector3 v41 = obbPoints[1] - obbPoints[4];
	Vector3 v10 = obbPoints[0] - obbPoints[1];

	// down
	Vector3 v23 = obbPoints[3] - obbPoints[2];
	Vector3 v36 = obbPoints[6] - obbPoints[3];

	Vector3 normal[normalLine] = {};

	float distance[48] = {};
	// near
	normal[0] = Vector3::Normalize(Vector3::Cross(v01, v12));
	// far
	normal[1] = Vector3::Normalize(Vector3::Cross(v65, v54));
	// left
	normal[2] = Vector3::Normalize(Vector3::Cross(v02, v26));
	// right
	normal[3] = Vector3::Normalize(Vector3::Cross(v53, v31));
	// up
	normal[4] = Vector3::Normalize(Vector3::Cross(v41, v10));
	// down
	normal[5] = Vector3::Normalize(Vector3::Cross(v23, v36));
	for (int i = 0; i < 8; i++) {
		distance[0 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[0], normal[0]);
		distance[1 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[4], normal[1]);
		distance[2 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[0], normal[2]);
		distance[3 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[1], normal[3]);
		distance[4 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[0], normal[4]);
		distance[5 + i * normalLine] = Vector3::Dot(FrustumPoints[i] - obbPoints[2], normal[5]);
	}

	for (int i = 0; i < 8; i++) {
		if (distance[0 + i * normalLine] <= CollisionDistance &&
			distance[1 + i * normalLine] <= CollisionDistance &&
			distance[2 + i * normalLine] <= CollisionDistance &&
			distance[3 + i * normalLine] <= CollisionDistance &&
			distance[4 + i * normalLine] <= CollisionDistance &&
			distance[5 + i * normalLine] <= CollisionDistance) {
			return true;
		}
	}

	return false;
}

bool LockOn::IsCollisionOBBViewFrustum(const OBB& obb, const ViewingFrustum& viewingFrustum) {
	if (IsCollisionOBB(obb, viewingFrustum) || IsCollisionViewFrustum(obb, viewingFrustum)) {
		return true;
	}
	else {
		return false;
	}
}
