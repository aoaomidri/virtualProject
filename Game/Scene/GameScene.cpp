#include "GameScene.h"
#include"Matrix.h"
#include <cassert>
void GameScene::TextureLoad() {
	textureManager_->Load("resources/texture/uvChecker.png");//0
	textureManager_->Load("resources/texture/rock.png");
	textureManager_->Load("resources/texture/Floor.png");
	textureManager_->Load("resources/texture/Road.png");
	textureManager_->Load("resources/texture/Sky.png");
	textureManager_->Load("resources/Enemy/EnemyTex.png");//5
	textureManager_->Load("resources/EnemyParts/EnemyParts.png");
	textureManager_->Load("resources/Weapon/Sword.png");
	textureManager_->Load("resources/texture/Magic.png");
	textureManager_->Load("resources/texture/Black.png");
	textureManager_->Load("resources/texture/circle.png");//10
	textureManager_->Load("resources/texture/monsterBall.png");
	textureManager_->Load("resources/texture/title.png");
	textureManager_->Load("resources/texture/pressA.png");
	textureManager_->Load("resources/texture/Clear.png");
	textureManager_->Load("resources/texture/Whitex64.png");//15
	textureManager_->Load("resources/skyDome/skyDome.png");
	textureManager_->Load("resources/texture/STAttack.png");
	textureManager_->Load("resources/texture/dash.png");
	textureManager_->Load("resources/texture/RB.png");
	textureManager_->Load("resources/texture/actionText.png");//20
	textureManager_->Load("resources/texture/LockOn.png");//21
}

void GameScene::SoundLoad(){
	titleBGM = audio_->SoundLoadWave("honobono.wav");
	gameBGM = audio_->SoundLoadWave("Game3.wav");
}

void GameScene::SpriteInitialize(DirectXCommon* dxCommon_){
	titleSprite_ = std::make_unique<Sprite>(textureManager_.get());
	titleSprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), 12);

	pressSprite_ = std::make_unique<Sprite>(textureManager_.get());
	pressSprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), 13);

	clearSprite_ = std::make_unique<Sprite>(textureManager_.get());
	clearSprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), 14);

	fadeSprite_ = std::make_unique<Sprite>(textureManager_.get());
	fadeSprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), 15);

	actionTextSprite_ = std::make_unique<Sprite>(textureManager_.get());
	actionTextSprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), 20);

	attackSprite_ = std::make_unique<Sprite>(textureManager_.get());
	attackSprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), 17);
}

void GameScene::ObjectInitialize(DirectXCommon* dxCommon_) {
	//particle_ = std::make_unique<ParticleBase>();
	//particle_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());
	/*obj_ = std::make_unique<Object3D>();
	obj_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());*/
	//model_ = Model::LoadObjFile("skyDome");
	//boxModel_ = Model::LoadObjFile("box");

	skyDomeObj_ = std::make_unique<Object3D>();
	skyDomeObj_->Initialize();

	skyDomeModel_ = Model::LoadObjFile("skyDome");

	skyDomeObj_->SetModel(skyDomeModel_);

	skyDomeTrnasform_ = {
		{100.0f,100.0f,100.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

}

void GameScene::Initialize(DirectXCommon* dxCommon_){
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize();
	TextureLoad();
	SoundLoad();
	audio_->SoundPlayWave(titleBGM, 0.5f);
	audio_->SoundPlayWave(gameBGM, 0.5f);
	

	SpriteInitialize(dxCommon_);
	ObjectInitialize(dxCommon_);

	floorManager_ = std::make_unique<FloorManager>();
	floorManager_->Initialize();

	floorManager_->AddFloor(firstFloor_, false);

	player_ = std::make_unique<Player>();
	player_->Initialize();

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize({ 0,1.0f,20.0f });
	enemies_.push_back(std::move(enemy_));

	textureManager_->MakeInstancingShaderResourceView(player_->GetParticle()->GetInstancingResource());

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

	titleSprite_->position_ = { 640.0f,175.0f };
	titleSprite_->scale_.x = 850.0f;
	titleSprite_->scale_.y = 150.0f;
	titleSprite_->anchorPoint_ = { 0.5f,0.5f };
	titleSprite_->color_ = { 0.0f,0.0f,0.0f,1.0f };

	pressSprite_->position_ = { 640.0f,500.0f };
	pressSprite_->scale_.x = 600.0f;
	pressSprite_->scale_.y = 136.0f;
	pressSprite_->anchorPoint_ = { 0.5f,0.5f };
	pressSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	actionTextSprite_->position_ = { 1072.0f,500.0f };
	actionTextSprite_->anchorPoint_ = { 0.5f,0.5f };
	actionTextSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	attackSprite_->position_ = { 1072.0f,650.0f };
	attackSprite_->anchorPoint_ = { 0.5f,0.5f };
	attackSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	clearSprite_->position_ = { 640.0f,175.0f };
	clearSprite_->scale_.x = 850.0f;
	clearSprite_->scale_.y = 150.0f;
	clearSprite_->anchorPoint_ = { 0.5f,0.5f };

	fadeSprite_->position_ = { 640.0f,360.0f };
	fadeSprite_->scale_.x = 1280.0f;
	fadeSprite_->scale_.y = 720.0f;
	fadeSprite_->color_ = { 0.0f,0.0f,0.0f,fadeAlpha_ };
	fadeSprite_->anchorPoint_ = { 0.5f,0.5f };

	sceneNum_ = SceneName::TITLE;
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), textureManager_.get());

	followCamera_->SetLockOn(lockOn_.get());
	player_->SetLockOn(lockOn_.get());
	//audio_->SoundPlayWave(soundData1);
}

