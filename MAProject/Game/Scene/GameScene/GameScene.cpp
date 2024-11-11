#include "GameScene.h"
#include"Matrix.h"

#include <cassert>
void GameScene::TextureLoad() {
	textureManager_->Load("resources/texture/rock.png");
	textureManager_->SetDissolveTex(textureManager_->Load("resources/texture/PostEffect/noise0.png"));
	textureManager_->Load("resources/texture/uvChecker.png");
	textureManager_->Load("resources/texture/Floor.png");
	textureManager_->Load("resources/texture/Road.png");
	//textureManager_->Load("resources/texture/Sky.png");
	textureManager_->Load("resources/Model/Enemy/EnemyTex.png");
	textureManager_->Load("resources/Model/EnemyParts/EnemyParts.png");
	textureManager_->Load("resources/Model/Weapon/Sword.png");
	textureManager_->Load("resources/texture/Magic.png");
	textureManager_->Load("resources/texture/Black.png");
	textureManager_->Load("resources/texture/circle.png");
	textureManager_->Load("resources/texture/monsterBall.png");
	textureManager_->Load("resources/texture/title.png");
	textureManager_->Load("resources/texture/pressA.png");
	textureManager_->Load("resources/texture/Clear.png");
	textureManager_->Load("resources/texture/Whitex64.png");
	textureManager_->Load("resources/texture/STAttack.png");
	textureManager_->Load("resources/texture/dash.png");
	textureManager_->Load("resources/texture/RB.png");
	textureManager_->Load("resources/texture/actionText.png");
	textureManager_->Load("resources/DDS/rostock_laage_airport_4k.dds");
}

void GameScene::SoundLoad(){
	titleBGM = audio_->LoadAudio("Game3.mp3");
	gameBGM = audio_->LoadAudio("Game3.mp3");
}

void GameScene::SpriteInitialize(){
	uint32_t textureHandle = 0;

	comboSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/combo.png");
	comboSprite_->Initialize(textureHandle);

	controlSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/control.png");
	controlSprite_->Initialize(textureHandle);
	controlSprite_->isDraw_ = false;
	//////ここから修正
	backSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Whitex64.png");
	backSprite_->Initialize(textureHandle);
	backSprite_->isDraw_ = false;

	pressSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/pressA.png");
	pressSprite_->Initialize(textureHandle);

	clearSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Clear.png");
	clearSprite_->Initialize(textureHandle);

	fadeSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/Black.png");
	fadeSprite_->Initialize(textureHandle);

	actionTextSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/actionText.png");
	actionTextSprite_->Initialize(textureHandle);

	attackSprite_ = std::make_unique<Sprite>();
	textureHandle = textureManager_->Load("resources/texture/STAttack.png");
	attackSprite_->Initialize(textureHandle);
}

void GameScene::ObjectInitialize() {
	

	skyBox_ = std::make_unique<SkyBox>();
	skyBox_->Initialize("resources/DDS/rostock_laage_airport_4k.dds");

	skyBox_->transform_.scale = { 1000.0f,1000.0f,1000.0f };

	stageObject_->Initialize();

}

