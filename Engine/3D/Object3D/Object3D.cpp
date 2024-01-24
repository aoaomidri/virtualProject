#include"Object3D.h"
#include"DirectXCommon.h"
#include <cassert>
#include<fstream>
#include<sstream>


void Object3D::Initialize() {
	makeResource();

	isDraw_ = true;

	materialDate->enableLighting = false;

	transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	
}

void Object3D::Update(const Matrix4x4& worldMatrix, const ViewProjection& viewProjection) {
	if (!isDraw_) {
		return;
	}

	//rotate_.y += 0.01f;
	worldMatrix_ = worldMatrix;
	if (parent_){
		worldMatrix_ = Matrix::GetInstance()->Multiply(worldMatrix, *parent_);
	}
	position_ = { worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2] };
	chackMatrix_ = { worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2], worldMatrix_.m[3][3] };

	Matrix4x4 worldViewProjectionMatrix = Matrix::GetInstance()->Multiply(worldMatrix_, viewProjection.matViewProjection_);
	wvpData->WVP = worldViewProjectionMatrix;
	materialDate->enableLighting = isUseLight_;
	wvpData->World = worldMatrix_;
	wvpData->WorldInverseTranspose = Matrix::GetInstance()->Inverce(Matrix::GetInstance()->Trnaspose(worldMatrix_));

	directionalLightDate->direction = Vector3::Normalize(directionalLightDate->direction);

	cameraForGPU_->worldPosition = viewProjection.translation_;
}

void Object3D::Draw(D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle) {

	if (!isDraw_) {
		return;
	}
	model_->Draw(DirectXCommon::GetInstance()->GetCommandList());
	//形状を設定。
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, GPUHandle);
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	//3D三角の描画
	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(UINT(model_->GetVertexData().size()), 1, 0, 0);


}

void Object3D::DrawImgui(){
	ImGui::Begin("オブジェの内部設定");
	ImGui::Checkbox("描画するかどうか", &isDraw_);
	ImGui::Checkbox("ライトを適応するか", &isUseLight_);
	ImGui::Text("ライト関連");
	ImGui::DragFloat4("ライトの色", &directionalLightDate->color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("ライトの向き", &directionalLightDate->direction.x, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat("ライトの輝き", &directionalLightDate->intensity, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("反射強度", &materialDate->shininess, 0.01f, 0.0f, 30.0f);
	ImGui::End();

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
	 materialResource = CreateBufferResource(sizeof(Material));

	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	//今回は赤を書き込んでみる
	materialDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialDate->enableLighting = false;

	materialDate->uvTransform = Matrix::GetInstance()->MakeIdentity4x4();

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
	directionalLightResource = CreateBufferResource(sizeof(DirectionalLight));
	

	//書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	//今回は白を書き込んでみる
	directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	directionalLightDate->direction = { 0.0f,-1.0f,0.0f };

	directionalLightDate->intensity = 1.0f;

	/*カメラリソース関連*/
	cameraResource_ = CreateBufferResource(sizeof(CameraForGPU));

	//書き込むためのアドレスを取得
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPU_));

	cameraForGPU_->worldPosition = { 0.0f,0.0f,0.0f };


}

