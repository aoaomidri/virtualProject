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

void TrailRender::MakeResource(std::vector<VertexData> vertexes){
	auto* textureManager = TextureManager::GetInstance();

	//頂点リソースの作成
	vertexResource_ = textureManager->CreateBufferResource(sizeof(VertexData) * vertexes.size());


	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点三つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertexes.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate_));
	std::memcpy(vertexDate_, vertexes.data(), sizeof(VertexData) * vertexes.size());

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
