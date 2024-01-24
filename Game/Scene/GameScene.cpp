#include "GameScene.h"
#include"Matrix.h"
#include <cassert>
void GameScene::TextureLoad() {
	textureManager_->Load("resources/texture/uvChecker.png");//0
	textureManager_->Load("resources/texture/rock.png");
	textureManager_->Load("resources/texture/Floor.png");
	textureManager_->Load("resources/texture/Road.png");
	textureManager_->Load("resources/texture/Sky.png");
	textureManager_->Load("resources/Enemy/EnemyTex.png");
	textureManager_->Load("resources/EnemyParts/EnemyParts.png");
	textureManager_->Load("resources/Weapon/Sword.png");
	textureManager_->Load("resources/texture/Magic.png");
	textureManager_->Load("resources/texture/Black.png");
	textureManager_->Load("resources/texture/circle.png");//10
	textureManager_->Load("resources/texture/monsterBall.png");
	textureManager_->Load("resources/texture/title.png");
	textureManager_->Load("resources/texture/Press.png");
	textureManager_->Load("resources/texture/Clear.png");
	textureManager_->Load("resources/texture/Whitex64.png");
}

void GameScene::SoundLoad(){
	titleBGM = audio_->SoundLoadWave("honobono.wav");
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
}

void GameScene::ObjectInitialize(DirectXCommon* dxCommon_){
	TestObj_ = std::make_unique<Object3D>();
	TestObj_->Initialize();

	TestModel_ = Model::LoadObjFile("sphere");

	TestObj_->SetModel(TestModel_);

	testTransform_.scale = { 1.0f,1.0f,1.0f };
	
}

void GameScene::Initialize(DirectXCommon* dxCommon_){
	audio_ = Audio::GetInstance();
	input_ = Input::GetInstance();

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize();

	TextureLoad();
	SoundLoad();

	//audio_->SoundPlayWave(titleBGM, 0.2f);

	SpriteInitialize(dxCommon_);
	ObjectInitialize(dxCommon_);


	//textureManager_->MakeInstancingShaderResourceView(player_->GetParticle()->GetInstancingResource());

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	//自キャラのワールドトランスフォームを追従カメラにセット
	
	titleSprite_->position_ = { 640.0f,175.0f };
	titleSprite_->scale_.x = 850.0f;
	titleSprite_->scale_.y = 150.0f;
	titleSprite_->anchorPoint_ = { 0.5f,0.5f };

	pressSprite_->position_ = { 640.0f,500.0f };
	pressSprite_->scale_.x = 600.0f;
	pressSprite_->scale_.y = 136.0f;
	pressSprite_->anchorPoint_ = { 0.5f,0.5f };

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

	followCamera_->SetTarget(&testTransform_);
	//lockOn_ = std::make_unique<LockOn>();
	//lockOn_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), textureManager_.get());

	//followCamera_->SetLockOn(lockOn_.get());
	//player_->SetLockOn(lockOn_.get());
	////audio_->SoundPlayWave(soundData1);
}

