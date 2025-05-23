#include "ParticleBase.h"
#include"../../Base/DirectXCommon.h"
#include"TextureManager.h"
#include <cassert>
#include<fstream>
#include<sstream>
//#include<iostream>
//#include<algorithm>

using namespace MAEngine;

void ParticleBase::Initialize(const ParticleBase::Emitter& emitter, const bool isLoop) {

	modelData_ = MakePrimitive();

	random_ = RandomMaker::GetInstance();

	emitter_ = emitter;

	accelerationField_.acceleration = { 0.0f,30.0f,0.0f };
	accelerationField_.area.min = { -100.0f,-100.0f,-100.0f };
	accelerationField_.area.max = { 100.0f,100.0f,100.0f };

	////パーティクルの初期設定
	//particles_ = Emission(emitter_);
	
	makeResource();

	isDraw_ = true;
	isMove_ = false;

	blend_ = BlendMode::kBlendModeAdd;

	cameraTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,2.0f,-10.0f}
	};

	GraphicsPipelineParticleNone_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineParticleNone_->ParticleExclusiveInitialize(
		L"resources/shaders/particle/Particle.VS.hlsl", L"resources/shaders/particle/Particle.PS.hlsl",
		true, BlendMode::kBlendModeNone);
	GraphicsPipelineParticleNormal_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineParticleNormal_->ParticleExclusiveInitialize(
		L"resources/shaders/particle/Particle.VS.hlsl", L"resources/shaders/particle/Particle.PS.hlsl",
		true, BlendMode::kBlendModeNormal);
	GraphicsPipelineParticleAdd_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineParticleAdd_->ParticleExclusiveInitialize(
		L"resources/shaders/particle/Particle.VS.hlsl", L"resources/shaders/particle/Particle.PS.hlsl",
		true, BlendMode::kBlendModeAdd);
	GraphicsPipelineParticleSubtract_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineParticleSubtract_->ParticleExclusiveInitialize(
		L"resources/shaders/particle/Particle.VS.hlsl", L"resources/shaders/particle/Particle.PS.hlsl",
		true, BlendMode::kBlendModeSubtract);
	GraphicsPipelineParticleMultily_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineParticleMultily_->ParticleExclusiveInitialize(
		L"resources/shaders/particle/Particle.VS.hlsl", L"resources/shaders/particle/Particle.PS.hlsl",
		true, BlendMode::kBlendModeMultily);
	GraphicsPipelineParticleScreen_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineParticleScreen_->ParticleExclusiveInitialize(
		L"resources/shaders/particle/Particle.VS.hlsl", L"resources/shaders/particle/Particle.PS.hlsl",
		true, BlendMode::kBlendModeScreen);

	textureHandle_= TextureManager::GetInstance()->Load("resources/texture/circle.png");//10

	gpuHandle_ = TextureManager::GetInstance()->MakeInstancingShaderResourceView<ParticleForGPU>(GetInstancingResource(), 600);

	isWind_ = false;
	isBillborad_ = true;
	isLoop_ = isLoop;
	isMoveParticle_ = true;
	isAlignedToMovement_ = false;
}

