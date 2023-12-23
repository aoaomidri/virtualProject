#include "GameScene.h"
#include"../../math/Matrix.h"
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
	textureManager_->Load("resources/Magic.png", 8);
	textureManager_->Load("resources/Black.png", 9);
	textureManager_->Load("resources/circle.png", 10);
	textureManager_->Load("resources/monsterBall.png", 11);
}

void GameScene::SoundLoad(){
	soundData1 = audio_->SoundLoadWave("Alarm01.wav");
}

void GameScene::ObjectInitialize(DirectXCommon* dxCommon_){
	/*particle_ = std::make_unique<ParticleBase>();
	particle_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());*/
	//obj_ = std::make_unique<Object3D>();
	//obj_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());
	//model_ = Model::LoadObjFile("skyDome");
	//boxModel_ = Model::LoadObjFile("box");
	
	
	
}

void GameScene::Initialize(DirectXCommon* dxCommon_){
	audio_ = Audio::GetInstance();

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(dxCommon_->GetDevice(),
		dxCommon_->GetCommandList(), dxCommon_->GetSRVHeap());
	TextureLoad();
	SoundLoad();

	ObjectInitialize(dxCommon_);

	floorManager_ = std::make_unique<FloorManager>();
	floorManager_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());

	floorManager_->AddFloor(firstFloor_, false);

	player_ = std::make_unique<Player>();
	player_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());

	//textureManager_->MakeInstancingShaderResourceView(particle_->GetInstancingResource());

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	//自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetTransform());
	followCamera_->SetTargetMatrix(&player_->GetRotateMatrix());
	
	player_->SetViewProjection(&followCamera_->GetViewProjection());

	stages_ = {
		"Stage1",
		"Stage2",
		"Stage3",
		"Stage4"
	};

	stageName_ = stages_[0].c_str();
	floorManager_->LoadFiles(stageName_);

	//audio_->SoundPlayWave(soundData1);
}

void GameScene::Update(Input* input_){
	DrawImgui();
	followCamera_->Update(input_);
	player_->Update(input_);

	AllCollision();
	//particle_->Update(particleTrnadform_, followCamera_->GetViewProjection());

	floorManager_->Update();
	
}

void GameScene::AudioDataUnLoad(){
	audio_->SoundUnload(&soundData1);
	
	
	
}

void GameScene::DrawParticle(){
	textureManager_->PreDrawParticle();

	//particle_->Draw(textureManager_->SendGPUDescriptorHandle(8), textureManager_->SendInstancingGPUDescriptorHandle());

	textureManager_->PostDrawParticle();
}

void GameScene::Draw3D(){
	/*描画前処理*/
	textureManager_->PreDraw3D();
	/*ここから下に描画処理を書き込む*/
	floorManager_->Draw(textureManager_.get(), followCamera_->GetViewProjection());

	player_->Draw(textureManager_.get(), followCamera_->GetViewProjection());
	
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

void GameScene::Finalize(){
}

void GameScene::DrawImgui(){
#ifdef _DEBUG
	player_->DrawImgui();
	//particle_->DrawImgui();
	ImGui::Begin("ステージ関連", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("オブジェクトの生成")) {
			if (ImGui::TreeNode("床生成")) {
				ImGui::DragFloat3("床の大きさ", &firstFloor_.scale.x, 0.01f);
				ImGui::DragFloat3("床の回転", &firstFloor_.rotate.x, 0.01f);
				ImGui::DragFloat3("床の座標", &firstFloor_.translate.x, 0.1f);

				ImGui::Checkbox("動く床にする", &isFloorMove_);

				if (ImGui::Button("床の追加")) {
					floorManager_->AddFloor(firstFloor_, isFloorMove_);
				}
				ImGui::TreePop();
			}


			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("オブジェクト一覧")) {
			if (ImGui::BeginMenu("床一覧")) {
				floorManager_->DrawImgui();
				ImGui::EndMenu();
			}


			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("ファイル関連")) {
			for (size_t i = 0; i < stages_.size(); i++) {
				if (ImGui::RadioButton(stages_[i].c_str(), &stageSelect_, static_cast<int>(i))) {
					stageName_ = stages_[stageSelect_].c_str();
				}

			}
			if (ImGui::Button("jsonファイルを作る")) {
				FilesSave(stages_);
			}
			if (ImGui::Button("上書きセーブ")) {
				FilesOverWrite(stageName_);
			}

			if (ImGui::Button("全ロード(手動)")) {
				FilesLoad(stageName_);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
	ImGui::End();


#endif // DEBUG	
}



void GameScene::AllCollision(){
	for (Floor* floor : floorManager_->GetFloors()) {
		if (IsCollisionOBBOBB(floor->GetOBB(), player_->GetOBB())) {
			chackCollision = 1;
			player_->onFlootCollision(floor->GetOBB());
			player_->SetIsDown(false);
			break;
		}
		else {
			chackCollision = 0;
			player_->SetIsDown(true);
		}
	}

	//if (player_->GetIsRespawn()) {
	//	int i = 0;
	//	for (const auto& enemy : enemies_) {
	//		enemy->Respawn({ 0.0f,0.7f,(7.0f + i * 4.0f) });
	//		i++;
	//	}
	//	player_->SetIsRespawn(false);
	//}


	//for (const auto& enemy : enemies_) {

	//	if (IsCollisionOBBOBB(player_->GetOBB(), enemy->GetOBB()) && !enemy->GetIsDead()) {
	//		int i = 0;
	//		player_->Respawn();
	//		for (const auto& enemy : enemies_) {
	//			enemy->Respawn({ 0.0f,0.7f,(7.0f + i * 4.0f) });
	//			i++;
	//		}
	//	}
	//}
	//for (const auto& enemy : enemies_) {
	//	if (IsCollisionOBBOBB(player_->GetWeaponOBB(), enemy->GetOBB())) {
	//		enemy->OnCollision();
	//	}
	//}

}

void GameScene::FilesSave(const std::vector<std::string>& stages) {
	floorManager_->SaveFile(stages);
	std::string message = std::format("{}.json created.", "all");
	MessageBoxA(nullptr, message.c_str(), "StagesObject", 0);
}

void GameScene::FilesOverWrite(const std::string& stage) {
	floorManager_->FileOverWrite(stage);
	std::string message = std::format("{}.json OverWrite.", "all");
	MessageBoxA(nullptr, message.c_str(), "StagesObject", 0);
}

void GameScene::FilesLoad(const std::string& stage) {
	floorManager_->LoadFiles(stage);
	std::string message = std::format("{}.json loaded.", "all");
	MessageBoxA(nullptr, message.c_str(), "StagesObject", 0);
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
		obb1Vetyex[index] = matrix.TransformVec(obb1Vetyex[index], rotateMatrix1);
		obb1Vetyex[index] = (obb1Vetyex[index] + obb1.center);
		obb2Vetyex[index] = matrix.TransformVec(obb2Vetyex[index], rotateMatrix2);
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
