#pragma once
#include"../Engine/Base/WinApp.h"
#include"../Engine/Base/DirectXCommon.h"

class ImGuiManager{
public:
	///<summary>
	/// 初期化
	///</summary>
	void Initialize(WinApp* window, DirectXCommon* directXcom);

	void Finalize();
	//imgui受付開始
	void Begin();
	//imgui受付終了
	void End();
	//画面への描画
	void Draw();

private:
	void ImguiInitialize();


	WinApp* window_ = nullptr;

	DirectXCommon* DXcom_ = nullptr;
};

