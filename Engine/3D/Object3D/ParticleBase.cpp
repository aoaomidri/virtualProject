#include "ParticleBase.h"
#include"../../Base/DirectXCommon.h"
#include <cassert>
#include<fstream>
#include<sstream>
//#include<iostream>
//#include<algorithm>


void ParticleBase::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {

	device_ = device;
	commandList_ = commandList;

	modelData_ = MakePrimitive();

	random_ = RandomMaker::GetInstance();

	emitter_.count = 3;
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;
	emitter_.transform.scale = { 1.0f,1.0f,1.0f };
	emitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	emitter_.transform.translate = { 0.0f,0.0f,0.0f };

	accelerationField_.acceleration = { 15.0f,0.0f,0.0f };
	accelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField_.area.max = { 1.0f,1.0f,1.0f };

	//パーティクルの初期設定
	for (int i = 0; i < 3; i++) {
		particles_.push_back(MakeNewParticle(emitter_.transform.translate));
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
	emitter_.frequencyTime += kDeltaTime_;//時刻を進める
	if (emitter_.frequency <= emitter_.frequencyTime) {//頻度より大きいなら発生
		particles_.splice(particles_.end(), Emission(emitter_));//発生処理
		emitter_.frequencyTime -= emitter_.frequency;//余計に過ぎた時間も加味して頻度計算する
	}

	assert(particles_.size() < 100);
	
	numInstance = 0;
	for (std::list<Particle>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end();) {
		if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
			particleIterator = particles_.erase(particleIterator);
			continue;
		}
		if (isWind_){
			if (IsCollision(accelerationField_.area,(*particleIterator).transform.translate)){
				(*particleIterator).velocity += accelerationField_.acceleration * kDeltaTime_;
			}
		}
		
		(*particleIterator).transform.translate += (*particleIterator).velocity * kDeltaTime_;
		(*particleIterator).currentTime += kDeltaTime_;
		
		float alpha_ = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);
		worldMatrix_ = Matrix::GetInstance()->MakeAffineMatrix((*particleIterator).transform);
		if (isBillborad_){
			backToFrontMatrix_ = Matrix::GetInstance()->MakeRotateMatrix({ 0.0f,0.0f,0.0f });
			billboardMatrix_ = Matrix::GetInstance()->Multiply(backToFrontMatrix_, viewProjection.cameraMatrix_);
			billboardMatrix_.m[3][0] = 0.0f;
			billboardMatrix_.m[3][1] = 0.0f;
			billboardMatrix_.m[3][2] = 0.0f;
			worldMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(
				Matrix::GetInstance()->MakeScaleMatrix((*particleIterator).transform.scale),
				billboardMatrix_,
				Matrix::GetInstance()->MakeTranslateMatrix((*particleIterator).transform.translate));
		}
		


		Matrix4x4 worldViewProjectionMatrix = Matrix::GetInstance()->Multiply(worldMatrix_, viewProjection.matViewProjection_);
		if (numInstance < particleMaxNum_) {
			wvpData[numInstance].WVP = worldViewProjectionMatrix;
			wvpData[numInstance].World = worldMatrix_;
			wvpData[numInstance].color = (*particleIterator).color;
			wvpData[numInstance].color.w = alpha_;
			++numInstance;
		}
		++particleIterator;
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
	commandList_->DrawInstanced(UINT(modelData_.vertices.size()), numInstance, 0, 0);
		
	
}

