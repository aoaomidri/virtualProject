#include "WinApp.h"
#include <d3d12.h> 
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include<dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"winmm.lib")

WinApp* WinApp::GetInstance(){
	static WinApp instance;
	return &instance;
}

void WinApp::Initialize() {
	// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	//ウィンドウプロシージャ
	wndClass_.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wndClass_.lpszClassName = L"WindowClass";
	//インスタンスハンドル
	wndClass_.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wndClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラスを登録する
	RegisterClass(&wndClass_);

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT windowRect_ = { 0,0,kWindowWidth,kWindowHeight };

	//クライアント領域をもとに実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&windowRect_, WS_OVERLAPPEDWINDOW, false);

	 hwnd_ = CreateWindow(
		wndClass_.lpszClassName,
		L"ゴリラゴリラゴリラ",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect_.right - windowRect_.left,
		windowRect_.bottom - windowRect_.top,
		nullptr,
		nullptr,
		wndClass_.hInstance,
		nullptr);

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックできるようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG


	//ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);

}

void WinApp::Finalize() {
	// ウィンドウクラスを登録解除
	UnregisterClass(wndClass_.lpszClassName, wndClass_.hInstance);

	// COM 終了
	CoUninitialize();
}

bool WinApp::ProcessMessage() {
	MSG msg{}; // メッセージ

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // メッセージがある？
	{
		TranslateMessage(&msg); // キー入力メッセージの処理
		DispatchMessage(&msg);  // ウィンドウプロシージャにメッセージを送る
	}

	if (msg.message == WM_QUIT) // 終了メッセージが来たらループを抜ける
	{
		return true;
	}

	return false;
}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//メッセージに応じてゲーム画面の処理を行う
	switch (msg){
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージの処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

