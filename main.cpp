#include"Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"Sprite.h"
#include"Object3D.h"
#include <cassert>
#include"math/Vector3.h"
#include"OBB.h"

bool IsCollisionOBBOBB(const OBB& obb1, const OBB& obb2) {
	static Vector3 vector;
	static Matrix matrix;

	Vector3 separatingAxis[15];
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

	Vector3 obb1Vetyex[8];
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

	Vector3 obb2Vetyex[8];
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


struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker()
	{
		//リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//CoInitializeEx(0, COINIT_MULTITHREADED);

	D3DResourceLeakChecker leakCheck;

	auto window_ = std::make_unique<WinApp>();
	window_->Initialize();

	/*キー入力の初期化処理*/
	auto input_ = std::make_unique<Input>();
	input_->Initialize(window_.get());

	auto dxCommon_ = std::make_unique<DirectXCommon>();
	dxCommon_->Initialize(window_.get());

	auto textureManager = std::make_unique<TextureManager>();
	textureManager->Initialize(dxCommon_->GetDevice(),
		dxCommon_->GetCommandList(), dxCommon_->GetSRVHeap());

	textureManager->Load("resources/uvChecker.png", 0);
	textureManager->Load("resources/rock.png", 1);
	textureManager->Load("resources/Floor.png", 2);
	textureManager->Load("resources/Road.png", 3);
	textureManager->Load("resources/Sky.png", 4);
	textureManager->Load("resources/Enemy/EnemyTex.png", 5);
	textureManager->Load("resources/EnemyParts/EnemyParts.png", 6);

	auto box_ = std::make_unique<Object3D>();
	box_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(),"box");	

	auto box2_ = std::make_unique<Object3D>();
	box2_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "box");

	auto box3_ = std::make_unique<Object3D>();
	box3_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "box");

	auto player_ = std::make_unique<Object3D>();
	player_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "box");

	auto goal_ = std::make_unique<Object3D>();
	goal_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "box");

	auto enemy_ = std::make_unique<Object3D>();
	enemy_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "Enemy");

	auto enemyParts_ = std::make_unique<Object3D>();
	enemyParts_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(), "EnemyParts");

	auto skyDome_ = std::make_unique<Object3D>();
	skyDome_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList(),"skyDome");
	//skyDome_->SetIsDraw(false);

	//auto sprite_ = std::make_unique<Sprite>();
	//sprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());
	//sprite_->SetIsDraw(false);
	//Vector2 position_ = { 100.0f,100.0f };

	//auto sprite2_ = std::make_unique<Sprite>();
	//sprite2_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());
	//sprite2_->SetPosition(position_);
	//sprite2_->SetIsDraw(false);

	/*初期化処理とかはここで終わり*/
	MSG msg{};
	
	Transform floorTransform[3] = {};
	
	floorTransform[0]={
		{2.0f,0.5f,2.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
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

	Transform PlayerTransform{
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.8f,0.0f}
	};

	Transform EnemyTransform{
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,3.14f,0.0f},
		.translate = {0.0f,0.8f,7.0f}
	};

	Transform EnemyPartsTransform{
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,0.0f,1.57f},
		.translate = {0.0f,1.7f,7.0f}
	};

	Transform goalTransform{
		.scale = {0.3f,0.3f,0.3f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.65f,9.0f}
	};

	Transform skyDomeTransform{
		.scale = {20.0f,20.0f,20.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};



	Transform cameraTransform{
		.scale = {1.0f,1.0f,1.0f},
		.rotate = {0.0f,0.0f,0.0f},		
		.translate = {0.0f,0.0f,0.0f}
	};

	OBB playerOBB = {};

	OBB enemyOBB = {};

	OBB floorOBB[3] = {};

	OBB goalOBB = {};

	Matrix4x4 playerMatrix{};

	Matrix4x4 enemyMatrix{};

	Matrix4x4 enemyPartsMatrix{};

	Matrix4x4 moveFloorMatrix[3]{};

	Matrix4x4 skyDomeMatrix{};

	Matrix4x4 goalMatrix{};

	float Magnification = 1.0f;	

	float moveSpeed_ = 0.02f;

	float EnemyMagnification = 1.0f;

	float EnemyMoveSpeed_ = 0.01f;

	const float playerSpeed_ = 0.1f;

	Vector3 move = {};

	Vector3 cameraMove_{};

	Vector3 vector_ = {};

	int chackCollision = 0;
	
	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (window_->ProcessMessage()) {
			break;
		}
		
		dxCommon_->StartImgui();
		//ゲームの処理

		input_->Update();

		cameraMove_ = { input_->GetPadRStick().y * 0.05f,input_->GetPadRStick().x * 0.05f,0.0f };

		

		cameraTransform.rotate += cameraMove_;

		Vector3 cameraOffset = { 0.0f,1.0f,-8.0f };

		Matrix4x4 cameraRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(cameraTransform.rotate);

		cameraOffset = Matrix::GetInstance()->TransformNormal(cameraOffset, cameraRotateMatrix);

		
		

		/*sprite_->Update();
		sprite2_->Update();*/
		if (input_->PushUp()){
			move.z = playerSpeed_;
			
		}
		else if (input_->PushDown()) {
			move.z = -playerSpeed_;
		}
		else{
			move.z = input_->GetPadLStick().y * playerSpeed_;
		}

		if (input_->PushRight()) {
			move.x = playerSpeed_;
		}
		else if (input_->PushLeft()) {
			move.x = -playerSpeed_;
		}
		else {
			move.x = input_->GetPadLStick().x * playerSpeed_;
		}
		

		move = Matrix::GetInstance()->TransformNormal(move, cameraRotateMatrix);
		
		move.y = 0.0f;
		if (move.x != 0.0f || move.z != 0.0f) {
			PlayerTransform.rotate.y = std::atan2(move.x, move.z);
		}

		if (input_->GetRTriggerDown()) {
			cameraTransform.rotate = PlayerTransform.rotate;

		}


		PlayerTransform.translate += move;
		PlayerTransform.translate.y -= 0.06f;

		EnemyTransform.translate.x += EnemyMoveSpeed_ * EnemyMagnification;

		if (EnemyTransform.translate.x <= -2.0f) {
			EnemyMagnification *= -1.0f;
		}
		else if (EnemyTransform.translate.x >= 2.0f) {
			EnemyMagnification *= -1.0f;
		}
		EnemyPartsTransform.translate.x = EnemyTransform.translate.x;
		EnemyPartsTransform.translate.z = EnemyTransform.translate.z;

		EnemyPartsTransform.rotate.x += 0.3f;

		floorTransform[1].translate.x += moveSpeed_ * Magnification;

		if (floorTransform[1].translate.x <= -4.0f) {
			Magnification *= -1.0f;
		}
		else if (floorTransform[1].translate.x >= 4.0f) {
			Magnification *= -1.0f;
		}

		playerMatrix = Matrix::GetInstance()->MakeAffineMatrix(PlayerTransform.scale, PlayerTransform.rotate, PlayerTransform.translate);
		enemyMatrix = Matrix::GetInstance()->MakeAffineMatrix(EnemyTransform.scale, EnemyTransform.rotate, EnemyTransform.translate);
		enemyPartsMatrix = Matrix::GetInstance()->MakeAffineMatrix(EnemyPartsTransform.scale, EnemyPartsTransform.rotate, EnemyPartsTransform.translate);
		moveFloorMatrix[0] = Matrix::GetInstance()->MakeAffineMatrix(floorTransform[0].scale, floorTransform[0].rotate, floorTransform[0].translate);
		moveFloorMatrix[1] = Matrix::GetInstance()->MakeAffineMatrix(floorTransform[1].scale, floorTransform[1].rotate, floorTransform[1].translate);
		moveFloorMatrix[2] = Matrix::GetInstance()->MakeAffineMatrix(floorTransform[2].scale, floorTransform[2].rotate, floorTransform[2].translate);
		skyDomeMatrix= Matrix::GetInstance()->MakeAffineMatrix(skyDomeTransform.scale, skyDomeTransform.rotate, skyDomeTransform.translate);
		goalMatrix = Matrix::GetInstance()->MakeAffineMatrix(goalTransform.scale, goalTransform.rotate, goalTransform.translate);

		PlayerTransform.translate = { playerMatrix.m[3][0],playerMatrix.m[3][1], playerMatrix.m[3][2] };



		

		/*OBBの設定および当たり判定処理*/
		playerOBB.center = PlayerTransform.translate;
		playerOBB.size = PlayerTransform.scale;
		Matrix4x4 playerRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(PlayerTransform.rotate);
		SetOridentatios(playerOBB, playerRotateMatrix);

		enemyOBB.center =EnemyTransform.translate;
		enemyOBB.size = EnemyTransform.scale;
		Matrix4x4 enemyRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(EnemyTransform.rotate);
		SetOridentatios(enemyOBB, enemyRotateMatrix);

		for (int i = 0; i < 3; i++){
			floorOBB[i].center = floorTransform[i].translate;
			floorOBB[i].size = floorTransform[i].scale;
			Matrix4x4 floorRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(floorTransform[i].rotate);
			SetOridentatios(floorOBB[i], floorRotateMatrix);
		}

		goalOBB.center = goalTransform.translate;
		goalOBB.size = goalTransform.scale;
		Matrix4x4 GoalRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(goalTransform.rotate);
		SetOridentatios(goalOBB, GoalRotateMatrix);

		for (int i = 0; i < 3; i++) {
			if (IsCollisionOBBOBB(playerOBB, floorOBB[i])){
				chackCollision = 1;
				PlayerTransform.translate.y = 0.85f;
				break;
			}
			else{
				chackCollision = 0;
				

			}			
		}
		if (IsCollisionOBBOBB(playerOBB, floorOBB[1])) {
			PlayerTransform.translate.x += moveSpeed_ * Magnification;
			//player_->parent_ = &moveFloorMatrix[1];
			//playerMatrix = Matrix::GetInstance()->Multiply(playerMatrix, moveFloorMatrix[1]);
			//PlayerTransform.translate = { playerMatrix.m[3][0],playerMatrix.m[3][1], playerMatrix.m[3][2] };
		}
		else {
			player_->parent_ = nullptr;
		}


		cameraTransform.translate = PlayerTransform.translate + cameraOffset;


		if (IsCollisionOBBOBB(playerOBB,goalOBB)||IsCollisionOBBOBB(playerOBB,enemyOBB)){
			PlayerTransform = {
				.scale = {0.3f,0.3f,0.3f},
				.rotate = {0.0f,0.0f,0.0f},
				.translate = {0.0f,3.8f,0.0f}
			};
		}

		if (PlayerTransform.translate.y <= -10.0f) {
			PlayerTransform= {
				.scale = {0.3f,0.3f,0.3f},
				.rotate = {0.0f,0.0f,0.0f},
				.translate = {0.0f,3.8f,0.0f}
			};
		}
		
		/*ここまで*/
		skyDomeTransform.rotate.y += 0.01f;

		box_->Update(moveFloorMatrix[0], cameraTransform);
		box2_->Update(moveFloorMatrix[1], cameraTransform);
		box3_->Update(moveFloorMatrix[2], cameraTransform);

		player_->Update(playerMatrix, cameraTransform);

		enemy_->Update(enemyMatrix, cameraTransform);

		enemyParts_->Update(enemyPartsMatrix, cameraTransform);

		goal_->Update(goalMatrix, cameraTransform);

		skyDome_->Update(skyDomeMatrix, cameraTransform);

		ImGui::Begin("床のTransform");
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

		ImGui::Begin("プレイやー");
		ImGui::DragFloat3("プレイヤーの座標", &PlayerTransform.translate.x, 0.01f);
		ImGui::DragFloat3("プレイヤーの回転", &PlayerTransform.rotate.x, 0.01f);
		ImGui::DragFloat3("プレイヤーの大きさ", &PlayerTransform.scale.x, 0.01f);
		ImGui::End();

		ImGui::Begin("ゴール");
		ImGui::Text("床とプレイヤーが接触しているか %d ", chackCollision);
		ImGui::DragFloat3("ゴールの座標", &goalTransform.translate.x, 0.01f);
		ImGui::DragFloat3("ゴールの回転", &goalTransform.rotate.x, 0.01f);
		ImGui::DragFloat3("ゴールの大きさ", &goalTransform.scale.x, 0.01f);
		ImGui::End();

		ImGui::Begin("カメラ関連");
		ImGui::DragFloat3("カメラ座標", &cameraTransform.translate.x, 0.01f);
		ImGui::DragFloat3("カメラ回転", &cameraTransform.rotate.x, 0.01f);
		ImGui::DragFloat3("カメラのオフセット", &cameraOffset.x, 0.01f);

		ImGui::End();

		
		dxCommon_->EndImgui();

		dxCommon_->PreDraw();
		textureManager->PreDraw3D();

		///////*ここから3Dモデルの描画*///////
		box_->Draw(dxCommon_->GetCommandList(), textureManager->SendGPUDescriptorHandle(3));
		box2_->Draw(dxCommon_->GetCommandList(), textureManager->SendGPUDescriptorHandle(3));
		box3_->Draw(dxCommon_->GetCommandList(), textureManager->SendGPUDescriptorHandle(3));

		player_->Draw(dxCommon_->GetCommandList(), textureManager->SendGPUDescriptorHandle(4));

		enemy_->Draw(dxCommon_->GetCommandList(), textureManager->SendGPUDescriptorHandle(5));

		enemyParts_->Draw(dxCommon_->GetCommandList(), textureManager->SendGPUDescriptorHandle(6));

		goal_->Draw(dxCommon_->GetCommandList(), textureManager->SendGPUDescriptorHandle(2));

		skyDome_->Draw(dxCommon_->GetCommandList(), textureManager->SendGPUDescriptorHandle(1));
		
		///////*3D描画はここまで*///////
		textureManager->PostDraw3D();
		textureManager->PreDraw2D();

		///////*ここから2Dテクスチャの描画*///////
		//sprite_->Draw(dxCommon_->GetCommandList(),textureManager->SendGPUDescriptorHandle(0));
		//sprite2_->Draw(dxCommon_->GetCommandList(), textureManager->SendGPUDescriptorHandle(1));
		
		///////*2Dテクスチャはここまで*///////
		textureManager->PostDraw2D();
		dxCommon_->PostDraw();

		if (input_->Trigerkey(DIK_ESCAPE)){
			break;
		}	

	}
	CoUninitialize();

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	window_->Finalize();


	return 0;
}