void ParticleBase::DrawImgui(){
	ImGui::Begin("パーティクルのあれこれ");
	ImGui::Text("現在のパーティクルの数 = %d", particles_.size());
	ImGui::DragFloat3("発生中心位置", &emitter_.transform.translate.x, 0.01f, -100.0f, 100.0f);
	ImGui::DragFloat2("移動ベクトル範囲", &velocityRange_.min, 0.1f, -5.0f, 5.0f);
	ImGui::DragFloat2("ランダム発生範囲", &positionRange_.min, 0.1f, -5.0f, 5.0f);
	ImGui::DragFloat("発生までの時間", &emitter_.frequency, 0.01f, 0.0f, 3.0f);
	ImGui::SliderInt("一度に発生する個数", &emitter_.count, 0, 6);
	ImGui::Checkbox("描画するか", &isDraw_);
	ImGui::Checkbox("ビルボード有効化", &isBillborad_);
	ImGui::Checkbox("フィールド有効化", &isWind_);
	if (isWind_){
		ImGui::DragFloat3("フィールドの範囲設定(最小)", &accelerationField_.area.min.x, 0.1f, 0.0f, -100.0f);
		ImGui::DragFloat3("フィールドの範囲設定(最大)", &accelerationField_.area.max.x, 0.1f, 0.0f, 100.0f);
	}
	if (ImGui::Button("座標リセット")){
		PositionReset();
	}
	if (ImGui::Button("動きや初期座標をランダムに変更")) {
		MoveChange();
	}
	if (ImGui::Button("パーティクル追加")) {
		particles_.splice(particles_.end(), Emission(emitter_));
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
	wvpInstancingResource = CreateBufferResource(device_, sizeof(ParticleForGPU) * particleMaxNum_);
	//書き込むためのアドレスを取得
	wvpInstancingResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	for (uint32_t i = 0; i < particleMaxNum_; ++i) {
		
		//単位行列を書き込んでおく
		wvpData[i].WVP = Matrix::GetInstance()->MakeIdentity4x4();
		wvpData[i].World = Matrix::GetInstance()->MakeIdentity4x4();
		wvpData[i].color = { 1.0f,1.0f,1.0f,1.0f };
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
	for (std::list<Particle>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end(); ++particleIterator) {
		(*particleIterator).transform.translate = random_->DistributionV3(positionRange_.min, positionRange_.max);
	}

}

void ParticleBase::MoveChange(){
	for (std::list<Particle>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end(); ++particleIterator) {
		color_ = random_->DistributionV3(0.0f, 1.0f);
		(*particleIterator).transform.translate = random_->DistributionV3(positionRange_.min, positionRange_.max);
		(*particleIterator).velocity = random_->DistributionV3(velocityRange_.min, velocityRange_.max);
		(*particleIterator).color = { color_.x,color_.y,color_.z,1.0f };
		(*particleIterator).currentTime = 0;
		(*particleIterator).lifeTime = random_->Distribution(1.0f, 3.0f);
	}
}

ParticleBase::Particle ParticleBase::MakeNewParticle(const Vector3& transform){
	Particle particle{};
	color_ = random_->DistributionV3(0.0f, 1.0f);
	particle.transform.scale = { 2.0f,2.0f,2.0f };
	particle.transform.rotate = { 0.0f,0.0f,0.0f };
	particle.transform.translate = random_->DistributionV3(positionRange_.min, positionRange_.max) + transform;
	
	particle.velocity = random_->DistributionV3(velocityRange_.min, velocityRange_.max);

	particle.color = { color_.x,color_.y,color_.z,1.0f };
	particle.currentTime = 0.0f;
	particle.lifeTime = random_->Distribution(1.0f, 3.0f);
	return particle;
}

std::list<ParticleBase::Particle> ParticleBase::Emission(const Emitter& emitter){
	std::list<Particle> particles;
	for (int count = 0; count < emitter.count; ++count){
		particles.push_back(MakeNewParticle(emitter.transform.translate));
	}

	return particles;
}

bool ParticleBase::IsCollision(const AABB& aabb, const Vector3& point){
	//最近接点を求める
	Vector3 closestPoint{
		std::clamp(point.x,aabb.min.x,aabb.max.x),
		std::clamp(point.y,aabb.min.y,aabb.max.y),
		std::clamp(point.z,aabb.min.z,aabb.max.z)
	};

	float distance = Vector3::Length(closestPoint - point);

	if (distance <= 0.0f) {
		return true;
	}
	else {
		return false;
	}
}

//bool IsCollision(const Sphere& sphere, const AABB& aabb) {
//
//	//最近接点を求める
//	Vector3 closestPoint{
//		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
//		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
//		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z)
//	};
//
//	float distance = Vector3::Length(closestPoint - sphere.center);
//
//	if (distance <= sphere.radius) {
//		return true;
//	}
//	else {
//		return false;
//	}
//
//}