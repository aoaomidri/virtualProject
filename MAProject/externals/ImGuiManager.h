#pragma once
#include"WinApp.h"
#include"DirectXCommon.h"
/*imguiの初期化や設定を行う*/
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
	//imguiの初期化処理
	void ImguiInitialize();

	ID3D12Device* device_ = nullptr;
};