void GameScene::Update(){
	DrawImgui();
	
	followCamera_->Update();
	switch (sceneNum_){
	case SceneName::TITLE:
		
		titleSprite_->Update();
		pressSprite_->Update();
		fadeSprite_->Update();
		audio_->PauseWave(gameBGM);
		followCamera_->SetIsMove(false);
		for (const auto& enemy : enemies_) {
			enemy->Update();
		}

		if (fadeAlpha_ <= 0.0f) {
			if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A)) {
				isFade_ = true;
			}
		}
		if (isFade_) {
			fadeAlpha_ += 0.01f;
		}
		else{
			fadeAlpha_ -= 0.01f;
		}
		if (fadeAlpha_ >= 1.0f&&isFade_) {
			followCamera_->RotateReset();
			player_->Update();
			player_->Respawn();
			for (const auto& enemy : enemies_) {
				enemy->Respawn({ 0, 2.0f, 20.0f });
			}
			audio_->PauseWave(titleBGM);
			audio_->RePlayWave(gameBGM);
			sceneNum_ = SceneName::GAME;
		}
		break;
	case SceneName::GAME:
		/*if (input_->Trigerkey(DIK_1)){
			sceneNum_ = SceneName::CLEAR;
		}*/
		followCamera_->SetIsMove(true);
		fadeSprite_->Update();
		actionTextSprite_->Update();
		attackSprite_->Update();
		fadeAlpha_ -= 0.01f;
		if (fadeAlpha_<=0.0f){
			
			
			isFade_ = false;
			fadeAlpha_ = 0.0f;
			lockOn_->Update(enemies_, followCamera_->GetViewProjection(), input_, followCamera_->GetLockViewingFrustum());

			player_->Update();
			for (const auto& enemy : enemies_) {
				enemy->Update();
			}
		}

		if (input_->Trigerkey(DIK_C)&&input_->Trigerkey(DIK_L)){
			sceneNum_ = SceneName::CLEAR;
		}

		AllCollision();
		//particle_->Update(player_->GetTransform(), followCamera_->GetViewProjection());

		floorManager_->Update();
		break;
	case SceneName::CLEAR:
		followCamera_->SetIsMove(false);
		audio_->PauseWave(gameBGM);
		clearSprite_->Update();
		for (const auto& enemy : enemies_) {
			enemy->Respawn({ 0, 2.0f, 20.0f });
		}

		fadeSprite_->Update();
		if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A)) {
			isFade_ = true;
		}
		if (isFade_) {
			fadeAlpha_ += 0.01f;
		}
		if (fadeAlpha_ >= 1.0f) {
			isFade_ = false;
			followCamera_->RotateReset();
			audio_->RePlayWave(titleBGM);
			sceneNum_ = SceneName::TITLE;
		}

		player_->Respawn();
		
		break;
	default:
		assert(0);
	}
	fadeSprite_->color_.w = fadeAlpha_;
	skyDomeTrnasform_.rotate.y += 0.001f;
	skyDomeMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(skyDomeTrnasform_);

	if (fadeAlpha_>=1.0f){
		fadeAlpha_ = 1.0f;
	}
	else if (fadeAlpha_ <= 0.0f) {
		fadeAlpha_ = 0.0f;
	}
}

