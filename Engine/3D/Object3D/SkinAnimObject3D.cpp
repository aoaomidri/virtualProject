#include"SkinAnimObject3D.h"
#include"DirectXCommon.h"
#include <cassert>
#include<fstream>
#include<sstream>


void SkinAnimObject3D::Initialize(const std::string fileName) {
	makeResource();

	isDraw_ = true;

	materialDate->enableLighting = false;

	model_ = Model::LoadModelFile(fileName);

	animation_ = Model::LoadAnimationFile(fileName);

	skeleton_ = model_->CreateSkeleton(model_->GetNode());

	if (model_->GetMaterial().textureFilePath != "") {
		texHandle_ = TextureManager::GetInstance()->Load(model_->GetMaterial().textureFilePath);
	}
	else {
		texHandle_ = 0;
	}

	transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	
}

void SkinAnimObject3D::Initialize(const std::string fileName, const std::string& modelName) {
	makeResource();

	isDraw_ = true;

	materialDate->enableLighting = false;

	model_ = Model::LoadModelFile(fileName, modelName);

	animation_ = Model::LoadAnimationFile(fileName, modelName);

	skeleton_ = model_->CreateSkeleton(model_->GetNode());

	SkeletonUpdate(skeleton_);

	if (model_->GetMaterial().textureFilePath != "") {
		texHandle_ = TextureManager::GetInstance()->Load(model_->GetMaterial().textureFilePath);
	}
	else {
		texHandle_ = 0;
	}

	transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};


}

void SkinAnimObject3D::Update(const Matrix4x4& worldMatrix, const ViewProjection& viewProjection) {
	if (!isDraw_) {
		return;
	}


	if (animation_.duration != 0) {
		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, animation_.duration);
		ApplyAnimation(skeleton_, animation_, animationTime);
		SkeletonUpdate(skeleton_);
		
	}
	else {
		localMatrix_ = model_->GetLocalMatrix();
	}

	worldMatrix_ = worldMatrix;
	if (parent_){
		worldMatrix_ = Matrix::GetInstance()->Multiply(worldMatrix, *parent_);
	}

	
	Matrix4x4 worldViewProjectionMatrix = Matrix::GetInstance()->Multiply(worldMatrix_, viewProjection.matViewProjection_);
	

	wvpData->WVP = worldViewProjectionMatrix;
	materialDate->enableLighting = isUseLight_;

	wvpData->World = worldViewProjectionMatrix;
	wvpData->WorldInverseTranspose = Matrix::GetInstance()->Inverce(Matrix::GetInstance()->Trnaspose(worldMatrix_));
			
	if (directionalLight){
	directionalLightDate->color = directionalLight->color;
	directionalLightDate->direction = directionalLight->direction;
	directionalLightDate->intensity = directionalLight->intensity;

	}
	if (pointLight){
	pointLightData->color = pointLight->color;
	pointLightData->position = pointLight->position;
	pointLightData->intensity = pointLight->intensity;
	pointLightData->radius = pointLight->radius;
	pointLightData->decay = pointLight->decay;

	}

	materialDate->shininess = shininess_;

	cameraForGPU_->worldPosition = viewProjection.translation_;
}

void SkinAnimObject3D::SkeletonUpdate(Model::Skeleton& skeleton){
	//全てのJointを更新。親が若いので通常ループで処理可能になっている
	for (Model::Joint& joint : skeleton.joints) {
		joint.localMatrix = Matrix::GetInstance()->MakeAffineMatrix(joint.transform);
		if (joint.parent) {//親がいれば親の行列を掛ける
			joint.skeltonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeltonSpaceMatrix;
		}
		else {//親がいないのでlocalMatrixとskeletonspacematrixは一致する
			joint.skeltonSpaceMatrix = joint.localMatrix;
		}
	}
}

void SkinAnimObject3D::Draw() {

	if (!isDraw_) {
		return;
	}
	model_->Draw(DirectXCommon::GetInstance()->GetCommandList());
	//形状を設定。
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->SendGPUDescriptorHandle(texHandle_));
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());
	//3D三角の描画
	DirectXCommon::GetInstance()->GetCommandList()->DrawIndexedInstanced(static_cast<uint32_t>(model_->GetIndexData().size()), 1, 0, 0, 0);


}

