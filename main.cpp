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

	auto sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());
	//sprite_->SetIsDraw(false);
	

	auto sprite2_ = std::make_unique<Sprite>();
	sprite2_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());
	Vector2 position_2 = { 640.0f,360.0f };
	float spriteRotate_2 = 0.0f;
	Vector2 scale_2 = { 640.0f,360.0f };
	Vector2 anchorPoint2_ = { 0.5f,0.5f };
	Vector4 spriteColor_2 = { 1.0f,1.0f,1.0f,1.0f };
	bool isSpriteDraw2 = true;
	bool isFlipX2_ = false;
	bool isFlipY2_ = false;
	Transform uvTransform_ = {
		.scale = {1.0f,1.0f,1.0f},
		.rotate = {0.0f,0.0f,0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};


	/*初期化処理とかはここで終わり*/
	MSG msg{};

	
	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (window_->ProcessMessage()) {
			break;
		}
		
		dxCommon_->StartImgui();
		//ゲームの処理

		input_->Update();

		//cameraMove_ = { input_->GetPadRStick().y * 0.05f,input_->GetPadRStick().x * 0.05f,0.0f };

		//
		///*カメラ*/
		//cameraTransform.rotate += cameraMove_;

		//Vector3 cameraOffset = { 0.0f,1.0f,-8.0f };

		//Matrix4x4 cameraRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(cameraTransform.rotate);

		//cameraOffset = Matrix::GetInstance()->TransformNormal(cameraOffset, cameraRotateMatrix);		
		

		sprite_->Update();
		sprite2_->Update();
		sprite2_->SetPosition(position_2);
		sprite2_->SetRotation(spriteRotate_2);
		sprite2_->SetScale(scale_2);
		sprite2_->SetAnchorPoint(anchorPoint2_);
		sprite2_->SetColor(spriteColor_2);
		sprite2_->SetIsDraw(isSpriteDraw2);
		sprite2_->SetIsFlipX(isFlipX2_);
		sprite2_->SetIsFlipY(isFlipY2_);
		sprite2_->SetUVTransform(uvTransform_);
		
		ImGui::Begin("2Dテクスチャ");
		ImGui::DragFloat2("座標", &position_2.x, 1.0f);
		ImGui::DragFloat("回転", &spriteRotate_2, 0.01f);
		ImGui::DragFloat2("大きさ", &scale_2.x, 1.0f);
		ImGui::DragFloat2("アンカーポイント", &anchorPoint2_.x, 0.1f, 0.0f, 1.0f);
		ImGui::ColorEdit4("画像の色", &spriteColor_2.x);
		ImGui::Checkbox("画像を描画する", &isSpriteDraw2);
		ImGui::Checkbox("左右反転", &isFlipX2_);
		ImGui::Checkbox("上下反転", &isFlipY2_);
		ImGui::DragFloat2("UVTranslate", &uvTransform_.translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &uvTransform_.scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &uvTransform_.rotate.z);
		ImGui::End();

		
		dxCommon_->EndImgui();

		dxCommon_->PreDraw();
		textureManager->PreDraw3D();

		///////*ここから3Dモデルの描画*///////
		
		
		///////*3D描画はここまで*///////
		textureManager->PostDraw3D();
		textureManager->PreDraw2D();

		///////*ここから2Dテクスチャの描画*///////
		sprite_->Draw(dxCommon_->GetCommandList(),textureManager->SendGPUDescriptorHandle(1));
		sprite2_->Draw(dxCommon_->GetCommandList(), textureManager->SendGPUDescriptorHandle(0));
		
		///////*2Dテクスチャはここまで*///////
		textureManager->PostDraw2D();
		dxCommon_->PostDraw();

		if (input_->Trigerkey(DIK_ESCAPE)||input_->GetLTrigger()){
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