void GameScene::AudioDataUnLoad(){
	
	
}

void GameScene::DrawParticle(){
	textureManager_->PreDrawParticle();

	switch (sceneNum_) {
	case SceneName::TITLE:
		break;
	case SceneName::GAME:
		player_->ParticleDraw(textureManager_.get(), followCamera_->GetViewProjection());
		break;
	case SceneName::CLEAR:

		break;
	default:
		assert(0);
	}

	textureManager_->PostDrawParticle();
}

void GameScene::Draw3D(){
	/*描画前処理*/
	textureManager_->PreDraw3D();
	skyDomeObj_->Update(skyDomeMatrix_, followCamera_->GetViewProjection());
	skyDomeObj_->Draw(textureManager_->SendGPUDescriptorHandle(16));

	/*ここから下に描画処理を書き込む*/
	switch (sceneNum_) {
	case SceneName::TITLE:
		
		break;
	case SceneName::GAME:
		floorManager_->Draw(textureManager_.get(), followCamera_->GetViewProjection());

		player_->Draw(textureManager_.get(), followCamera_->GetViewProjection());

		for (const auto& enemy : enemies_) {
			enemy->Draw(textureManager_.get(), followCamera_->GetViewProjection());
		}
		break;
	case SceneName::CLEAR:
		
		break;
	default:
		assert(0);
	}
	
	
	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw3D();
}

void GameScene::Draw2D(){
	/*描画前処理*/
	textureManager_->PreDraw2D();
	///*ここから下に描画処理を書き込む*/
	switch (sceneNum_) {
	case SceneName::TITLE:
		titleSprite_->Draw(textureManager_->SendGPUDescriptorHandle(12));
		pressSprite_->Draw(textureManager_->SendGPUDescriptorHandle(13));
		fadeSprite_->Draw(textureManager_->SendGPUDescriptorHandle(15));
		break;
	case SceneName::GAME:
		actionTextSprite_->Draw(textureManager_->SendGPUDescriptorHandle(20));
		attackSprite_->Draw(textureManager_->SendGPUDescriptorHandle(17));
		fadeSprite_->Draw(textureManager_->SendGPUDescriptorHandle(15));
		lockOn_->Draw(textureManager_.get());
		break;
	case SceneName::CLEAR:
		clearSprite_->Draw(textureManager_->SendGPUDescriptorHandle(14));
		pressSprite_->Draw(textureManager_->SendGPUDescriptorHandle(13));
		fadeSprite_->Draw(textureManager_->SendGPUDescriptorHandle(15));
		break;
	default:
		assert(0);
	}
	//testTexture_->Draw(textureManager_->SendGPUDescriptorHandle(0));
	
	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw2D();
}

void GameScene::Finalize(){
	textureManager_->Finalize();
	delete skyDomeModel_;
}

