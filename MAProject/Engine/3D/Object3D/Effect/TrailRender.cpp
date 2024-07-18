#include "TrailRender.h"
#include"DirectXCommon.h"

void TrailRender::Initialize(){
	GraphicsPipelineTrail_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineTrail_->InitializeTrail(L"resources/shaders/TrailEffect/TrailEffect.VS.hlsl", L"resources/shaders/TrailEffect/TrailEffect.PS.hlsl");

	MakeResource();
}

void TrailRender::Draw(const TrailEffect* trail){
	auto command = DirectXCommon::GetInstance()->GetCommandList();

	command->SetGraphicsRootSignature(GraphicsPipelineTrail_->GetRootSignature());
	command->SetPipelineState(GraphicsPipelineTrail_->GetPipeLineState());

	//形状を設定。
	command->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	command->IASetVertexBuffers(0, 1, trail->GetVertexBuffer());
	command->SetGraphicsRootConstantBufferView(0, wvpResource_->GetGPUVirtualAddress());
	command->SetGraphicsRootConstantBufferView(1, materialResource_->GetGPUVirtualAddress());
	command->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->SendGPUDescriptorHandle(trail->GetTextureHandle()));

	command->DrawInstanced(static_cast<uint32_t>(trail->GetVertexSize()), 1, 0, 0);
}


void TrailRender::MakeResource(){
	auto* textureManager = TextureManager::GetInstance();

	//マテリアル用のリソース
	materialResource_ = textureManager->CreateBufferResource(sizeof(Model::Material));

	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialDate_));
	//今回は赤を書き込んでみる
	materialDate_->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialDate_->enableLighting = false;

	materialDate_->uvTransform.Identity();

	materialDate_->shininess = 1.0f;

	//wvp用のリソースを作る。TransformationMatrix一つ分のサイズを用意する
	wvpResource_ = textureManager->CreateBufferResource(sizeof(TransformationMatrix));
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおく
	wvpData_->WVP = Matrix::GetInstance()->MakeIdentity4x4();
	wvpData_->World = Matrix::GetInstance()->MakeIdentity4x4();
	wvpData_->WorldInverseTranspose = Matrix::GetInstance()->MakeIdentity4x4();

}
