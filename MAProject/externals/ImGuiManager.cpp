#include "ImGuiManager.h"

void ImGuiManager::Initialize(){
#ifdef _DEBUG
	device_ = DirectXCommon::GetInstance()->GetDevice();

	ImguiInitialize();

#endif // _DEBUG


	
}

void ImGuiManager::Finalize(){
#ifdef _DEBUG
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#endif // _DEBUG

	//device_->Release();
}

void ImGuiManager::Begin(){
#ifdef _DEBUG

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // _DEBUG

}

void ImGuiManager::End(){
#ifdef _DEBUG

	ImGui::Render();
#endif // _DEBUG

}

void ImGuiManager::Draw(){
#ifdef _DEBUG
	//実際ののCommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXCommon::GetInstance()->GetCommandList());
#endif // _DEBUG

	
}

void ImGuiManager::ImguiInitialize(){
#ifdef _DEBUG

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHwnd());
	ImGui_ImplDX12_Init(
		device_,
		DirectXCommon::GetInstance()->GetSwapChainDesc().BufferCount/*swapChainDesc.BufferCount*/,
		DirectXCommon::GetInstance()->GetRTVDesc().Format,
		SRVDescriptorHeap::GetInstance()->GetSRVHeap(),
		SRVDescriptorHeap::GetInstance()->GetCPUDescriptorHandle(),
		SRVDescriptorHeap::GetInstance()->GetGPUDescriptorHandle()
	);


	//型"ImGuiIO&"の参照(constで修飾されていない)は型"ImGuiIO"の値では初期化できません
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	io.Fonts->AddFontFromFileTTF("Resources/Fonts/ipaexm.ttf", 13.0f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
#endif // _DEBUG

}