void GameScene::DrawImgui(){
#ifdef _DEBUG
	switch (sceneNum_) {
	case SceneName::TITLE:
	ImGui::Begin("BGM関連");
	if (ImGui::Button("音源の復活")){
		gameBGM = audio_->SoundPlayWave(gameBGM, 0.5f);
	}
	if (ImGui::Button("最初から再生")) {
		audio_->RePlayWave(gameBGM);
	}
	if (ImGui::Button("完全に停止")) {
		audio_->StopWave(gameBGM);
	}
	if (ImGui::Button("一時停止")){
		audio_->PauseWave(gameBGM);
	}
	if (ImGui::Button("停止解除")) {
		audio_->ResumeWave(gameBGM);
	}
	ImGui::End();
		break;
	case SceneName::GAME:
		player_->DrawImgui();
		followCamera_->DrawImgui();
		lockOn_->DrawImgui();

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
		break;
	case SceneName::CLEAR:
		break;
	default:
		assert(0);
	}

	ImGui::Begin("スプライト");
	ImGui::DragFloat2("action : ポジション", &attackSprite_->position_.x, 1.0f);
	ImGui::DragFloat2("action : 大きさ", &attackSprite_->scale_.x, 1.0f);
	ImGui::DragFloat4("action : 色", &attackSprite_->color_.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat2("press : ポジション", &actionTextSprite_->position_.x, 1.0f);
	ImGui::DragFloat2("press : 大きさ", &actionTextSprite_->scale_.x, 1.0f);
	ImGui::DragFloat4("press : 色", &actionTextSprite_->color_.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat2("fade : ポジション", &fadeSprite_->position_.x, 1.0f);
	ImGui::DragFloat2("fade : 大きさ", &fadeSprite_->scale_.x, 1.0f);
	ImGui::DragFloat4("fade : 色", &fadeSprite_->color_.x, 0.01f, 0.0f, 1.0f);
	ImGui::End();
	//particle_->DrawImgui("ステージパーティクル");
	


#endif // _DEBUG	
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
	for (const auto& enemy : enemies_) {
		if (IsCollisionOBBOBB(player_->GetWeaponOBB(), enemy->GetOBB())) {
			uint32_t serialNumber = enemy->GetSerialNumber();
			if (player_->RecordCheck(serialNumber)){
				return;
			}
			//接触履歴に登録
			player_->AddRecord(serialNumber);

			enemy->OnCollision();
		}

		if (enemy->GetIsDead()) {
			audio_->PauseWave(gameBGM);
			sceneNum_ = SceneName::CLEAR;
		}
	}

	for (const auto& enemy : enemies_) {
		if (IsCollisionOBBOBB(player_->GetOBB(), enemy->GetOBB())) {
			player_->SetCollisionEnemy(true);
			break;
		}
		else {
			player_->SetCollisionEnemy(false);
		}
	}
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
	Vector3 vector{};
	Matrix matrix;

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

	Vector3 obb1Vertex[8]{};
	// bottom
	obb1Vertex[0] = Vector3{ -obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vertex[1] = Vector3{ +obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vertex[2] = Vector3{ -obb1.size.x, -obb1.size.y, +obb1.size.z };
	obb1Vertex[3] = Vector3{ +obb1.size.x, -obb1.size.y, +obb1.size.z };
	// top
	obb1Vertex[4] = Vector3{ -obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vertex[5] = Vector3{ +obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vertex[6] = Vector3{ -obb1.size.x, +obb1.size.y, +obb1.size.z };
	obb1Vertex[7] = Vector3{ +obb1.size.x, +obb1.size.y, +obb1.size.z };

	Matrix4x4 rotateMatrix1 = GetRotate(obb1);

	Vector3 obb2Vertex[8]{};
	// bottom
	obb2Vertex[0] = Vector3{ -obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vertex[1] = Vector3{ +obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vertex[2] = Vector3{ -obb2.size.x, -obb2.size.y, +obb2.size.z };
	obb2Vertex[3] = Vector3{ +obb2.size.x, -obb2.size.y, +obb2.size.z };
	// top
	obb2Vertex[4] = Vector3{ -obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vertex[5] = Vector3{ +obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vertex[6] = Vector3{ -obb2.size.x, +obb2.size.y, +obb2.size.z };
	obb2Vertex[7] = Vector3{ +obb2.size.x, +obb2.size.y, +obb2.size.z };

	Matrix4x4 rotateMatrix2 = GetRotate(obb2);

	for (int index = 0; index < 8; index++) {
		obb1Vertex[index] = matrix.TransformVec(obb1Vertex[index], rotateMatrix1);
		obb1Vertex[index] = (obb1Vertex[index] + obb1.center);
		obb2Vertex[index] = matrix.TransformVec(obb2Vertex[index], rotateMatrix2);
		obb2Vertex[index] = (obb2Vertex[index] + obb2.center);
	}

	for (axisNum = 0; axisNum < 15; axisNum++) {
		float projectionPoint1[8]{};
		float projectionPoint2[8]{};
		float min1, max1;
		float min2, max2;
		min1 = vector.Dot(obb1Vertex[0], vector.Normalize(separatingAxis[axisNum]));
		min2 = vector.Dot(obb2Vertex[0], vector.Normalize(separatingAxis[axisNum]));
		max1 = min1;
		max2 = min2;
		for (int index = 0; index < 8; index++) {
			projectionPoint1[index] =
				vector.Dot(obb1Vertex[index], vector.Normalize(separatingAxis[axisNum]));
			projectionPoint2[index] =
				vector.Dot(obb2Vertex[index], vector.Normalize(separatingAxis[axisNum]));
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
