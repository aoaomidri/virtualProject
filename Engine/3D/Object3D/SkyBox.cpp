#include "SkyBox.h"
#include"DirectXCommon.h"
#include <cassert>
#include<fstream>
#include<sstream>


void SkyBox::Initialize(const std::string fileName) {
	makeResource();

	isDraw_ = true;

	materialDate->enableLighting = false;
	
	texHandle_ = TextureManager::GetInstance()->Load(fileName);	

	transform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};


}

void SkyBox::Update(const ViewProjection& viewProjection) {
	if (!isDraw_) {
		return;
	}

	//rotate_.y += 0.01f;

	worldMatrix_.MakeAffineMatrix(transform_);
	if (parent_) {
		worldMatrix_ = worldMatrix_ * (*parent_);
	}


	Matrix4x4 worldViewProjectionMatrix = Matrix::GetInstance()->Multiply(worldMatrix_, viewProjection.matViewProjection_);

	wvpData->WVP = worldViewProjectionMatrix;
	materialDate->enableLighting = isUseLight_;

	wvpData->World = worldMatrix_;
	wvpData->WorldInverseTranspose = Matrix::GetInstance()->Inverce(Matrix::GetInstance()->Transpose(worldMatrix_));


	materialDate->shininess = shininess_;

}

void SkyBox::Draw() {

	if (!isDraw_) {
		return;
	}
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	DirectXCommon::GetInstance()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//形状を設定。
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, wvpResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, materialResource->GetGPUVirtualAddress());
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->SendGPUDescriptorHandle(texHandle_));
	//3D三角の描画
	DirectXCommon::GetInstance()->GetCommandList()->DrawIndexedInstanced(kIndexNum_, 1, 0, 0, 0);


}

void SkyBox::DrawImgui(std::string name) {
#ifdef _DEBUG
	ImGui::Begin((name + "オブジェの内部設定").c_str());
	ImGui::Checkbox("描画するかどうか", &isDraw_);
	ImGui::End();
#endif
}


Microsoft::WRL::ComPtr<ID3D12Resource> SkyBox::CreateBufferResource(size_t sizeInBytes) {

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

void SkyBox::MakeVertexResouce(){
	//頂点リソースの作成
	vertexResource_ = CreateBufferResource(sizeof(SkyBoxVertexData) * kVertexNum_);

	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点三つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(SkyBoxVertexData) * kVertexNum_);
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(SkyBoxVertexData);

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate_));

	//頂点情報を書き込む

	//右面、描画インデックスは[0,1,2][2,1,3]で内側を向く
	vertexDate_[0].position =  { 1.0f, 1.0f, 1.0f, 1.0f };
	vertexDate_[1].position =  { 1.0f, 1.0f,-1.0f, 1.0f };
	vertexDate_[2].position =  { 1.0f,-1.0f, 1.0f, 1.0f };
	vertexDate_[3].position =  { 1.0f,-1.0f,-1.0f, 1.0f };

	//左面、描画インデックスは[4,5,6][6,5,7]で内側を向く
	vertexDate_[4].position =  {-1.0f, 1.0f,-1.0f, 1.0f };
	vertexDate_[5].position =  {-1.0f, 1.0f, 1.0f, 1.0f };
	vertexDate_[6].position =  {-1.0f,-1.0f,-1.0f, 1.0f };
	vertexDate_[7].position =  {-1.0f,-1.0f, 1.0f, 1.0f };

	//インデックスで描画するのでここまで

	////前面、描画インデックスは[8,9,10][10,9,11]で内側を向く
	//vertexDate_[8].position =  {-1.0f, 1.0f, 1.0f, 1.0f };
	//vertexDate_[9].position =  { 1.0f, 1.0f, 1.0f, 1.0f };
	//vertexDate_[10].position = {-1.0f,-1.0f, 1.0f, 1.0f };
	//vertexDate_[11].position = { 1.0f,-1.0f, 1.0f, 1.0f };

	////後面、描画インデックスは[12,13,14][14,13,15]で内側を向く
	//vertexDate_[12].position = { 1.0f, 1.0f,-1.0f, 1.0f };
	//vertexDate_[13].position = {-1.0f, 1.0f,-1.0f, 1.0f };
	//vertexDate_[14].position = { 1.0f,-1.0f,-1.0f, 1.0f };
	//vertexDate_[15].position = {-1.0f,-1.0f,-1.0f, 1.0f };

	////上面、描画インデックスは[16,17,18][18,17,19]で内側を向く
	//vertexDate_[16].position = {-1.0f, 1.0f,-1.0f, 1.0f };
	//vertexDate_[17].position = { 1.0f, 1.0f,-1.0f, 1.0f };
	//vertexDate_[18].position = {-1.0f, 1.0f, 1.0f, 1.0f };
	//vertexDate_[19].position = { 1.0f, 1.0f, 1.0f, 1.0f };

	////下面、描画インデックスは[20,21,22][22,21,23]で内側を向く
	//vertexDate_[20].position = {-1.0f,-1.0f, 1.0f, 1.0f };
	//vertexDate_[21].position = { 1.0f,-1.0f, 1.0f, 1.0f };
	//vertexDate_[22].position = {-1.0f,-1.0f,-1.0f, 1.0f };
	//vertexDate_[23].position = { 1.0f,-1.0f,-1.0f, 1.0f };

	/*インデックスの処理*/

	indexResource_ = CreateBufferResource(sizeof(uint32_t) * kIndexNum_);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();

	indexBufferView_.SizeInBytes = sizeof(uint32_t) * kIndexNum_;

	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//右面
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 2; indexData_[4] = 1; indexData_[5] = 3;

	//左面
	indexData_[6] = 4; indexData_[7] = 5; indexData_[8] = 6;
	indexData_[9] = 6; indexData_[10] = 5; indexData_[11] = 7;

	//前面
	indexData_[12] = 5; indexData_[13] = 0; indexData_[14] = 7;
	indexData_[15] = 7; indexData_[16] = 0; indexData_[17] = 2;

	//後面
	indexData_[18] = 1; indexData_[19] = 4; indexData_[20] = 3;
	indexData_[21] = 3; indexData_[22] = 4; indexData_[23] = 6;

	//上面
	indexData_[24] = 4; indexData_[25] = 1; indexData_[26] = 5;
	indexData_[27] = 5; indexData_[28] = 1; indexData_[29] = 0;

	//下面
	indexData_[30] = 7; indexData_[31] = 2; indexData_[32] = 6;
	indexData_[33] = 6; indexData_[34] = 2; indexData_[35] = 3;

}

void SkyBox::makeResource() {
	MakeVertexResouce();

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



}
