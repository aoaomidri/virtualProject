#include"Object3D.h"
#include"DirectXCommon.h"
#include <cassert>
#include<fstream>
#include<sstream>


void Object3D::Initialize(const std::string fileName) {
	makeResource();

	isDraw_ = true;

	materialDate->enableLighting = false;

	model_ = ModelManager::GetInstance()->LoadModelFile(fileName);

	

	animation_ = Model::LoadAnimationFile(fileName);

	if (model_->GetMaterial().textureFilePath != "") {
		texHandle_ = (model_->GetMaterial().textureHandle);
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

void Object3D::Update(const ViewProjection& viewProjection) {
	if (!isDraw_) {
		return;
	}

	//rotate_.y += 0.01f;

	if (animation_.duration != 0) {
		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, animation_.duration);
		Model::NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[model_->GetNodeName()];
		animeTranslate_ = CalculateValue(rootNodeAnimation.translate, animationTime);
		animeRotate_ = CalculateValue(rootNodeAnimation.rotate, animationTime);
		animeScale_ = CalculateValue(rootNodeAnimation.scale, animationTime);
		localMatrix_ = Matrix::MakeAffineMatrix(animeScale_, animeRotate_, animeTranslate_);
	}
	else {
		localMatrix_ = model_->GetLocalMatrix();
	}
	if (setMatrix_.m[3][3] != 0) {
		worldMatrix_ = setMatrix_;
	}
	else {

		worldMatrix_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	}
	
	if (parent_){
		worldMatrix_.Multiply(*parent_);
	}

	
	Matrix4x4 worldViewProjectionMatrix = Matrix::Multiply(worldMatrix_, viewProjection.matViewProjection_);
	if (isGetTop_) {

		Vector3 result{};
		Vector3 resultTail{};
		auto vertexes = model_->GetVertexData();

		for (size_t i = 0; i < vertexes.size(); i++) {
			auto position = vertexes[i].position;
			if (result.z > position.z) {
				result = { position.x,position.y,position.z };
			}

		}
		result = { result.x,(-result.z) * trailData_.x,-result.y };
		resultTail = { result.x,result.y * trailData_.y,result.z };
		Matrix4x4 transMat;

		Matrix4x4 transMatTail;
		transMat = Matrix::MakeTranslateMatrix(result);
		transMatTail = Matrix::MakeTranslateMatrix(resultTail);

		matTop_ = Matrix::Multiply(transMat, worldMatrix_);
		matTail_ = Matrix::Multiply(transMatTail, worldMatrix_);
		vectorTop_.head = { matTop_.m[3][0],matTop_.m[3][1], matTop_.m[3][2] };
		vectorTop_.tail = { matTail_.m[3][0],matTail_.m[3][1], matTail_.m[3][2] };
	}
	wvpData->WVP = Matrix::Multiply(localMatrix_, worldViewProjectionMatrix);
	materialDate->enableLighting = isUseLight_;

	wvpData->World = Matrix::Multiply(localMatrix_, worldMatrix_);
	wvpData->WorldInverseTranspose = Matrix::Inverce(Matrix::Transpose(worldMatrix_));
	
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

void Object3D::UniqueUpdate(){
	if (!isDraw_) {
		return;
	}

	//rotate_.y += 0.01f;

	if (animation_.duration != 0) {
		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, animation_.duration);
		Model::NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[model_->GetNodeName()];
		animeTranslate_ = CalculateValue(rootNodeAnimation.translate, animationTime);
		animeRotate_ = CalculateValue(rootNodeAnimation.rotate, animationTime);
		animeScale_ = CalculateValue(rootNodeAnimation.scale, animationTime);
		localMatrix_ = Matrix::MakeAffineMatrix(animeScale_, animeRotate_, animeTranslate_);
	}
	else {
		localMatrix_ = model_->GetLocalMatrix();
	}
	if (setMatrix_.m[3][3] != 0) {
		worldMatrix_ = setMatrix_;
	}
	else {

		worldMatrix_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	}

	if (parent_) {
		worldMatrix_.Multiply(*parent_);
	}


	if (isGetTop_) {

		Vector3 result{};
		Vector3 resultTail{};
		auto vertexes = model_->GetVertexData();

		for (size_t i = 0; i < vertexes.size(); i++) {
			auto position = vertexes[i].position;
			if (result.z > position.z) {
				result = { position.x,position.y,position.z };
			}

		}
		result = { result.x,(-result.z) * trailData_.x,-result.y };
		resultTail = { result.x,result.y * trailData_.y,result.z };
		Matrix4x4 transMat;

		Matrix4x4 transMatTail;
		transMat = Matrix::MakeTranslateMatrix(result);
		transMatTail = Matrix::MakeTranslateMatrix(resultTail);

		matTop_ = Matrix::Multiply(transMat, worldMatrix_);
		matTail_ = Matrix::Multiply(transMatTail, worldMatrix_);
		vectorTop_.head = { matTop_.m[3][0],matTop_.m[3][1], matTop_.m[3][2] };
		vectorTop_.tail = { matTail_.m[3][0],matTail_.m[3][1], matTail_.m[3][2] };
	}

}

void Object3D::Draw() {

	if (!isDraw_) {
		return;
	}
	const auto texManagerIns = TextureManager::GetInstance();

	model_->Draw(DirectXCommon::GetInstance()->GetCommandList());
	//形状を設定。
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, texManagerIns->SendGPUDescriptorHandle(texHandle_));
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(3, texManagerIns->SendGPUDescriptorHandle(texManagerIns->GetSkyBoxTex()));
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, directionalLightResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(7, texManagerIns->SendGPUDescriptorHandle(texManagerIns->GetDissolveTex()));
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(8, dissolveResource_->GetGPUVirtualAddress());
	//3D三角の描画
	DirectXCommon::GetInstance()->GetCommandList()->DrawIndexedInstanced(static_cast<uint32_t>(model_->GetIndexData().size()), 1, 0, 0, 0);


}

