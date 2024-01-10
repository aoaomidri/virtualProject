#include"Game/Input/Input.h"
#include"Engine/Base/WinApp.h"
#include"Engine/Base/DirectXCommon.h"
#include"Game/Scene/GameScene.h"
#include "Game/Item/Adjustment_Item.h"
#include"Game/random/RandomMaker.h"
#include <cassert>

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
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//CoInitializeEx(0, COINIT_MULTITHREADED);

	D3DResourceLeakChecker leakCheck;

	auto window_ = std::make_unique<WinApp>();
	window_->Initialize();

	/*キー入力の初期化処理*/
	auto input_ = std::make_unique<Input>();
	input_->Initialize(window_.get());

	auto dxCommon_ = std::make_unique<DirectXCommon>();
	dxCommon_->Initialize(window_.get());

	Adjustment_Item* adjustment_item = Adjustment_Item::GetInstance();

	//グローバル変数の読み込み
	adjustment_item->LoadFiles();

	RandomMaker* randomMaker = RandomMaker::GetInstance();
	//此処だけに初期化
	randomMaker->Initialize();

	Audio* audio_ = Audio::GetInstance();
	audio_->Initialize();

	auto gameScene_ = std::make_unique<GameScene>();
	gameScene_->Initialize(dxCommon_.get());

	

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

		//adjustment_item->Update();

		gameScene_->Update(input_.get());
		
		dxCommon_->EndImgui();

		dxCommon_->PreDraw();
		//3D描画
		gameScene_->Draw3D();
		gameScene_->DrawParticle();
		//2D描画
		gameScene_->Draw2D();
		dxCommon_->PostDraw();

		if (input_->Trigerkey(DIK_ESCAPE)){
			break;
		}
#ifdef _DEBUG
		
#endif // DEBUG_


	}
	CoUninitialize();

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	audio_->Reset();

	gameScene_->AudioDataUnLoad();

	gameScene_->Finalize();
	window_->Finalize();
#ifdef _DEBUG
	OutputDebugStringA("文字列リテラルを出力するよ\n");
	std::string a("stringに埋め込んだ文字列を出力するよ\n");
	OutputDebugStringA(a.c_str());
#endif // _DEBUG

	

	return 0;
}

