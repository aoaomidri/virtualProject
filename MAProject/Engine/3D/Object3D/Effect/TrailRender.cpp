#include "TrailRender.h"
#include"DirectXCommon.h"

void TrailRender::Initialize(){
	
	GraphicsPipelineTrail_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineTrail_->InitializeTrail(L"resources/shaders/particle/Particle.VS.hlsl", L"resources/shaders/particle/Particle.PS.hlsl");
}

void TrailRender::Draw(){
	auto command = DirectXCommon::GetInstance()->GetCommandList();

	command->SetGraphicsRootSignature(GraphicsPipelineTrail_->GetRootSignature());
	command->SetPipelineState(GraphicsPipelineTrail_->GetPipeLineState());

	command->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	command->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。
	command->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	command->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	command->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->SendGPUDescriptorHandle(0));

	command->DrawInstanced(3, 1, 0, 0);
}

void TrailRender::MakeResource(){

}