void GameScene::Update(){
	DrawImgui();
	testMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(testTransform_);

	switch (sceneNum_){
	case SceneName::TITLE:

		followCamera_->Update();
		
		

		/*titleSprite_->Update();
		pressSprite_->Update();
		fadeSprite_->Update();*/
		break;
	case SceneName::GAME:
		/*if (input_->Trigerkey(DIK_1)){
			sceneNum_ = SceneName::CLEAR;
		}*/
		fadeSprite_->Update();
		fadeAlpha_ -= 0.01f;
		if (fadeAlpha_<=0.0f){

		}
		

		
		//AllCollision();
		////particle_->Update(player_->GetTransform(), followCamera_->GetViewProjection());

		//floorManager_->Update();
		break;
	case SceneName::CLEAR:
		clearSprite_->Update();
		if (input_->GetPadButtonTriger(XINPUT_GAMEPAD_A)) {
			sceneNum_ = SceneName::TITLE;
		}
		
		fadeAlpha_ = 0.0f;
		isFade_ = false;

		break;
	default:
		assert(0);
	}
	fadeSprite_->color_.w = fadeAlpha_;
	
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
		//player_->ParticleDraw(textureManager_.get(), followCamera_->GetViewProjection());
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
	/*ここから下に描画処理を書き込む*/
	switch (sceneNum_) {
	case SceneName::TITLE:
		TestObj_->Update(testMatrix_, followCamera_->GetViewProjection());
		TestObj_->Draw(textureManager_->SendGPUDescriptorHandle(0));
		break;
	case SceneName::GAME:
		
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
		fadeSprite_->Draw(textureManager_->SendGPUDescriptorHandle(15));
		break;
	case SceneName::CLEAR:
		clearSprite_->Draw(textureManager_->SendGPUDescriptorHandle(14));
		pressSprite_->Draw(textureManager_->SendGPUDescriptorHandle(13));
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
	delete TestModel_;
	textureManager_->Finalize();
}

void GameScene::DrawImgui(){
#ifdef _DEBUG
	switch (sceneNum_) {
	case SceneName::TITLE:
	ImGui::Begin("BGM関連");
	if (ImGui::Button("音源の復活")){
		titleBGM = audio_->SoundPlayWave(titleBGM, 0.5f);
	}
	if (ImGui::Button("最初から再生")) {
		audio_->RePlayWave(titleBGM);
	}
	if (ImGui::Button("完全に停止")) {
		audio_->StopWave(titleBGM);
	}
	if (ImGui::Button("一時停止")){
		audio_->PauseWave(titleBGM);
	}
	if (ImGui::Button("停止解除")) {
		audio_->ResumeWave(titleBGM);
	}
	ImGui::End();

	ImGui::Begin("テストオブジェ");
	ImGui::DragFloat3("大きさ", &testTransform_.scale.x, 0.1f);
	ImGui::DragFloat3("回転", &testTransform_.rotate.x, 0.1f);
	ImGui::DragFloat3("座標", &testTransform_.translate.x, 0.1f);

	ImGui::End();
	TestObj_->DrawImgui();

	followCamera_->DrawImgui();
		break;
	case SceneName::GAME:
		
		break;
	case SceneName::CLEAR:
		break;
	default:
		assert(0);
	}

	ImGui::Begin("スプライト");
	ImGui::DragFloat2("title : ポジション", &titleSprite_->position_.x, 1.0f);
	ImGui::DragFloat2("title : 大きさ", &titleSprite_->scale_.x, 1.0f);
	ImGui::DragFloat4("title : 色", &titleSprite_->color_.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat2("press : ポジション", &pressSprite_->position_.x, 1.0f);
	ImGui::DragFloat2("press : 大きさ", &pressSprite_->scale_.x, 1.0f);
	ImGui::DragFloat4("press : 色", &pressSprite_->color_.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat2("fade : ポジション", &fadeSprite_->position_.x, 1.0f);
	ImGui::DragFloat2("fade : 大きさ", &fadeSprite_->scale_.x, 1.0f);
	ImGui::DragFloat4("fade : 色", &fadeSprite_->color_.x, 0.01f, 0.0f, 1.0f);
	ImGui::End();
	//particle_->DrawImgui("ステージパーティクル");
	


#endif // _DEBUG	
}



//void GameScene::AllCollision(){
//	for (Floor* floor : floorManager_->GetFloors()) {
//		if (IsCollisionOBBOBB(floor->GetOBB(), player_->GetOBB())) {
//			chackCollision = 1;
//			player_->onFlootCollision(floor->GetOBB());
//			player_->SetIsDown(false);
//			break;
//		}
//		else {
//			chackCollision = 0;
//			player_->SetIsDown(true);
//		}
//	}
//
//	//if (player_->GetIsRespawn()) {
//	//	int i = 0;
//	//	for (const auto& enemy : enemies_) {
//	//		enemy->Respawn({ 0.0f,0.7f,(7.0f + i * 4.0f) });
//	//		i++;
//	//	}
//	//	player_->SetIsRespawn(false);
//	//}
//
//
//	//for (const auto& enemy : enemies_) {
//
//	//	if (IsCollisionOBBOBB(player_->GetOBB(), enemy->GetOBB()) && !enemy->GetIsDead()) {
//	//		int i = 0;
//	//		player_->Respawn();
//	//		for (const auto& enemy : enemies_) {
//	//			enemy->Respawn({ 0.0f,0.7f,(7.0f + i * 4.0f) });
//	//			i++;
//	//		}
//	//	}
//	//}
//	for (const auto& enemy : enemies_) {
//		if (IsCollisionOBBOBB(player_->GetWeaponOBB(), enemy->GetOBB())) {
//			enemy->OnCollision();
//		}
//
//		if (enemy->GetIsDead()) {
//			sceneNum_ = SceneName::CLEAR;
//		}
//	}
//	//}
//
//}
//
//void GameScene::FilesSave(const std::vector<std::string>& stages) {
//	floorManager_->SaveFile(stages);
//	std::string message = std::format("{}.json created.", "all");
//	MessageBoxA(nullptr, message.c_str(), "StagesObject", 0);
//}
//
//void GameScene::FilesOverWrite(const std::string& stage) {
//	floorManager_->FileOverWrite(stage);
//	std::string message = std::format("{}.json OverWrite.", "all");
//	MessageBoxA(nullptr, message.c_str(), "StagesObject", 0);
//}
//
//void GameScene::FilesLoad(const std::string& stage) {
//	floorManager_->LoadFiles(stage);
//	std::string message = std::format("{}.json loaded.", "all");
//	MessageBoxA(nullptr, message.c_str(), "StagesObject", 0);
//}
//
//bool GameScene::IsCollisionOBBOBB(const OBB& obb1, const OBB& obb2){
//	Vector3 vector{};
//	Matrix matrix;
//
//	Vector3 separatingAxis[15]{};
//	separatingAxis[0] = obb1.orientations[0];
//	separatingAxis[1] = obb1.orientations[1];
//	separatingAxis[2] = obb1.orientations[2];
//	separatingAxis[3] = obb2.orientations[0];
//	separatingAxis[4] = obb2.orientations[1];
//	separatingAxis[5] = obb2.orientations[2];
//	int axisNum = 6;
//	for (int index1 = 0; index1 < 3; index1++) {
//		for (int index2 = 0; index2 < 3; index2++) {
//			separatingAxis[axisNum] = vector.Cross(obb1.orientations[index1], obb2.orientations[index2]);
//			axisNum++;
//		}
//	}
//
//	Vector3 obb1Vertex[8]{};
//	// bottom
//	obb1Vertex[0] = Vector3{ -obb1.size.x, -obb1.size.y, -obb1.size.z };
//	obb1Vertex[1] = Vector3{ +obb1.size.x, -obb1.size.y, -obb1.size.z };
//	obb1Vertex[2] = Vector3{ -obb1.size.x, -obb1.size.y, +obb1.size.z };
//	obb1Vertex[3] = Vector3{ +obb1.size.x, -obb1.size.y, +obb1.size.z };
//	// top
//	obb1Vertex[4] = Vector3{ -obb1.size.x, +obb1.size.y, -obb1.size.z };
//	obb1Vertex[5] = Vector3{ +obb1.size.x, +obb1.size.y, -obb1.size.z };
//	obb1Vertex[6] = Vector3{ -obb1.size.x, +obb1.size.y, +obb1.size.z };
//	obb1Vertex[7] = Vector3{ +obb1.size.x, +obb1.size.y, +obb1.size.z };
//
//	Matrix4x4 rotateMatrix1 = GetRotate(obb1);
//
//	Vector3 obb2Vertex[8]{};
//	// bottom
//	obb2Vertex[0] = Vector3{ -obb2.size.x, -obb2.size.y, -obb2.size.z };
//	obb2Vertex[1] = Vector3{ +obb2.size.x, -obb2.size.y, -obb2.size.z };
//	obb2Vertex[2] = Vector3{ -obb2.size.x, -obb2.size.y, +obb2.size.z };
//	obb2Vertex[3] = Vector3{ +obb2.size.x, -obb2.size.y, +obb2.size.z };
//	// top
//	obb2Vertex[4] = Vector3{ -obb2.size.x, +obb2.size.y, -obb2.size.z };
//	obb2Vertex[5] = Vector3{ +obb2.size.x, +obb2.size.y, -obb2.size.z };
//	obb2Vertex[6] = Vector3{ -obb2.size.x, +obb2.size.y, +obb2.size.z };
//	obb2Vertex[7] = Vector3{ +obb2.size.x, +obb2.size.y, +obb2.size.z };
//
//	Matrix4x4 rotateMatrix2 = GetRotate(obb2);
//
//	for (int index = 0; index < 8; index++) {
//		obb1Vertex[index] = matrix.TransformVec(obb1Vertex[index], rotateMatrix1);
//		obb1Vertex[index] = (obb1Vertex[index] + obb1.center);
//		obb2Vertex[index] = matrix.TransformVec(obb2Vertex[index], rotateMatrix2);
//		obb2Vertex[index] = (obb2Vertex[index] + obb2.center);
//	}
//
//	for (axisNum = 0; axisNum < 15; axisNum++) {
//		float projectionPoint1[8]{};
//		float projectionPoint2[8]{};
//		float min1, max1;
//		float min2, max2;
//		min1 = vector.Dot(obb1Vertex[0], vector.Normalize(separatingAxis[axisNum]));
//		min2 = vector.Dot(obb2Vertex[0], vector.Normalize(separatingAxis[axisNum]));
//		max1 = min1;
//		max2 = min2;
//		for (int index = 0; index < 8; index++) {
//			projectionPoint1[index] =
//				vector.Dot(obb1Vertex[index], vector.Normalize(separatingAxis[axisNum]));
//			projectionPoint2[index] =
//				vector.Dot(obb2Vertex[index], vector.Normalize(separatingAxis[axisNum]));
//			if (index == 0) {
//				min1 = projectionPoint1[index];
//				min2 = projectionPoint2[index];
//				max1 = min1;
//				max2 = min2;
//			}
//			else {
//				min1 = min(min1, projectionPoint1[index]);
//				min2 = min(min2, projectionPoint2[index]);
//				max1 = max(max1, projectionPoint1[index]);
//				max2 = max(max2, projectionPoint2[index]);
//			}
//		}
//		float L1 = max1 - min1;
//		float L2 = max2 - min2;
//		float sumSpan = L1 + L2;
//		float longSpan = max(max1, max2) - min(min1, min2);
//		if (sumSpan < longSpan) {
//			return false;
//		}
//	}
//	return true;
//}
