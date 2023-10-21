#include "Sprite.h"
#include "TextureManager.h"
#include"DirectXCommon.h"
#include <cassert>
#include <d3dcompiler.h>
#include "externals/DirectXTex/d3dx12.h"
#include"externals/DirectXTex/DirectXTex.h"

#pragma comment(lib, "d3dcompiler.lib")

void Sprite::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){
	GraphicsPipeline_ = std::make_unique<GraphicsPipeline>();

	device_ = device;
	commandList_ = commandList;

	GraphicsPipeline_->Initialize(device_, L"resources/shaders/Object2d.VS.hlsl", L"resources/shaders/Object2d.PS.hlsl");

	makeSpriteResource();

	transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	transformSprite = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	cameraTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,-10.0f}
	};
}

void Sprite::Update(){
	//transform.rotate.y += 0.03f;
	//ImGui::Begin("三角形");
	//ImGui::DragFloat3("三角形のScale", &transform.scale.x, 0.1f);
	//ImGui::DragFloat3("三角形のRotate", &transform.rotate.x, 0.1f);
	//ImGui::DragFloat3("三角形のTransform", &transform.translate.x, 0.1f);
	//ImGui::End();
	transformSprite.translate = { position_.x,position_.y,0.0f };
	if (!isDraw_){
		return;
	}

	Matrix4x4 worldMatrixSprite = Matrix::GetInstance()->MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 cameraMatrixSprite = Matrix::GetInstance()->MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrixSprite = Matrix::GetInstance()->Inverce(cameraMatrixSprite);
	Matrix4x4 projectionMatrixSprite = Matrix::GetInstance()->MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Matrix::GetInstance()->Multiply(worldMatrixSprite, Matrix::GetInstance()->Multiply(viewMatrixSprite, projectionMatrixSprite));
	*wvpDataSprite = worldViewProjectionMatrixSprite;

}

void Sprite::Draw(ID3D12GraphicsCommandList* commandList, D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle){

	if (!isDraw_){
		return;
	}
	
	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipeline_->GetRootSignature());
	commandList->SetPipelineState(GraphicsPipeline_->GetPipeLineState());
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	
	///*commandList_->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	//commandList_->IASetIndexBuffer(&indexBufferViewSprite);*/
	//commandList->DrawInstanced(3, 1, 0, 0);

	//2Dの描画
	//マテリアルにCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, GPUHandle);
	//Spriteの描画。変更が必要なものだけ変更する
	commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	commandList->IASetIndexBuffer(&indexBufferViewSprite);
	//TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResourceSprite->GetGPUVirtualAddress());
	//描画
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);


}

Microsoft::WRL::ComPtr<ID3D12Resource> Sprite::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;

	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource = nullptr;
	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(hr));

	return bufferResource;
}

void Sprite::makeSpriteResource(){
	//Sprite用の頂点リソースを作る
	vertexResourceSprite = CreateBufferResource(device_, sizeof(VertexData) * 4);
	//頂点バッファービューを作成する
	
	//リソースの先頭アドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//1枚目の三角形
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	vertexDataSprite[3].texcoord = { 1.0f,0.0f };

	indexResourceSprite = CreateBufferResource(device_, sizeof(uint32_t) * 6);

	
	//リソースの先頭のアドレスから使用する
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス六つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
	//インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;	indexDataSprite[1] = 1;	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;	indexDataSprite[4] = 3;	indexDataSprite[5] = 2;


	////頂点リソースの作成
	//vertexResource = CreateBufferResource(device_, sizeof(VertexData) * 3);

	////頂点バッファビューを作成する
	//
	////リソースの先頭のアドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点三つ分のサイズ
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * 3;
	////1頂点当たりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	//リソースを作る
	wvpResourceSprite = CreateBufferResource(device_, sizeof(Matrix4x4));

	//アドレス取得
	wvpResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataSprite));
	//単位行列を書き込む
	*wvpDataSprite = Matrix::GetInstance()->MakeIdentity4x4();

	////頂点リソースにデータを書き込む
	//VertexData* vertexDate_ = nullptr;
	////書き込むためのアドレスを取得
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate_));
	////左下
	//vertexDate_[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	//vertexDate_[0].texcoord = { 0.0f,1.0f };
	////上
	//vertexDate_[1].position = { 0.0f,0.5f,0.0f,1.0f };
	//vertexDate_[1].texcoord = { 0.5f,0.0f };
	////右下
	//vertexDate_[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	//vertexDate_[2].texcoord = { 1.0f,1.0f };

	//マテリアル用のリソース
	materialResource = CreateBufferResource(device_, sizeof(Vector4));
	//マテリアルにデータを書き込む
	Vector4* materialDate = nullptr;

	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	//今回は赤を書き込んでみる
	*materialDate = { 1.0f, 1.0f, 1.0f, 1.0f };

	//リソースを作る
	wvpResource = CreateBufferResource(device_, sizeof(Matrix4x4));
	
	//アドレス取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込む
	*wvpData = Matrix::GetInstance()->MakeIdentity4x4();

}


