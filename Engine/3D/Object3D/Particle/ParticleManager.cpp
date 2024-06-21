#include "ParticleManager.h"
#include"DirectXCommon.h"

void ParticleManager::Initialize(){
	device_ = DirectXCommon::GetInstance()->GetDevice();
	textureManager_ = TextureManager::GetInstance();
	random_ = RandomMaker::GetInstance();

	modelData_ = MakePrimitive();

	MakeResource();
	MakePipeLines();

	//gpuHandle_ = textureManager_->MakeInstancingShaderResourceView(GetInstancingResource());

}

void ParticleManager::Updata(){
	backToFrontMatrix_ = Matrix::GetInstance()->MakeRotateMatrix({ 0.0f,0.0f,0.0f });
	billboardMatrix_ = Matrix::GetInstance()->Multiply(backToFrontMatrix_, viewProjection_->cameraMatrix_);
	billboardMatrix_.m[3][0] = 0.0f;
	billboardMatrix_.m[3][1] = 0.0f;
	billboardMatrix_.m[3][2] = 0.0f;

	for (auto& particleGroup : particleGroups_) {
		auto& particleList = particleGroup.second.particleList;
		for (std::list<Particle>::iterator particleIterator = particleList.begin(); particleIterator != particleList.end();) {
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
				particleIterator = particleList.erase(particleIterator);
				continue;
			}


			worldMatrix_ = Matrix::GetInstance()->MakeAffineMatrix(
				Matrix::GetInstance()->MakeScaleMatrix((*particleIterator).transform.scale),
				billboardMatrix_,
				Matrix::GetInstance()->MakeTranslateMatrix((*particleIterator).transform.translate));





		}
	}
}

void ParticleManager::Draw(){

}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureName, const AccelerationField field){
	bool isCreated = false;

	for (auto& particle : particleGroups_) {
		if (particle.first == name){
			isCreated = true;
		}
	}
	if (isCreated){
		return;
	}

	ParticleGroup newParticleGroup;

	newParticleGroup.particleMaxNum_ = 600;

	newParticleGroup.accelerationField = field;

	newParticleGroup.materialData.textureFilePath = textureName;
	newParticleGroup.materialData.textureHandle = textureManager_->Load(textureName);

	//wvp用のリソースを作る。TransformationMatrix一つ分のサイズを用意する
	newParticleGroup.wvpInstancingResource = textureManager_->CreateBufferResource(sizeof(ParticleForGPU) * newParticleGroup.particleMaxNum_);
	//書き込むためのアドレスを取得
	newParticleGroup.wvpInstancingResource->Map(0, nullptr, reinterpret_cast<void**>(&newParticleGroup.wvpData));
	for (int32_t i = 0; i < newParticleGroup.particleMaxNum_; ++i) {

		//単位行列を書き込んでおく
		newParticleGroup.wvpData[i].WVP = Matrix::GetInstance()->MakeIdentity4x4();
		newParticleGroup.wvpData[i].World = Matrix::GetInstance()->MakeIdentity4x4();
		newParticleGroup.wvpData[i].color = { 1.0f,1.0f,1.0f,1.0f };
	}

	newParticleGroup.gpuHandle_ = textureManager_->MakeInstancingShaderResourceView(newParticleGroup.wvpInstancingResource.Get());

}

void ParticleManager::MakeResource(){
	//頂点リソースの作成
	vertexResource_ = textureManager_->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());


	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点三つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate_));
	std::memcpy(vertexDate_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());


	//マテリアル用のリソース
	materialResource_ = textureManager_->CreateBufferResource(sizeof(Model::Material));

	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialDate_));
	//今回は赤を書き込んでみる
	materialDate_->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialDate_->enableLighting = false;

	materialDate_->uvTransform = Matrix::GetInstance()->MakeIdentity4x4();

	for (auto& particle : particleGroups_){
		//wvp用のリソースを作る。TransformationMatrix一つ分のサイズを用意する
		particle.second.wvpInstancingResource = textureManager_->CreateBufferResource(sizeof(ParticleForGPU) * particle.second.particleMaxNum_);
		//書き込むためのアドレスを取得
		particle.second.wvpInstancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particle.second.wvpData));
		for (int32_t i = 0; i < particle.second.particleMaxNum_; ++i) {

			//単位行列を書き込んでおく
			particle.second.wvpData[i].WVP = Matrix::GetInstance()->MakeIdentity4x4();
			particle.second.wvpData[i].World = Matrix::GetInstance()->MakeIdentity4x4();
			particle.second.wvpData[i].color = { 1.0f,1.0f,1.0f,1.0f };
		}
	}
	
}

void ParticleManager::MakePipeLines(){
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
}

Model::ModelData ParticleManager::MakePrimitive() {
	Model::ModelData modelData;//構築するModelData
	modelData.vertices.push_back({
		{ -1.0f,1.0f,0.0f,1.0f }, { 0.0f,0.0f }, { 0.0f,0.0f,1.0f } });//左上
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

	return modelData;
}

bool ParticleManager::IsCollision(const AABB& aabb, const Vector3& point) {
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
