#include "ParticleBase.h"
#include"../../Base/DirectXCommon.h"
#include <cassert>
#include<fstream>
#include<sstream>


void ParticleBase::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {

	device_ = device;
	commandList_ = commandList;
	Model::SetDevice(device_);

	modelData_ = MakePrimitive();
	
	makeResource();

	isDraw_ = true;

	for (int i = 0; i < particleNum_; i++) {
		transforms[i].scale = { 3.0f,3.0f,3.0f };
		transforms[i].rotate = { 0.0f,0.0f,0.0f };
		transforms[i].translate = { i * 0.5f,i * 0.5f, i * 0.5f };
	}

	cameraTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,2.0f,-10.0f}
	};
}

void ParticleBase::Update(const Transform& transform, const ViewProjection& viewProjection) {
	if (!isDraw_) {
		return;
	}

	//rotate_.y += 0.01f;
	for (int i = 0; i < particleNum_; i++) {


		worldMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(transforms[i]);
		/*if (parent_) {
			worldMatrix_ = Matrix::GetInstance()->Multiply(worldMatrix_, *parent_);
		}
		position_ = { worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2] };
		chackMatrix_ = { worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2], worldMatrix_.m[3][3] };*/

		Matrix4x4 worldViewProjectionMatrix = Matrix::GetInstance()->Multiply(worldMatrix_, viewProjection.matViewProjection_);
		wvpData[i].WVP = worldViewProjectionMatrix;
		wvpData[i].World = worldMatrix_;
	}
	//materialDate->enableLighting = true;
}

void ParticleBase::Draw(D3D12_GPU_DESCRIPTOR_HANDLE TextureHandle, D3D12_GPU_DESCRIPTOR_HANDLE InstancingHandle) {

	if (!isDraw_) {
		return;
	}
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView);
	//形状を設定。
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(1, InstancingHandle);
	commandList_->SetGraphicsRootDescriptorTable(2, TextureHandle);
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	//3D三角の描画
	commandList_->DrawInstanced(UINT(modelData_.vertices.size()), particleNum_, 0, 0);
		
	
}

void ParticleBase::DrawImgui(){
	ImGui::Begin("行列表示");
	ImGui::DragFloat4("平行移動成分", &chackMatrix_.x, 0.01f);
	ImGui::End();

}

Microsoft::WRL::ComPtr<ID3D12Resource> ParticleBase::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

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

void ParticleBase::makeResource() {
	//頂点リソースの作成
	vertexResource = CreateBufferResource(device_, sizeof(VertexData) * modelData_.vertices.size());

	
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点三つ分のサイズ
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate));
	std::memcpy(vertexDate, modelData_.vertices.data(), sizeof(VertexData)* modelData_.vertices.size());
	

	//マテリアル用のリソース
	 materialResource = CreateBufferResource(device_, sizeof(Material));

	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	//今回は赤を書き込んでみる
	materialDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialDate->enableLighting = false;

	materialDate->uvTransform = Matrix::GetInstance()->MakeIdentity4x4();

	//wvp用のリソースを作る。TransformationMatrix一つ分のサイズを用意する
	wvpInstancingResource = CreateBufferResource(device_, sizeof(TransformationMatrix) * particleNum_);
	//書き込むためのアドレスを取得
	wvpInstancingResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	for (uint32_t i = 0; i < particleNum_; ++i) {
		
		//単位行列を書き込んでおく
		wvpData[i].WVP = Matrix::GetInstance()->MakeIdentity4x4();
		wvpData[i].World = Matrix::GetInstance()->MakeIdentity4x4();

	}

	/*平行光源用リソース関連*/
	//マテリアル用のリソース
	directionalLightResource = CreateBufferResource(device_, sizeof(DirectionalLight));
	

	//書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	//今回は白を書き込んでみる
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	directionalLightDate->direction = { 0.0f,1.0f,0.0f };

	directionalLightDate->intensity = 1.0f;
	
}


ModelData ParticleBase::MakePrimitive() {
	ModelData modelData;//構築するModelData
	modelData.vertices.push_back({ 
		{ -1.0f,1.0f,0.0f,1.0f }, { 0.0f,0.0f }, { 0.0f,0.0f,1.0f }});//左上
	modelData.vertices.push_back({
		{ 1.0f,1.0f,0.0f,1.0f }, { 1.0f,0.0f }, { 0.0f,0.0f,1.0f } });//右上
	modelData.vertices.push_back({
		{ -1.0f,-1.0f,0.0f,1.0f }, { 0.0f,1.0f }, { 0.0f,0.0f,1.0f } });//左下
	modelData.vertices.push_back({
		{ -1.0f,-1.0f,0.0f,1.0f }, { 0.0f,1.0f }, { 0.0f,0.0f,1.0f } });//左下
	modelData.vertices.push_back({
		{ 1.0f,1.0f,0.0f,1.0f }, { 1.0f,0.0f }, { 0.0f,0.0f,1.0f } });//右上
	modelData.vertices.push_back({
		{ 1.0f,-1.0f,0.0f,1.0f }, { 1.0f,1.0f }, { 0.0f,0.0f,1.0f } });//右下
	modelData.material.textureFilePath = "./resources/uvChecker.png";

	return modelData;
}
