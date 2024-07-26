#include "Sprite.h"
#include"../Base/DirectXCommon.h"
#include <cassert>
#include <d3dcompiler.h>
#include "../../externals/DirectXTex/d3dx12.h"
#include"../../externals/DirectXTex/DirectXTex.h"

#pragma comment(lib, "d3dcompiler.lib")

void Sprite::Initialize(uint32_t TextureNumber){

	device_ = DirectXCommon::GetInstance()->GetDevice();
	commandList_ = DirectXCommon::GetInstance()->GetCommandList();
	textureManager_ = TextureManager::GetInstance();

	makeSpriteResource();

	transformSprite = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	cameraTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,-10.0f}
	};

	materialDate->uvTransform = Matrix::GetInstance()->MakeIdentity4x4();

	if (TextureNumber != UINT32_MAX) {
		textureNumber_ = TextureNumber;
		AdjustTextureSize();
		//テクスチャサイズをスプライトのサイズに適用
		scale_ = textureSize_;
	}
	
}

void Sprite::Update(){

	transformSprite.translate = { position_.x,position_.y,0.0f };
	transformSprite.rotate = { 0.0f,0.0f,rotation_.z };
	
	//scale_ = textureSize_;

	if (!isDraw_){
		return;
	}

	ID3D12Resource* textureBuffer = textureManager_->GetTextureBuffer(textureNumber_);
	if (textureBuffer){
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		float tex_left = textureLeftTop_.x / resDesc.Width;
		float tex_right = (textureLeftTop_.x + textureSize_.x) / resDesc.Width;
		float tex_top = textureLeftTop_.y / resDesc.Height;
		float tex_bottom = (textureLeftTop_.y + textureSize_.y) / resDesc.Height;

		//頂点のUVに反映する
		vertexDataSprite[0].texcoord = { tex_left,tex_bottom };		//左下
		vertexDataSprite[1].texcoord = { tex_left,tex_top };		//左上
		vertexDataSprite[2].texcoord = { tex_right,tex_bottom };	//右下
		vertexDataSprite[3].texcoord = { tex_right,tex_top };		//右上
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

	//左右反転
	if (isFlipX_){
		left *= -1.0f;
		right *= -1.0f;
	}
	//上下反転
	if (isFlipY_){
		top *= -1.0f;
		bottom *= -1.0f;
	}

	//頂点データ
	vertexDataSprite[0].position = { left,bottom,0.0f,1.0f };//左下
	vertexDataSprite[1].position = { left,top,0.0f,1.0f };//左上
	vertexDataSprite[2].position = { right,bottom,0.0f,1.0f };//右下
	vertexDataSprite[3].position = { right,top,0.0f,1.0f };//右上

	materialDate->color = color_;

	Matrix4x4 worldMatrixSprite = Matrix::GetInstance()->MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 cameraMatrixSprite = Matrix::GetInstance()->MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrixSprite = Matrix::GetInstance()->Inverce(cameraMatrixSprite);
	Matrix4x4 projectionMatrixSprite = Matrix::GetInstance()->MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Matrix::GetInstance()->Multiply(worldMatrixSprite, Matrix::GetInstance()->Multiply(viewMatrixSprite, projectionMatrixSprite));
	*wvpDataSprite = worldViewProjectionMatrixSprite;

	Matrix4x4 uvTransformMatrixSprite = Matrix::GetInstance()->MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrixSprite = Matrix::GetInstance()->Multiply(uvTransformMatrixSprite, Matrix::GetInstance()->MakeRotateMatrixZ(uvTransform_.rotate));
	uvTransformMatrixSprite = Matrix::GetInstance()->Multiply(uvTransformMatrixSprite, Matrix::GetInstance()->MakeTranslateMatrix(uvTransform_.translate));
	materialDate->uvTransform = uvTransformMatrixSprite;
}

void Sprite::Update(const Matrix4x4& viewPro){
	transformSprite.translate = position_;
	transformSprite.rotate = rotation_;
	transformSprite.scale = { scale_.x,scale_.y,0 };

	//scale_ = textureSize_;

	if (!isDraw_) {
		return;
	}

	ID3D12Resource* textureBuffer = textureManager_->GetTextureBuffer(textureNumber_);
	if (textureBuffer) {
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		float tex_left = textureLeftTop_.x / resDesc.Width;
		float tex_right = (textureLeftTop_.x + textureSize_.x) / resDesc.Width;
		float tex_top = textureLeftTop_.y / resDesc.Height;
		float tex_bottom = (textureLeftTop_.y + textureSize_.y) / resDesc.Height;

		//頂点のUVに反映する
		vertexDataSprite[0].texcoord = { tex_left,tex_bottom };		//左下
		vertexDataSprite[1].texcoord = { tex_left,tex_top };		//左上
		vertexDataSprite[2].texcoord = { tex_right,tex_bottom };	//右下
		vertexDataSprite[3].texcoord = { tex_right,tex_top };		//右上
	}

	if (anchorPoint_.x < 0) {
		anchorPoint_.x = 0.0f;
	}
	else if (anchorPoint_.x > 1) {
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

	//左右反転
	if (isFlipX_) {
		left *= -1.0f;
		right *= -1.0f;
	}
	//上下反転
	if (isFlipY_) {
		top *= -1.0f;
		bottom *= -1.0f;
	}

	//頂点データ
	vertexDataSprite[0].position = { left,bottom,0.0f,1.0f };//左下
	vertexDataSprite[1].position = { left,top,0.0f,1.0f };//左上
	vertexDataSprite[2].position = { right,bottom,0.0f,1.0f };//右下
	vertexDataSprite[3].position = { right,top,0.0f,1.0f };//右上

	materialDate->color = color_;

	Matrix4x4 worldMatrixSprite = Matrix::GetInstance()->MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 worldViewProjectionMatrixSprite = Matrix::GetInstance()->Multiply(worldMatrixSprite, viewPro);
	*wvpDataSprite = worldViewProjectionMatrixSprite;

	Matrix4x4 uvTransformMatrixSprite = Matrix::GetInstance()->MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrixSprite = Matrix::GetInstance()->Multiply(uvTransformMatrixSprite, Matrix::GetInstance()->MakeRotateMatrixZ(uvTransform_.rotate));
	uvTransformMatrixSprite = Matrix::GetInstance()->Multiply(uvTransformMatrixSprite, Matrix::GetInstance()->MakeTranslateMatrix(uvTransform_.translate));
	materialDate->uvTransform = uvTransformMatrixSprite;
}

void Sprite::Draw(){

	if (!isDraw_){
		return;
	}
	Update();
	//2Dの描画
	//マテリアルにCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, textureManager_->SendGPUDescriptorHandle(textureNumber_));
	//Spriteの描画。変更が必要なものだけ変更する
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	commandList_->IASetIndexBuffer(&indexBufferViewSprite);
	//TransformationMatrixCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, wvpResourceSprite->GetGPUVirtualAddress());
	//描画
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);


}

void Sprite::Draw(const Matrix4x4& viewPro){
	if (!isDraw_) {
		return;
	}
	Update(viewPro);
	//2Dの描画
	//マテリアルにCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, textureManager_->SendGPUDescriptorHandle(textureNumber_));
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
	
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };//左下
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };//左上
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };//右下
	vertexDataSprite[3].texcoord = { 1.0f,0.0f };//右上

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
	materialResource = CreateBufferResource(device_, sizeof(Model::Material));

	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));

	materialDate->enableLighting = false;

	materialDate->uvTransform = Matrix::GetInstance()->MakeIdentity4x4();
	//今回は赤を書き込んでみる
	
}

void Sprite::AdjustTextureSize(){
	ID3D12Resource* textureBuffer = textureManager_->GetTextureBuffer(textureNumber_);
	assert(textureBuffer);

	//テクスチャ情報取得
	D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

	textureSize_.x = static_cast<float>(resDesc.Width);
	textureSize_.y = static_cast<float>(resDesc.Height);
}


