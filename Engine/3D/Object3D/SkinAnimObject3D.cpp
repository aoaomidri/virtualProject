#include"SkinAnimObject3D.h"
#include"DirectXCommon.h"
#include <cassert>
#include<fstream>
#include<sstream>



void SkinAnimObject3D::Initialize(const std::string fileName, const bool isLoop) {
	makeResource();

	isDraw_ = true;

	materialDate_->enableLighting = false;

	model_ = Model::LoadModelFile(fileName);

	nowAnimName_ = fileName;

	beforeAnimation_ = Model::LoadAnimationFile(fileName);
	beforeAnimation_.isAnimLoop = isLoop;

	animations_.push_back({ fileName, beforeAnimation_ });

	animationName_.reserve(animations_.size()); // ベクトルの領域を事前に確保

	for (const auto& animation : animations_) {
		animationName_.push_back(animation.first);
	}

	skeleton_ = model_->CreateSkeleton(model_->GetNode());

	skinCluster_ = model_->CreateSkinCluster(skeleton_);

	SkeletonUpdate(skeleton_);

	SkeletonUpdate(skinCluster_, skeleton_);

	if (model_->GetMaterial().textureFilePath != "") {
		texHandle_ = TextureManager::GetInstance()->Load(model_->GetMaterial().textureFilePath);
	}
	else {
		texHandle_ = 0;
	}

	transform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	
}

void SkinAnimObject3D::Initialize(const std::string fileName, const std::string& modelName, const bool isLoop) {
	makeResource();

	isDraw_ = true;

	materialDate_->enableLighting = false;

	model_ = Model::LoadModelFile(fileName, modelName);

	nowAnimName_ = modelName;

	beforeAnimation_ = Model::LoadAnimationFile(fileName, modelName);
	beforeAnimation_.isAnimLoop = isLoop;


	animations_.push_back({ modelName, beforeAnimation_ });

	animationName_.reserve(animations_.size()); // ベクトルの領域を事前に確保

	for (const auto& animation : animations_) {
		animationName_.push_back(animation.first);
	}

	skeleton_ = model_->CreateSkeleton(model_->GetNode());

	skinCluster_ = model_->CreateSkinCluster(skeleton_);

	SkeletonUpdate(skeleton_);

	SkeletonUpdate(skinCluster_, skeleton_);

	if (model_->GetMaterial().textureFilePath != "") {
		texHandle_ = TextureManager::GetInstance()->Load(model_->GetMaterial().textureFilePath);
	}
	else {
		texHandle_ = 0;
	}

	transform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};


}

void SkinAnimObject3D::Update(const ViewProjection& viewProjection) {
	if (!isDraw_) {
		return;
	}
	if (beforeAnimation_.duration != 0) {

		if (isAnimation_){
			animationTime_ += (1.0f / 60.0f) * animSpeed_;
		}
		
		if (beforeAnimation_.isAnimLoop){
			animationTime_ = std::fmod(animationTime_, beforeAnimation_.duration);
		}
		if (afterAnimation_.duration != 0) {
			blendFactor_ += (1.0f / 60.0f) * changeAnimSpeed_;
		}

		
				

		ApplyAnimation(skeleton_, beforeAnimation_,afterAnimation_, animationTime_);
		SkeletonUpdate(skeleton_);

		SkeletonUpdate(skinCluster_, skeleton_);

		if (blendFactor_ >= 1.0f) {
			blendFactor_ = 0;
			beforeAnimation_ = afterAnimation_;
			afterAnimation_ = Model::Animation();
		}
		
	}
	else {
		localMatrix_ = model_->GetLocalMatrix();
	}

	if (setMatrix_.m[3][3] != 0) {
		worldMatrix_ = setMatrix_;
	}
	else {
		worldMatrix_.MakeAffineMatrix(transform_);
	}
	if (parent_){
		worldMatrix_.Multiply(*parent_);
	}

	
	Matrix4x4 worldViewProjectionMatrix = Matrix::GetInstance()->Multiply(worldMatrix_, viewProjection.matViewProjection_);
	

	wvpData_->WVP = worldViewProjectionMatrix;
	materialDate_->enableLighting = isUseLight_;

	wvpData_->World = worldMatrix_;
	wvpData_->WorldInverseTranspose = Matrix::GetInstance()->Inverce(Matrix::GetInstance()->Transpose(worldMatrix_));
			
	if (directionalLight_){
		directionalLightDate_->color = directionalLight_->color;
		directionalLightDate_->direction = directionalLight_->direction;
		directionalLightDate_->intensity = directionalLight_->intensity;
	}
	if (pointLight_){
		pointLightData_->color = pointLight_->color;
		pointLightData_->position = pointLight_->position;
		pointLightData_->intensity = pointLight_->intensity;
		pointLightData_->radius = pointLight_->radius;
		pointLightData_->decay = pointLight_->decay;
	}

	materialDate_->shininess = shininess_;

	cameraForGPU_->worldPosition = viewProjection.translation_;
}


