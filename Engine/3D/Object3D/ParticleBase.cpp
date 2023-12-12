#include "ParticleBase.h"
#include"../../Base/DirectXCommon.h"
#include <cassert>
#include<fstream>
#include<sstream>


void ParticleBase::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {

	device_ = device;
	commandList_ = commandList;

	modelData_ = MakePrimitive();

	random_ = RandomMaker::GetInstance();

	//パーティクルの初期設定
	for (int i = 0; i < particleNum_; i++) {
		particles_[i] = MakeNewParticle();
	}
	
	makeResource();

	isDraw_ = true;
	isMove_ = false;

	

	cameraTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,2.0f,-10.0f}
	};
}

void ParticleBase::Update(const Transform& transform, const ViewProjection& viewProjection) {
	/*imguiで値がおかしくならないように調整*/
	if (positionRange_.min >= positionRange_.max) {
		positionRange_.min = positionRange_.max - 0.1f;
	}
	else if (positionRange_.max < positionRange_.min) {
		positionRange_.max = positionRange_.min + 0.1f;
	}
	if (velocityRange_.min >= velocityRange_.max) {
		velocityRange_.min = velocityRange_.max - 0.1f;
	}
	else if (velocityRange_.max < velocityRange_.min) {
		velocityRange_.max = velocityRange_.min + 0.1f;
	}

	if (!isDraw_) {
		return;
	}

	for (int i = 0; i < particleNum_; i++) {
		if (isMove_){
			particles_[i].transform.translate += particles_[i].velocity * kDeltaTime_;
		}


		worldMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(particles_[i].transform);

		Matrix4x4 worldViewProjectionMatrix = Matrix::GetInstance()->Multiply(worldMatrix_, viewProjection.matViewProjection_);
		wvpData[i].WVP = worldViewProjectionMatrix;
		wvpData[i].World = worldMatrix_;
	}
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
	ImGui::Begin("パーティクルのあれこれ");
	ImGui::DragFloat2("移動ベクトル", &velocityRange_.min, 0.1f, -5.0f, 5.0f);
	ImGui::DragFloat2("初期位置", &positionRange_.min, 0.1f, -5.0f, 5.0f);
	ImGui::Checkbox("描画するか", &isDraw_);
	ImGui::Checkbox("動かすか", &isMove_);
	if (ImGui::Button("座標リセット")){
		PositionReset();
	}
	if (ImGui::Button("動きや初期座標をランダムに変更")) {
		MoveChange();
	}
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
	wvpInstancingResource = CreateBufferResource(device_, sizeof(ParticleForGPU) * particleNum_);
	//書き込むためのアドレスを取得
	wvpInstancingResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	for (uint32_t i = 0; i < particleNum_; ++i) {
		
		//単位行列を書き込んでおく
		wvpData[i].WVP = Matrix::GetInstance()->MakeIdentity4x4();
		wvpData[i].World = Matrix::GetInstance()->MakeIdentity4x4();
		wvpData[i].color = particles_[i].color;
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

void ParticleBase::PositionReset(){
	for (int i = 0; i < particleNum_; i++) {
		particles_[i].transform.translate = { i * 0.5f,i * 0.5f, i * 0.5f };
	}

}

void ParticleBase::MoveChange(){
	for (int i = 0; i < particleNum_; i++) {
		color_ = random_->DistributionV3(0.0f, 1.0f);
		particles_[i].transform.translate = random_->DistributionV3(positionRange_.min, positionRange_.max);
		particles_[i].velocity = random_->DistributionV3(velocityRange_.min, velocityRange_.max);
		particles_[i].color = { color_.x,color_.y,color_.z,1.0f };
	}
}

ParticleBase::Particle ParticleBase::MakeNewParticle(){
	Particle particle{};
	color_ = random_->DistributionV3(0.0f, 1.0f);
	particle.transform.scale = { 2.0f,2.0f,2.0f };
	particle.transform.rotate = { 0.0f,0.0f,0.0f };
	particle.transform.translate = random_->DistributionV3(positionRange_.min, positionRange_.max);
	
	particle.velocity = random_->DistributionV3(velocityRange_.min, velocityRange_.max);

	particle.color = { color_.x,color_.y,color_.z,1.0f };
	return particle;
}
