#include "Sprite.h"
#include"DirectXCommon.h"
#include <cassert>
#include <d3dcompiler.h>
#include "externals/DirectXTex/d3dx12.h"
#include"externals/DirectXTex/DirectXTex.h"

#pragma comment(lib, "d3dcompiler.lib")

Sprite::Sprite(){
	textureManager_ = TextureManager::GetInstance();
}

Sprite::~Sprite(){
	
}

void Sprite::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){

	device_ = device;
	commandList_ = commandList;

	makeSpriteResource();

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

	transformSprite.translate = { position_.x,position_.y,0.0f };
	transformSprite.rotate = { 0.0f,0.0f,rotation_ };

	

	if (!isDraw_){
		return;
	}
	if (anchorPoint_.x < 0) {
		anchorPoint_.x = 0.0f;
	}else if (anchorPoint_.x > 1) {
		anchorPoint_.x = 1.0f;
	}
	if (anchorPoint_.y < 0) {
		anchorPoint_.y = 0.0f;
	}
	else if (anchorPoint_.y > 1) {
		anchorPoint_.y = 1.0f;
	}

	float left = (0.0f - anchorPoint_.x) * scale_.x;
	float right = (1.0f - anchorPoint_.x) * scale_.x;
	float top = (0.0f - anchorPoint_.y) * scale_.y;
	float bottom = (1.0f - anchorPoint_.y) * scale_.y;

	//頂点データ
	vertexDataSprite[0].position = { left,bottom,0.0f,1.0f };//左下
	vertexDataSprite[1].position = { left,top,0.0f,1.0f };//左上
	vertexDataSprite[2].position = { right,bottom,0.0f,1.0f };//右下
	vertexDataSprite[3].position = { right,top,0.0f,1.0f };//右上

	*materialDate = color_;

	Matrix4x4 worldMatrixSprite = Matrix::GetInstance()->MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 cameraMatrixSprite = Matrix::GetInstance()->MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrixSprite = Matrix::GetInstance()->Inverce(cameraMatrixSprite);
	Matrix4x4 projectionMatrixSprite = Matrix::GetInstance()->MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Matrix::GetInstance()->Multiply(worldMatrixSprite, Matrix::GetInstance()->Multiply(viewMatrixSprite, projectionMatrixSprite));
	*wvpDataSprite = worldViewProjectionMatrixSprite;

}

void Sprite::Draw(D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle){

	if (!isDraw_){
		return;
	}
	
	//2Dの描画
	//マテリアルにCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, GPUHandle);
	//Spriteの描画。変更が必要なものだけ変更する
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	commandList_->IASetIndexBuffer(&indexBufferViewSprite);
	//TransformationMatrixCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, wvpResourceSprite->GetGPUVirtualAddress());
	//描画
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);


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

	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
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

	//リソースを作る
	wvpResourceSprite = CreateBufferResource(device_, sizeof(Matrix4x4));

	//アドレス取得
	wvpResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&wvpDataSprite));
	//単位行列を書き込む
	*wvpDataSprite = Matrix::GetInstance()->MakeIdentity4x4();

	//マテリアル用のリソース
	materialResource = CreateBufferResource(device_, sizeof(Vector4));

	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	//今回は赤を書き込んでみる
	
}


