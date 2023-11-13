#include "GameScene.h"
#include"math/Vector3.h"
#include <cassert>
void GameScene::TextureLoad() {
	textureManager_->Load("resources/uvChecker.png", 0);
	textureManager_->Load("resources/rock.png", 1);
	textureManager_->Load("resources/Floor.png", 2);
	textureManager_->Load("resources/Road.png", 3);
	textureManager_->Load("resources/Sky.png", 4);
	textureManager_->Load("resources/Enemy/EnemyTex.png", 5);
	textureManager_->Load("resources/EnemyParts/EnemyParts.png", 6);
	textureManager_->Load("resources/Weapon/Sword.png", 7);
}

void GameScene::Initialize(DirectXCommon* dxCommon_){
	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(dxCommon_->GetDevice(),
		dxCommon_->GetCommandList(), dxCommon_->GetSRVHeap());
	TextureLoad();

	for (int i = 0; i < 3; i++){
		floor_[i] = std::make_unique<Object3D>();
		floor_[i]->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "box");
	}
	 goal_ = std::make_unique<Object3D>();
	goal_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "box");

	 enemy_ = std::make_unique<Object3D>();
	enemy_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "Enemy");

	 enemyParts_ = std::make_unique<Object3D>();
	enemyParts_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "EnemyParts");

	 skyDome_ = std::make_unique<Object3D>();
	skyDome_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "skyDome");

	testTexture_ = std::make_unique<Sprite>();
	testTexture_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());

	player_ = std::make_unique<Player>();
	player_->Initislize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	//自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetTransform());

	player_->SetCameraTransform(&followCamera_->GetCameraTransform());

	floorTransform[0] = {
		.scale = {2.0f,0.5f,2.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};

	floorTransform[1] = {
		.scale = {2.0f,0.5f,2.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,4.0f}
	};

	floorTransform[2] = {
		.scale = {2.0f,0.5f,2.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,8.0f}
	};

	axis = Matrix::GetInstance()->Normalize({ 1.0f,1.0f,1.0f });
	rotateMatrix = Matrix::GetInstance()->MakeRotateAxisAngle(axis, angle);

	from0 = Vector3::Normalize({ 1.0f,0.7f,0.5f });
	to0 = Vector3::Normalize({ -1.0f,-0.7f,-0.5f });
	from1 = Vector3::Normalize({ -0.6f,0.9f,0.2f });
	to1 = Vector3::Normalize({ 0.4f,0.7f,-0.5f });

	rotateMatrix0 = Matrix::GetInstance()->DirectionToDirection(Vector3::Normalize({ 1.0f,0.0f,0.0f }), Vector3::Normalize({ -1.0f,0.0f,0.0f }));
	rotateMatrix1 = Matrix::GetInstance()->DirectionToDirection(from0, to0);
	rotateMatrix2 = Matrix::GetInstance()->DirectionToDirection(from1, to1);

}

