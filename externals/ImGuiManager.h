#pragma once
#include"WinApp.h"
#include"DirectXCommon.h"

class ImGuiManager{
public:
	///<summary>
	/// 初期化
	///</summary>
	void Initialize();

	void Finalize();
	//imgui受付開始
	void Begin();
	//imgui受付終了
	void End();
	//画面への描画
	void Draw();

private:
	void ImguiInitialize();
};