void SkinAnimObject3D::Draw() {

	if (!isDraw_) {
		return;
	}
	const auto texManagerIns = TextureManager::GetInstance();
	model_->Draw(DirectXCommon::GetInstance()->GetCommandList());

	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		model_->GetVertexBufferView(),
		skinCluster_.influenceBufferView//INfluenceのVBV
	};

	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 2, vbvs);

	//形状を設定。
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, texManagerIns->SendGPUDescriptorHandle(texHandle_));
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(3, texManagerIns->SendGPUDescriptorHandle(texManagerIns->GetSkyBoxTex()));
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(4, skinCluster_.paletteSrvHandle.second);
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, directionalLightResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, cameraResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(7, pointLightResource_->GetGPUVirtualAddress());
	//3D三角の描画
	DirectXCommon::GetInstance()->GetCommandList()->DrawIndexedInstanced(static_cast<uint32_t>(model_->GetIndexData().size()), 1, 0, 0, 0);


}

void SkinAnimObject3D::DrawImgui(std::string name){
#ifdef _DEBUG
	ImGui::Begin((name + "オブジェの内部設定").c_str());
	ImGui::Text("アニメーションの時間 = %.1f", animationTime_);
	ImGui::Text("アニメーション補完の時間 = %.1f", blendFactor_);
	ImGui::DragFloat("アニメーション補完の速度", &changeAnimSpeed_, 0.01f, 0.1f, 60.0f);
	ImGui::Checkbox("描画するかどうか", &isDraw_);
	ImGui::Checkbox("ライティングするかどうか", &isUseLight_);
	ImGui::DragFloat("反射強度", &shininess_, 0.01f, 0.0f, 100.0f);
	ImGui::ColorEdit4("ライトの色", &directionalLightDate_->color.x);
	ImGui::DragFloat3("ライトの向き", &directionalLightDate_->direction.x, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat("ライトの輝き", &directionalLightDate_->intensity, 0.01f, 0.0f, 1.0f);
	ImGui::End();
#endif
}

void SkinAnimObject3D::SetDirectionalLight(const DirectionalLight::DirectionalLightData* light){
	directionalLight_ = light;
}
void SkinAnimObject3D::SetPointLight(const PointLight::PointLightData* pLight){
	pointLight_ = pLight;
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
	 materialResource_ = CreateBufferResource(sizeof(Model::Material));

	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialDate_));
	//今回は赤を書き込んでみる
	materialDate_->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialDate_->enableLighting = true;

	materialDate_->uvTransform.Identity();

	materialDate_->shininess = 1.0f;

	//wvp用のリソースを作る。TransformationMatrix一つ分のサイズを用意する
	wvpResource_ = CreateBufferResource(sizeof(TransformationMatrix));
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおく
	wvpData_->WVP = Matrix::GetInstance()->MakeIdentity4x4();
	wvpData_->World = Matrix::GetInstance()->MakeIdentity4x4();
	wvpData_->WorldInverseTranspose = Matrix::GetInstance()->MakeIdentity4x4();

	/*平行光源用リソース関連*/
	//マテリアル用のリソース
	directionalLightResource_ = CreateBufferResource(sizeof(DirectionalLight::DirectionalLightData));

	//書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate_));
	//今回は白を書き込んでみる
	directionalLightDate_->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	directionalLightDate_->direction = { 0.0f,-1.0f,0.0f };

	directionalLightDate_->intensity = 1.0f;

	//マテリアル用のリソース
	pointLightResource_ = CreateBufferResource(sizeof(PointLight::PointLightData));

	//書き込むためのアドレスを取得
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	//今回は白を書き込んでみる
	pointLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	pointLightData_->position = { 0.0f,10.0f,0.0f };

	pointLightData_->intensity = 1.0f;

	pointLightData_->radius = 10.0f;

	pointLightData_->decay = 1.0f;

	/*カメラリソース関連*/
	cameraResource_ = CreateBufferResource(sizeof(CameraForGPU));

	//書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPU_));

	cameraForGPU_->worldPosition = { 0.0f,0.0f,0.0f };


}