void ParticleBase::Update(const EulerTransform& transform, const ViewProjection& viewProjection) {
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

	emitter_.transform.translate = transform.translate;

	if (!isDraw_) {
		return;
	}
	emitter_.frequencyTime += kDeltaTime_;//時刻を進める
	if (isLoop_) {
		if (emitter_.frequency <= emitter_.frequencyTime) {//頻度より大きいなら発生
			if (particles_.size() + emitter_.count < particleMaxNum_) {
				particles_.splice(particles_.end(), Emission(emitter_));//発生処理
				emitter_.frequencyTime -= emitter_.frequency;//余計に過ぎた時間も加味して頻度計算する

			}
		}
	}
	assert(particles_.size() < particleMaxNum_);
	
	Vector3 previewTrans{};

	numInstance_ = 0;
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
		if (isMoveParticle_){
			previewTrans = (*particleIterator).transform.translate;
			(*particleIterator).transform.translate += (*particleIterator).velocity * kDeltaTime_;
		}
		(*particleIterator).currentTime += kDeltaTime_;
		
		float alpha_ = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);
		worldMatrix_ = Matrix::MakeAffineMatrix((*particleIterator).transform);
		if (isBillborad_){
			backToFrontMatrix_ = Matrix::MakeRotateMatrix({ 0.0f,0.0f,0.0f });
			billboardMatrix_ = Matrix::Multiply(backToFrontMatrix_, viewProjection.cameraMatrix_);
			billboardMatrix_.m[3][0] = 0.0f;
			billboardMatrix_.m[3][1] = 0.0f;
			billboardMatrix_.m[3][2] = 0.0f;
			//移動ベクトルに合わせてz軸回転
			if (isAlignedToMovement_){
				// 移動ベクトルを正規化
				Vector3 normalizedDirection = Vector3::Normalize(((*particleIterator).velocity * kDeltaTime_));
				Vector3 defaultForward = Vector3(0, 0, 1);  // デフォルトの前方ベクトル
				float dotProduct = Vector3::Dot(defaultForward, normalizedDirection);
				float angle = acos(dotProduct);  // デフォルト前方ベクトルとの角度
				Vector3 rotationAxis = Vector3::Normalize(Vector3::Cross(defaultForward, normalizedDirection));
				
				Matrix4x4 zRotationMatrix = Matrix::MakeRotateAxisAngle(rotationAxis, angle);				

				// ビルボード行列にZ軸回転を適用
				Matrix4x4 finalBillboardMatrix = Matrix::Multiply(billboardMatrix_, zRotationMatrix);

				worldMatrix_ = Matrix::MakeAffineMatrix(
					Matrix::MakeScaleMatrix((*particleIterator).transform.scale),
					finalBillboardMatrix,
					Matrix::MakeTranslateMatrix((*particleIterator).transform.translate));
			}
			else {

				worldMatrix_ = Matrix::MakeAffineMatrix(
					Matrix::MakeScaleMatrix((*particleIterator).transform.scale),
					billboardMatrix_,
					Matrix::MakeTranslateMatrix((*particleIterator).transform.translate));

			}
		}
		else {
			if (isAlignedToMovement_) {
				// 移動ベクトルを正規化
				Vector3 direction = (*particleIterator).transform.translate - previewTrans; // 移動ベクトル
				float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
				if (length > std::numeric_limits<float>::epsilon()) {
					direction.x /= length;
					direction.y /= length;
				}
				else {
					direction.x = 1.0f; // デフォルト方向
					direction.y = 0.0f;
				}

				// Z軸回転行列を手動で構築
				Matrix4x4 zRotationMatrix;
				zRotationMatrix.m[0][0] = direction.x;
				zRotationMatrix.m[0][1] = -direction.y;
				zRotationMatrix.m[1][0] = direction.y;
				zRotationMatrix.m[1][1] = direction.x;
				zRotationMatrix.m[2][2] = 1.0f;
				zRotationMatrix.m[3][3] = 1.0f;


				worldMatrix_ = Matrix::MakeAffineMatrix(
					Matrix::MakeScaleMatrix((*particleIterator).transform.scale),
					zRotationMatrix,
					Matrix::MakeTranslateMatrix((*particleIterator).transform.translate));
			}
		}

		Matrix4x4 worldViewProjectionMatrix = Matrix::Multiply(worldMatrix_, viewProjection.matViewProjection_);
		if (numInstance_ < particleMaxNum_) {
			wvpData_[numInstance_].WVP = worldViewProjectionMatrix;
			wvpData_[numInstance_].World = worldMatrix_;
			wvpData_[numInstance_].color = (*particleIterator).color;
			wvpData_[numInstance_].color.w = alpha_;
			++numInstance_;
		}
		++particleIterator;
	}
}

void ParticleBase::Draw() {

	if (!isDraw_) {
		return;
	}
	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	if (blend_ == BlendMode::kBlendModeNone) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineParticleNone_->GetParticleRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineParticleNone_->GetPipeLineState());
	}
	else if (blend_ == BlendMode::kBlendModeNormal){
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineParticleNormal_->GetParticleRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineParticleNormal_->GetPipeLineState());
	}
	else if (blend_ == BlendMode::kBlendModeAdd) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineParticleAdd_->GetParticleRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineParticleAdd_->GetPipeLineState());
	}
	else if (blend_ == BlendMode::kBlendModeSubtract) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineParticleSubtract_->GetParticleRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineParticleSubtract_->GetPipeLineState());
	}
	else if (blend_ == BlendMode::kBlendModeMultily) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineParticleMultily_->GetParticleRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineParticleMultily_->GetPipeLineState());
	}
	else if (blend_ == BlendMode::kBlendModeScreen) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineParticleScreen_->GetParticleRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineParticleScreen_->GetPipeLineState());
	}
	

	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(1, gpuHandle_);
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->SendGPUDescriptorHandle(textureHandle_));

	//3D三角の描画
	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), numInstance_, 0, 0);
		
	
}