void GameScene::Update(Input* input_){
	DrawImgui();
	//followCamera_->Update(input_);

	//testTexture_->Update();
	//testTexture_->SetPosition(spritePosition_);
	//testTexture_->SetRotation(spriteRotate_);
	//testTexture_->SetScale(spriteScale_);
	//testTexture_->SetAnchorPoint(spriteAnchorPoint_);
	//testTexture_->SetColor(spriteColor_);
	//testTexture_->SetIsDraw(isSpriteDraw);

	///*敵の移動*/
	//EnemyTransform.translate.x += EnemyMoveSpeed_ * EnemyMagnification;

	//if (EnemyTransform.translate.x <= -2.0f) {
	//	EnemyMagnification *= -1.0f;
	//}
	//else if (EnemyTransform.translate.x >= 2.0f) {
	//	EnemyMagnification *= -1.0f;
	//}
	///*エネミーのパーツ*/
	//EnemyPartsTransform.translate.x = EnemyTransform.translate.x;
	//EnemyPartsTransform.translate.y = EnemyTransform.translate.y + 0.9f;
	//EnemyPartsTransform.translate.z = EnemyTransform.translate.z;

	//EnemyPartsTransform.rotate.x += 0.3f;

	//enemyMatrix = Matrix::GetInstance()->MakeAffineMatrix(EnemyTransform.scale, EnemyTransform.rotate, EnemyTransform.translate);
	//enemyPartsMatrix = Matrix::GetInstance()->MakeAffineMatrix(EnemyPartsTransform.scale, EnemyPartsTransform.rotate, EnemyPartsTransform.translate);
	//moveFloorMatrix[0] = Matrix::GetInstance()->MakeAffineMatrix(floorTransform[0].scale, floorTransform[0].rotate, floorTransform[0].translate);
	//moveFloorMatrix[1] = Matrix::GetInstance()->MakeAffineMatrix(floorTransform[1].scale, floorTransform[1].rotate, floorTransform[1].translate);
	//moveFloorMatrix[2] = Matrix::GetInstance()->MakeAffineMatrix(floorTransform[2].scale, floorTransform[2].rotate, floorTransform[2].translate);
	//skyDomeMatrix = Matrix::GetInstance()->MakeAffineMatrix(skyDomeTransform.scale, skyDomeTransform.rotate, skyDomeTransform.translate);
	//goalMatrix = Matrix::GetInstance()->MakeAffineMatrix(goalTransform.scale, goalTransform.rotate, goalTransform.translate);

	//enemyOBB.center = EnemyTransform.translate;
	//enemyOBB.size = EnemyTransform.scale;
	//Matrix4x4 enemyRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(EnemyTransform.rotate);
	//SetOridentatios(enemyOBB, enemyRotateMatrix);

	//for (int i = 0; i < 3; i++) {
	//	floorOBB[i].center = floorTransform[i].translate;
	//	floorOBB[i].size = floorTransform[i].scale;
	//	Matrix4x4 floorRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(floorTransform[i].rotate);
	//	SetOridentatios(floorOBB[i], floorRotateMatrix);
	//}

	//goalOBB.center = goalTransform.translate;
	//goalOBB.size = goalTransform.scale;
	//Matrix4x4 GoalRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(goalTransform.rotate);
	//SetOridentatios(goalOBB, GoalRotateMatrix);

	///*if (IsCollisionOBBOBB(playerOBB, floorOBB[1])) {
	//	FloorPlayerPosition += move;
	//}
	//else {
	//	FloorPlayerPosition = PlayerTransform.translate - floorTransform[1].translate;
	//}

	//movePlayerMatrix = Matrix::GetInstance()->MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), PlayerTransform.rotate, PlayerTransform.translate);
	//moveFloorTransformMatrix = Matrix::GetInstance()->MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(floorTransform[1].rotate), Vector3(floorTransform[1].translate));*/



	///*OBBの設定および当たり判定処理*/


	//for (int i = 0; i < 3; i++) {
	//	if (IsCollisionOBBOBB(player_->GetOBB(), floorOBB[i])) {
	//		chackCollision = 1;
	//		player_->onFlootCollision(floorOBB[i]);
	//		player_->SetIsDown(false);

	//		break;
	//	}
	//	else{
	//		chackCollision = 0;
	//		player_->SetIsDown(true);
	//	}
	//}
	///*if (IsCollisionOBBOBB(playerOBB, floorOBB[1])) {

	//	player_->parent_ = &moveFloorTransformMatrix;

	//	playerMatrix = Matrix::GetInstance()->Multiply(playerMatrix, (Matrix::GetInstance()->Inverce(movePlayerMatrix)));

	//	playerMatrix.m[3][0] = (FloorPlayerPosition.x);
	//	playerMatrix.m[3][1] = FloorPlayerPosition.y;
	//	playerMatrix.m[3][2] = (FloorPlayerPosition.z);

	//	PlayerTransform.translate = floorTransform[1].translate +FloorPlayerPosition;

	//}
	//else {
	//	player_->parent_ = nullptr;
	//}*/

	////cameraTransform.translate = vector_.Add(player_->GetTranslate(), cameraOffset);

	//floorTransform[1].translate.x += moveSpeed_.x * Magnification;
	//floorTransform[1].translate.y += moveSpeed_.y * MagnificationY;

	//if (floorTransform[1].translate.x <= -4.0f) {
	//	Magnification *= -1.0f;
	//}
	//else if (floorTransform[1].translate.x >= 4.0f) {
	//	Magnification *= -1.0f;
	//}

	//if (floorTransform[1].translate.y <= -2.0f) {
	//	MagnificationY *= -1.0f;
	//}
	//else if (floorTransform[1].translate.y >= 2.0f) {
	//	MagnificationY *= -1.0f;
	//}

	//if (IsCollisionOBBOBB(player_->GetOBB(), goalOBB) || IsCollisionOBBOBB(player_->GetOBB(), enemyOBB)) {
	//	player_->Respawn();
	//}

	//if ( IsCollisionOBBOBB(player_->GetWeaponOBB(), enemyOBB)) {
	//	EnemyTransform.translate.y = 200.0f;
	//}


	///*ここまで*/
	//skyDomeTransform.rotate.y += 0.01f;

	//for (int i = 0; i < 3; i++){
	//	floor_[i]->Update(moveFloorMatrix[i], followCamera_->GetCameraTransform());
	//}

	//player_->Update(input_);
	//

	//enemy_->Update(enemyMatrix, followCamera_->GetCameraTransform());

	//enemyParts_->Update(enemyPartsMatrix, followCamera_->GetCameraTransform());

	//goal_->Update(goalMatrix, followCamera_->GetCameraTransform());

	//skyDome_->Update(skyDomeMatrix, followCamera_->GetCameraTransform());

	//if (input_->Trigerkey(DIK_R)){
	//	EnemyTransform.translate = { 0.0f,0.8f,7.0f };
	//}
}

