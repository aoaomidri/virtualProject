#include "ComputePipeLineManager.h"
#include <DirectXCommon.h>

void ComputePipeLineManager::Initialize(){
	pipeLine_ = std::make_unique<ComputeGraphicsPipeline>();

	pipeLine_->Initialize(L"resources/shaders/object3D/Skinning.CS.hlsl");
}

ComputePipeLineManager* ComputePipeLineManager::GetInstance(){
	static ComputePipeLineManager instance;
	return &instance;
}

void ComputePipeLineManager::SetPipeLine(){
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	commandList->SetComputeRootSignature(pipeLine_->GetRootSignature());
	commandList->SetPipelineState(pipeLine_->GetPipeLineState());

}