void GameScene::Initialize(){
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	postEffect_ = PostEffect::GetInstance();
	postEffect_->SetPostEffect(PostEffect::EffectType::None);
	
	TextureLoad();
	SoundLoad();
	audio_->ResumeWave(titleBGM);
	/*audio_->PlayAudio(gameBGM, 0.1f, true);
	audio_->PauseWave(gameBGM);*/

	SpriteInitialize();
	ObjectInitialize();

	floorManager_ = std::make_unique<FloorManager>();
	floorManager_->Initialize();

	firstFloor_ = LevelLoader::GetInstance()->GetLevelObjectTransform("Cube");

	floorManager_->AddFloor(firstFloor_, false);

	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->Update();

	enemysPos_ = {
		Vector3(0.0f,1.0f,20.0f),
		Vector3(-5.0f,1.0f,25.0f),
		Vector3(5.0f,1.0f,25.0f),
		Vector3(-10.0f,1.0f,30.0f),
		Vector3(0.0f,1.0f,30.0f),
		Vector3(10.0f,1.0f,30.0f),
		Vector3(-15.0f,1.0f,35.0f),
		Vector3(-5.0f,1.0f,35.0f),
		Vector3(5.0f,1.0f,35.0f),
		Vector3(15.0f,1.0f,-35.0f),
		Vector3(0.0f,1.0f,-20.0f),
		Vector3(-5.0f,1.0f,-25.0f),
		Vector3(5.0f,1.0f,-25.0f),
		Vector3(-10.0f,1.0f,-30.0f),
		Vector3(0.0f,1.0f,-30.0f),
		Vector3(10.0f,1.0f,-30.0f),
		Vector3(-15.0f,1.0f,-35.0f),
		Vector3(-5.0f,1.0f,-35.0f),
		Vector3(5.0f,1.0f,-35.0f),
		Vector3(15.0f,1.0f,-35.0f)
	};

	for (size_t i = 0; i < enemyNum_; i++) {
		enemy_ = std::make_unique<Enemy>();
		enemy_->Initialize(enemysPos_[i]);
		enemy_->SetTarget(&player_->GetTransform());
		enemy_->SetTargetMat(&player_->GetRotateMatrix());
		enemy_->Update();
		enemies_.push_back(std::move(enemy_));

	}
	

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	//自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTargetMatrix(&player_->GetRotateMatrix());
	followCamera_->SetTarget(&player_->GetTransform());
	
	player_->SetViewProjection(&followCamera_->GetViewProjection());

	stages_ = {
		"Stage1",
		"Stage2",
		"Stage3",
		"Stage4"
	};

	stageName_ = stages_[0].c_str();

	comboSprite_->position_ = { 640.0f,550.0f };
	comboSprite_->anchorPoint_ = { 0.5f,0.5f };
	comboSprite_->color_ = { 0.0f,0.0f,0.0f,1.0f };
	comboSprite_->isDraw_ = true;

	controlSprite_->position_ = { 640.0f,360.0f };
	controlSprite_->anchorPoint_ = { 0.5f,0.5f };
	controlSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };

	backSprite_->position_ = { 640.0f,360.0f };
	backSprite_->anchorPoint_ = { 0.5f,0.5f };
	backSprite_->scale_.x = 1280.0f;
	backSprite_->scale_.y = 720.0f;
	backSprite_->color_ = {0.0f,0.0f,0.0f,0.85f };

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

	sceneNum_ = SceneName::GAME;
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	followCamera_->SetLockOn(lockOn_.get());
	player_->SetLockOn(lockOn_.get());
	//player_->Update();

}

void GameScene::Update(){
	DrawImgui();
	
	followCamera_->Update();
	postEffect_->SetMatProjectionInverse(followCamera_->GetProjectionInverse());
		
	followCamera_->SetIsMove(true);
		
		
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
	if (player_->GetHitTimer() != 0 and postEffect_->GetEffectType() == PostEffect::EffectType::None) {
		postEffect_->SetPostEffect(PostEffect::EffectType::Smoothing9x9);
	}
	else if (postEffect_->GetEffectType() == PostEffect::EffectType::Smoothing9x9 and player_->GetHitTimer() == 0) {
		postEffect_->SetPostEffect(PostEffect::EffectType::None);
	}
		

	if (input_->Trigerkey(DIK_C)&&input_->Trigerkey(DIK_L)){
		isReset_ = true;
		sceneNum_ = SceneName::CLEAR;
	}

	AllCollision();


	floorManager_->Update();
	fadeSprite_->color_.w = fadeAlpha_;
	

	if (fadeAlpha_>=1.0f){
		fadeAlpha_ = 1.0f;
	}
	else if (fadeAlpha_ <= 0.0f) {
		fadeAlpha_ = 0.0f;
	}
}

void GameScene::Debug(){

}