void GameScene::Draw3D(){
	/*描画前処理*/
	textureManager_->PreDraw3D();
	/*ここから下に描画処理を書き込む*/

	/*for (int i = 0; i < 3; i++){
		floor_[i]->Draw(textureManager_->SendGPUDescriptorHandle(3));
	}
	player_->Draw(textureManager_.get(), followCamera_->GetCameraTransform());

	enemy_->Draw(textureManager_->SendGPUDescriptorHandle(5));

	enemyParts_->Draw(textureManager_->SendGPUDescriptorHandle(6));

	goal_->Draw(textureManager_->SendGPUDescriptorHandle(2));

	skyDome_->Draw(textureManager_->SendGPUDescriptorHandle(1));*/

	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw3D();
}

void GameScene::Draw2D(){
	/*描画前処理*/
	textureManager_->PreDraw2D();
	///*ここから下に描画処理を書き込む*/
	//testTexture_->Draw(textureManager_->SendGPUDescriptorHandle(0));

	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw2D();
}

void GameScene::DrawImgui(){
	ImGui::Begin("ある方向からある方向へ向ける回転行列");
	if (ImGui::TreeNode("一個目")){
		for (int i = 0; i < 4; i++) {
			ImGui::DragFloat4((std::to_string(i + 1) + "行目").c_str(), rotateMatrix0.m[i], 0.001f);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("二個目")){
		for (int i = 0; i < 4; i++) {
			ImGui::DragFloat4((std::to_string(i + 1) + "行目").c_str(), rotateMatrix1.m[i], 0.001f);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("三個目")) {
		for (int i = 0; i < 4; i++) {
			ImGui::DragFloat4((std::to_string(i + 1) + "行目").c_str(), rotateMatrix2.m[i], 0.001f);
		}
		ImGui::TreePop();
	}
	
	
	ImGui::End();

	
	/*ImGui::Begin("床のTransform");
	if (ImGui::TreeNode("一個目")) {
		ImGui::DragFloat3("床の座標", &floorTransform[0].translate.x, 0.01f);
		ImGui::DragFloat3("床の回転", &floorTransform[0].rotate.x, 0.01f);
		ImGui::DragFloat3("床の大きさ", &floorTransform[0].scale.x, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("二個目")) {
		ImGui::DragFloat3("床の座標", &floorTransform[1].translate.x, 0.01f);
		ImGui::DragFloat3("床の回転", &floorTransform[1].rotate.x, 0.01f);
		ImGui::DragFloat3("床の大きさ", &floorTransform[1].scale.x, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("三個目")) {
		ImGui::DragFloat3("床の座標", &floorTransform[2].translate.x, 0.01f);
		ImGui::DragFloat3("床の回転", &floorTransform[2].rotate.x, 0.01f);
		ImGui::DragFloat3("床の大きさ", &floorTransform[2].scale.x, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();

	ImGui::Begin("床から見たプレイヤー");
	ImGui::DragFloat3("ベクトル", &FloorPlayerPosition.x, 0.01f);
	ImGui::End();

	ImGui::Begin("ゴール");
	ImGui::Text("床とプレイヤーが接触しているか %d ", chackCollision);
	ImGui::DragFloat3("ゴールの座標", &goalTransform.translate.x, 0.01f);
	ImGui::DragFloat3("ゴールの回転", &goalTransform.rotate.x, 0.01f);
	ImGui::DragFloat3("ゴールの大きさ", &goalTransform.scale.x, 0.01f);
	ImGui::End();

	ImGui::Begin("2Dテクスチャ");
	ImGui::DragFloat2("座標", &spritePosition_.x, 1.0f);
	ImGui::DragFloat("回転", &spriteRotate_, 0.01f);
	ImGui::DragFloat2("大きさ", &spriteScale_.x, 1.0f);
	ImGui::DragFloat2("アンカーポイント", &spriteAnchorPoint_.x, 0.1f, 0.0f, 1.0f);
	ImGui::ColorEdit4("画像の色", &spriteColor_.x);
	ImGui::Checkbox("画像を描画する", &isSpriteDraw);
	ImGui::End();*/
}

bool GameScene::IsCollisionOBBOBB(const OBB& obb1, const OBB& obb2){
	static Vector3 vector;
	static Matrix matrix;

	Vector3 separatingAxis[15]{};
	separatingAxis[0] = obb1.orientations[0];
	separatingAxis[1] = obb1.orientations[1];
	separatingAxis[2] = obb1.orientations[2];
	separatingAxis[3] = obb2.orientations[0];
	separatingAxis[4] = obb2.orientations[1];
	separatingAxis[5] = obb2.orientations[2];
	int axisNum = 6;
	for (int index1 = 0; index1 < 3; index1++) {
		for (int index2 = 0; index2 < 3; index2++) {
			separatingAxis[axisNum] = vector.Cross(obb1.orientations[index1], obb2.orientations[index2]);
			axisNum++;
		}
	}

	Vector3 obb1Vetyex[8]{};
	// bottom
	obb1Vetyex[0] = Vector3{ -obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vetyex[1] = Vector3{ +obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vetyex[2] = Vector3{ -obb1.size.x, -obb1.size.y, +obb1.size.z };
	obb1Vetyex[3] = Vector3{ +obb1.size.x, -obb1.size.y, +obb1.size.z };
	// top
	obb1Vetyex[4] = Vector3{ -obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vetyex[5] = Vector3{ +obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vetyex[6] = Vector3{ -obb1.size.x, +obb1.size.y, +obb1.size.z };
	obb1Vetyex[7] = Vector3{ +obb1.size.x, +obb1.size.y, +obb1.size.z };

	Matrix4x4 rotateMatrix1 = GetRotate(obb1);

	Vector3 obb2Vetyex[8]{};
	// bottom
	obb2Vetyex[0] = Vector3{ -obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vetyex[1] = Vector3{ +obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vetyex[2] = Vector3{ -obb2.size.x, -obb2.size.y, +obb2.size.z };
	obb2Vetyex[3] = Vector3{ +obb2.size.x, -obb2.size.y, +obb2.size.z };
	// top
	obb2Vetyex[4] = Vector3{ -obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vetyex[5] = Vector3{ +obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vetyex[6] = Vector3{ -obb2.size.x, +obb2.size.y, +obb2.size.z };
	obb2Vetyex[7] = Vector3{ +obb2.size.x, +obb2.size.y, +obb2.size.z };

	Matrix4x4 rotateMatrix2 = GetRotate(obb2);

	for (int index = 0; index < 8; index++) {
		obb1Vetyex[index] = matrix.Transform(obb1Vetyex[index], rotateMatrix1);
		obb1Vetyex[index] = (obb1Vetyex[index] + obb1.center);
		obb2Vetyex[index] = matrix.Transform(obb2Vetyex[index], rotateMatrix2);
		obb2Vetyex[index] = (obb2Vetyex[index] + obb2.center);
	}

	for (axisNum = 0; axisNum < 15; axisNum++) {
		float projectionPoint1[8];
		float projectionPoint2[8];
		float min1, max1;
		float min2, max2;
		min1 = vector.Dot(obb1Vetyex[0], vector.Normalize(separatingAxis[axisNum]));
		min2 = vector.Dot(obb2Vetyex[0], vector.Normalize(separatingAxis[axisNum]));
		max1 = min1;
		max2 = min2;
		for (int index = 0; index < 8; index++) {
			projectionPoint1[index] =
				vector.Dot(obb1Vetyex[index], vector.Normalize(separatingAxis[axisNum]));
			projectionPoint2[index] =
				vector.Dot(obb2Vetyex[index], vector.Normalize(separatingAxis[axisNum]));
			if (index == 0) {
				min1 = projectionPoint1[index];
				min2 = projectionPoint2[index];
				max1 = min1;
				max2 = min2;
			}
			else {
				min1 = min(min1, projectionPoint1[index]);
				min2 = min(min2, projectionPoint2[index]);
				max1 = max(max1, projectionPoint1[index]);
				max2 = max(max2, projectionPoint2[index]);
			}
		}
		float L1 = max1 - min1;
		float L2 = max2 - min2;
		float sumSpan = L1 + L2;
		float longSpan = max(max1, max2) - min(min1, min2);
		if (sumSpan < longSpan) {
			return false;
		}
	}
	return true;
}
