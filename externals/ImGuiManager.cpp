#include "ImGuiManager.h"

void ImGuiManager::Initialize(){
	device_ = DirectXCommon::GetInstance()->GetDevice();

	ImguiInitialize();

	
}

void ImGuiManager::Finalize(){
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	device_->Release();
}

void ImGuiManager::Begin(){
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End(){
	ImGui::Render();
}

void ImGuiManager::Draw(){
#ifdef _DEBUG
	//実際ののCommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXCommon::GetInstance()->GetCommandList());
#endif // _DEBUG

	
}

void ImGuiManager::ImguiInitialize(){
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHwnd());
	ImGui_ImplDX12_Init(
		device_,
		DirectXCommon::GetInstance()->GetSwapChainDesc().BufferCount/*swapChainDesc.BufferCount*/,
		DirectXCommon::GetInstance()->GetRTVDesc().Format,
		DirectXCommon::GetInstance()->GetSRVHeap(),
		DirectXCommon::GetInstance()->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
		DirectXCommon::GetInstance()->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart()
	);


	//型"ImGuiIO&"の参照(constで修飾されていない)は型"ImGuiIO"の値では初期化できません
	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF("Resources/Fonts/ipaexm.ttf", 13.0f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
}