void SkinAnimObject3D::SkeletonUpdate(Model::Skeleton& skeleton){
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

void SkinAnimObject3D::SkeletonUpdate(Model::SkinCluster& skinCluster, Model::Skeleton& skeleton){
	//全てのJointを更新。親が若いので通常ループで処理可能になっている
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex){
		assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());

		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix =
			skinCluster.inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeltonSpaceMatrix;

		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
			Matrix::GetInstance()->Transpose(Matrix::GetInstance()->Inverce(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}	
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

void SkinAnimObject3D::ApplyAnimation(Model::Skeleton& skeleton, Model::Animation& beforeAnimation, Model::Animation& afterAnimation, float animationTime){
	for (Model::Joint& joint :skeleton.joints){
		////対象のJointのAnimationがあれば、値の適用を行う。下記のif文はc++17から可能になった初期化付きif文
		//if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
		//	const Model::NodeAnimation& rootNodeAnimation = (*it).second;
		//	joint.transform.translate = CalculateValue(rootNodeAnimation.translate, animationTime);
		//	joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate, animationTime);
		//	joint.transform.scale = CalculateValue(rootNodeAnimation.scale, animationTime);
		//}
		if (auto it1 = beforeAnimation.nodeAnimations.find(joint.name); it1 != beforeAnimation.nodeAnimations.end()) {
			const Model::NodeAnimation& anim1 = it1->second;
			Vector3 translate1 = CalculateValue(anim1.translate, animationTime);
			Quaternion rotate1 = CalculateValue(anim1.rotate, animationTime);
			Vector3 scale1 = CalculateValue(anim1.scale, animationTime);

			if (auto it2 = afterAnimation.nodeAnimations.find(joint.name); it2 != afterAnimation.nodeAnimations.end()) {
				const Model::NodeAnimation& anim2 = it2->second;
				Vector3 translate2 = CalculateValue(anim2.translate, animationTime);
				Quaternion rotate2 = CalculateValue(anim2.rotate, animationTime);
				Vector3 scale2 = CalculateValue(anim2.scale, animationTime);

				// 補完
				joint.transform.translate = Vector3::Lerp(translate1, translate2, blendFactor_);
				joint.transform.rotate = Quaternion::GetInstance()->Slerp(rotate1, rotate2, blendFactor_);
				joint.transform.scale = Vector3::Lerp(scale1, scale2, blendFactor_);
			}
			else {
				// animation2に対応するジョイントがない場合
				joint.transform.translate = translate1;
				joint.transform.rotate = rotate1;
				joint.transform.scale = scale1;
			}
		}
	}


}