void Object3D::DrawImgui(std::string name){
#ifdef _DEBUG
	ImGui::Begin((name + "オブジェの内部設定").c_str());
	ImGui::Checkbox("描画するかどうか", &isDraw_);
	ImGui::Checkbox("ライティングするかどうか", &isUseLight_);
	ImGui::DragFloat3("モデルに設定されたScale", &animeScale_.x, 0.1f);
	ImGui::DragFloat4("モデルに設定されたRotate", &animeRotate_.quaternion_.x, 0.1f);
	ImGui::DragFloat3("モデルに設定されたTransform", &animeTranslate_.x, 0.1f);
	ImGui::DragFloat3("モデルに設定されたTrScale", &transform_.scale.x, 0.1f);
	ImGui::DragFloat3("モデルに設定されたTrRotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("モデルに設定されたTrTransform", &transform_.translate.x, 0.1f);
	ImGui::DragFloat("しきい値", &dissolveData_->threshold, 0.001f, 0.0f, 1.0f);
	ImGui::End();
#endif
}

void Object3D::SetDirectionalLight(const DirectionalLight::DirectionalLightData* light){
	directionalLight = light;
}

void Object3D::SetPointLight(const PointLight::PointLightData* pLight){
	pointLight = pLight;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Object3D::CreateBufferResource(size_t sizeInBytes) {

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

void Object3D::makeResource() {
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
	wvpData->WVP = Matrix::MakeIdentity4x4();
	wvpData->World = Matrix::MakeIdentity4x4();
	wvpData->WorldInverseTranspose = Matrix::MakeIdentity4x4();

	/*平行光源用リソース関連*/
	//マテリアル用のリソース
	directionalLightResource = CreateBufferResource(sizeof(DirectionalLight::DirectionalLightData));

	//書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	//今回は白を書き込んでみる
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };

	directionalLightDate->intensity = 1.0f;

	//マテリアル用のリソース
	pointLightResource = CreateBufferResource(sizeof(PointLight::PointLightData));

	//書き込むためのアドレスを取得
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));
	//今回は白を書き込んでみる
	pointLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	pointLightData->position = { 0.0f,10.0f,0.0f };

	pointLightData->intensity = 1.0f;

	pointLightData->radius = 1.0f;

	pointLightData->decay = 1.0f;

	/*カメラリソース関連*/
	cameraResource_ = CreateBufferResource(sizeof(CameraForGPU));

	//書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPU_));

	cameraForGPU_->worldPosition = { 0.0f,0.0f,0.0f };

	dissolveResource_ = TextureManager::GetInstance()->CreateBufferResource(sizeof(PostEffect::Threshold));

	dissolveResource_->Map(0, nullptr, reinterpret_cast<void**>(&dissolveData_));

	dissolveData_->threshold = 0.0f;


}

Vector3 Object3D::CalculateValue(const std::vector<Model::KeyframeVector3>& keyframes, float time){
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

Quaternion Object3D::CalculateValue(const std::vector<Model::KeyframeQuaternion>& keyframes, float time) {
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