void SkinAnimObject3D::DrawImgui(std::string name){
#ifdef _DEBUG
	ImGui::Begin((name + "オブジェの内部設定").c_str());
	ImGui::Text("アニメーションの時間 = %.1f", animationTime);
	ImGui::Checkbox("描画するかどうか", &isDraw_);
	for (int i = 0; i < 4; i++){
		ImGui::DragFloat4(("LocalMat1" + std::to_string(i)).c_str(), skeleton_.joints[0].skeltonSpaceMatrix.m[i], 0.1f);
	}
	for (int i = 0; i < 4; i++) {
		ImGui::DragFloat4(("LocalMat2" + std::to_string(i)).c_str(), skeleton_.joints[1].skeltonSpaceMatrix.m[i], 0.1f);
	}
	for (int i = 0; i < 4; i++) {
		ImGui::DragFloat4(("LocalMat3" + std::to_string(i)).c_str(), skeleton_.joints[2].skeltonSpaceMatrix.m[i], 0.1f);
	}
	ImGui::End();
#endif
}

void SkinAnimObject3D::SetDirectionalLight(const Model::DirectionalLight* light){
	directionalLight = light;
}

void SkinAnimObject3D::SetPointLight(const Model::PointLight* pLight){
	pointLight = pLight;
}

Microsoft::WRL::ComPtr<ID3D12Resource> SkinAnimObject3D::CreateBufferResource(size_t sizeInBytes) {

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
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(hr));

	return bufferResource;
}

void SkinAnimObject3D::makeResource() {
	//マテリアル用のリソース
	 materialResource = CreateBufferResource(sizeof(Model::Material));

	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	//今回は赤を書き込んでみる
	materialDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialDate->enableLighting = false;

	materialDate->uvTransform.Identity();

	materialDate->shininess = 1.0f;

	//wvp用のリソースを作る。TransformationMatrix一つ分のサイズを用意する
	wvpResource = CreateBufferResource(sizeof(TransformationMatrix));
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	wvpData->WVP = Matrix::GetInstance()->MakeIdentity4x4();
	wvpData->World = Matrix::GetInstance()->MakeIdentity4x4();
	wvpData->WorldInverseTranspose = Matrix::GetInstance()->MakeIdentity4x4();

	/*平行光源用リソース関連*/
	//マテリアル用のリソース
	directionalLightResource = CreateBufferResource(sizeof(Model::DirectionalLight));

	//書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	//今回は白を書き込んでみる
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };

	directionalLightDate->intensity = 1.0f;

	//マテリアル用のリソース
	pointLightResource = CreateBufferResource(sizeof(Model::PointLight));

	//書き込むためのアドレスを取得
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));
	//今回は白を書き込んでみる
	pointLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	pointLightData->position = { 0.0f,10.0f,0.0f };

	pointLightData->intensity = 1.0f;

	/*カメラリソース関連*/
	cameraResource_ = CreateBufferResource(sizeof(CameraForGPU));

	//書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPU_));

	cameraForGPU_->worldPosition = { 0.0f,0.0f,0.0f };


}

Vector3 SkinAnimObject3D::CalculateValue(const std::vector<Model::KeyframeVector3>& keyframes, float time){
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		//indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			//範囲内を補完する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Vector3::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}

Quaternion SkinAnimObject3D::CalculateValue(const std::vector<Model::KeyframeQuaternion>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; index++) {
		size_t nextIndex = index + 1;
		//indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			//範囲内を補完する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Quaternion::GetInstance()->Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}

void SkinAnimObject3D::ApplyAnimation(Model::Skeleton& skeleton, Model::Animation& animation, float animationTime){
	for (Model::Joint& joint :skeleton.joints){
		//対象のJointのAnimationがあれば、値の適用を行う。下記のif文はc++17から可能になった初期化付きif文
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const Model::NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = CalculateValue(rootNodeAnimation.translate, animationTime);
			joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate, animationTime);
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale, animationTime);

		}
	}


}