void GameScene::DrawParticle(){
	textureManager_->PreDrawParticle();


	player_->ParticleDraw(followCamera_->GetViewProjection());
	for (const auto& enemy : enemies_) {
		enemy->ParticleDraw(followCamera_->GetViewProjection(), player_->GetTrailColor());
	}

	textureManager_->PostDrawParticle();
}

void GameScene::DrawSkin3D(){
	textureManager_->PreDrawSkin3D();
	player_->SkinningDraw(followCamera_->GetViewProjection());

}

void GameScene::Draw3D(){

	/*描画前処理*/
	textureManager_->PreDrawMapping3D();

	///*ここから下に描画処理を書き込む*/
	player_->Draw(followCamera_->GetViewProjection());

	for (const auto& enemy : enemies_) {
		enemy->Draw(followCamera_->GetViewProjection());
	}

	lockOn_->Draw();
	floorManager_->Draw(followCamera_->GetViewProjection());

	textureManager_->PreDraw3D();
	stageObject_->Draw(followCamera_->GetViewProjection());
	
	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw3D();

	//textureManager_->PreDrawSkyBox();

	//skyBox_->Update(followCamera_->GetViewProjection());
	//skyBox_->Draw();

	textureManager_->PreDrawWorld2D();

	player_->TexDraw(followCamera_->GetViewProjection().matViewProjection_);

	for (const auto& enemy : enemies_) {
		enemy->TexDraw(followCamera_->GetViewProjection().matViewProjection_);
	}


	
}

void GameScene::Draw2D(){
	/*描画前処理*/
	textureManager_->PreDraw2D();
	///*ここから下に描画処理を書き込む*/
	actionTextSprite_->Draw();
	attackSprite_->Draw();
	fadeSprite_->Draw();
		
	//testTexture_->Draw(textureManager_->SendGPUDescriptorHandle(0));
	
	/*描画処理はここまで*/
	/*描画後処理*/
	textureManager_->PostDraw2D();
}

void GameScene::Finalize(){
	
}

void GameScene::AllDraw3D(){
	DrawSkin3D();
	Draw3D();
	DrawParticle();
}

void GameScene::AllDraw2D(){
	Draw2D();
}

void GameScene::DrawImgui(){
#ifdef _DEBUG

	player_->DrawImgui();
	followCamera_->DrawImgui();
	lockOn_->DrawImgui();
	floorManager_->DrawImgui();

	for (const auto& enemy : enemies_) {
		enemy->DrawImgui();
	}
		
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

#endif // _DEBUG	
}



void GameScene::AllCollision(){
	for (const auto& floor : floorManager_->GetFloors()) {
		if (IsCollisionOBBOBB(floor->GetOBB(), player_->GetOBB())) {
			chackCollision = 1;
			player_->OnFlootCollision(floor->GetOBB());
			player_->SetIsDown(false);
			break;
		}
		else {
			chackCollision = 0;
			player_->SetIsDown(true);
		}
	}

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

		//if (enemy->GetIsDead()) {
		//	
		//	//audio_->PauseWave(gameBGM);
		//	isReset_ = true;
		//	sceneNum_ = SceneName::CLEAR;
		//}

		if (IsCollisionOBBOBB(player_->GetOBB(), enemy->GetBodyOBB())) {
			player_->SetCollisionEnemy(true);
			break;
		}
		else {
			player_->SetCollisionEnemy(false);
		}

		if (IsCollisionOBBOBB(player_->GetOBB(), enemy->GetAttackOBB())&& !player_->GetIsHitEnemyAttack()) {
			player_->OnCollisionEnemyAttack();

			break;
		}
		
	}

	


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
				min1 = std::min(min1, projectionPoint1[index]);
				min2 = std::min(min2, projectionPoint2[index]);
				max1 = max(max1, projectionPoint1[index]);
				max2 = max(max2, projectionPoint2[index]);
			}
		}
		float L1 = max1 - min1;
		float L2 = max2 - min2;
		float sumSpan = L1 + L2;
		float longSpan = max(max1, max2) - std::min(min1, min2);
		if (sumSpan < longSpan) {
			return false;
		}
	}
	return true;
}