void ParticleBase::DrawImgui(const std::string& imguiTag){
	imguiTag;
#ifdef _DEBUG
	ImGui::Begin(imguiTag.c_str());
	ImGui::Text("現在のパーティクルの数 = %d", particles_.size());
	ImGui::DragFloat3("発生中心位置", &emitter_.transform.translate.x, 0.01f, -100.0f, 100.0f);
	ImGui::DragFloat3("一粒の大きさ", &emitter_.transform.scale.x, 0.01f, 0.01f, 50.0f);
	ImGui::DragFloat2("移動ベクトル範囲", &velocityRange_.min, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat2("ランダム発生範囲", &positionRange_.min, 0.1f, -10.0f, 10.0f);
	ImGui::DragFloat("発生までの時間", &emitter_.frequency, 0.01f, 0.0f, 3.0f);
	ImGui::SliderInt("一度に発生する個数", &emitter_.count, 0, 6);
	ImGui::Checkbox("描画するか", &isDraw_);
	ImGui::Checkbox("ビルボード有効化", &isBillborad_);
	ImGui::Checkbox("フィールド有効化", &isWind_);
	if (isWind_){
		ImGui::DragFloat3("風向き", &accelerationField_.acceleration.x, 0.1f);
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
	if (ImGui::TreeNode("ブレンドモード変更")) {
		ImGui::RadioButton("なし", &blend_, BlendMode::kBlendModeNone);
		ImGui::SameLine();
		ImGui::RadioButton("ノーマル", &blend_, BlendMode::kBlendModeNormal);
		ImGui::SameLine();
		ImGui::RadioButton("加算", &blend_, BlendMode::kBlendModeAdd);
		
		ImGui::RadioButton("減算", &blend_, BlendMode::kBlendModeSubtract);
		ImGui::SameLine();
		ImGui::RadioButton("乗算　　", &blend_, BlendMode::kBlendModeMultily);
		ImGui::SameLine();
		ImGui::RadioButton("スクリーン", &blend_, BlendMode::kBlendModeScreen);

		ImGui::Text("今のブレンド %d", blend_);

		ImGui::TreePop();
	}
	ImGui::End();
#endif
}

Microsoft::WRL::ComPtr<ID3D12Resource> ParticleBase::CreateBufferResource(size_t sizeInBytes) {

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
	hr_ = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(hr_));

	return bufferResource;
}

void ParticleBase::makeResource() {
	//頂点リソースの作成
	vertexResource_ = CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点三つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate_));
	std::memcpy(vertexDate_, modelData_.vertices.data(), sizeof(VertexData)* modelData_.vertices.size());
	

	//マテリアル用のリソース
	 materialResource_ = CreateBufferResource(sizeof(Model::Material));

	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialDate_));
	//今回は赤を書き込んでみる
	materialDate_->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialDate_->enableLighting = false;

	materialDate_->uvTransform = Matrix::MakeIdentity4x4();

	//wvp用のリソースを作る。TransformationMatrix一つ分のサイズを用意する
	wvpInstancingResource_ = CreateBufferResource(sizeof(ParticleForGPU) * particleMaxNum_);
	//書き込むためのアドレスを取得
	wvpInstancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	for (uint32_t i = 0; i < particleMaxNum_; ++i) {
		
		//単位行列を書き込んでおく
		wvpData_[i].WVP = Matrix::MakeIdentity4x4();
		wvpData_[i].World = Matrix::MakeIdentity4x4();
		wvpData_[i].color = { 1.0f,1.0f,1.0f,1.0f };
	}

}


void ParticleBase::AddParticle(const ParticleBase::Emitter& emitter){
	emitter_.count = emitter.count;
	emitter_.transform.scale = emitter.transform.scale;
	if (!isLoop_){
		if (particles_.size() + emitter_.count < particleMaxNum_) {
			particles_.splice(particles_.end(), Emission(emitter_));//発生処理
		}
	}
}

Model::ModelData ParticleBase::MakePrimitive() {
	Model::ModelData modelData;//構築するModelData
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
		if (isMoveParticle_) {
			(*particleIterator).velocity = random_->DistributionV3(velocityRange_.min, velocityRange_.max);
		}
		(*particleIterator).color = { color_.x,color_.y,color_.z,1.0f };
		(*particleIterator).currentTime = 0;
		if (lifeTime_ != 0) {
			(*particleIterator).lifeTime = lifeTime_;
		}
		else {
			(*particleIterator).lifeTime = random_->Distribution(1.0f, 3.0f);
		}
	}
}

ParticleBase::Particle ParticleBase::MakeNewParticle(const Vector3& transform){
	Particle particle{};
	if (!isOneColor_) {
		color_ = random_->DistributionV3(0.0f, 1.0f);
	}
	particle.transform.scale = emitter_.transform.scale;
	particle.transform.rotate = { 0.0f,0.0f,0.0f };
	particle.transform.translate = random_->DistributionV3(positionRange_.min / 2.0f, positionRange_.max / 2.0f) + transform;
	if (isMoveParticle_) {
		particle.velocity = random_->DistributionV3(velocityRange_.min, velocityRange_.max);
		if (isUpper_){
			if (particle.velocity.y <= 0.0f) {
				particle.velocity.y *= -1.0f;
			}
		}
		particle.transform.translate = transform;
	}
	particle.color = { color_.x,color_.y,color_.z,1.0f };
	particle.currentTime = 0.0f;
	if (lifeTime_ != 0) {
		particle.lifeTime = lifeTime_;
	}
	else {
		particle.lifeTime = random_->Distribution(1.0f, 3.0f);
	}
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

