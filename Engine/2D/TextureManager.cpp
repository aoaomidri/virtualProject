#include "TextureManager.h"
#include"DirectXCommon.h"
#include"Model.h"
#include <cassert>
#include<fstream>
#include<sstream>

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize() {
	GraphicsPipeline2D_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipeline2D_->Initialize(L"resources/shaders/Object2d.VS.hlsl", L"resources/shaders/Object2d.PS.hlsl", false);
	GraphicsPipeline3D_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipeline3D_->Initialize(L"resources/shaders/Object3d.VS.hlsl", L"resources/shaders/Object3d.PS.hlsl", true);

	GraphicsPipelineSkinning3D_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSkinning3D_->InitializeSkinning(L"resources/shaders/SkinningObject3d.VS.hlsl", L"resources/shaders/Object3d.PS.hlsl", true);
	
	//ヴィネッティングなし
	GraphicsPipelineCopy_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineCopy_->InitializeCopy(L"resources/shaders/FullScreen.VS.hlsl", L"resources/shaders/CopyImage.PS.hlsl");
	GraphicsPipelineGray_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineGray_->InitializeCopy(L"resources/shaders/FullScreen.VS.hlsl", L"resources/shaders/Grayscale.PS.hlsl");
	GraphicsPipelineSepia_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSepia_->InitializeCopy(L"resources/shaders/FullScreen.VS.hlsl", L"resources/shaders/Sepiascale.PS.hlsl");
	//ヴィネッティングあり
	GraphicsPipelineNormalVignetting_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineNormalVignetting_->InitializeCopy(L"resources/shaders/FullScreen.VS.hlsl", L"resources/shaders/NormalVignetting.PS.hlsl");
	GraphicsPipelineGrayVignetting_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineGrayVignetting_->InitializeCopy(L"resources/shaders/FullScreen.VS.hlsl", L"resources/shaders/GrayVignetting.PS.hlsl");
	GraphicsPipelineSepiaVignetting_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSepiaVignetting_->InitializeCopy(L"resources/shaders/FullScreen.VS.hlsl", L"resources/shaders/SepiaVignetting.PS.hlsl");
	
	device_ = DirectXCommon::GetInstance()->GetDevice();
	Model::SetDevice(device_);

}

void TextureManager::Finalize() {
	//device_->Release();
}


//void TextureManager::Load(const std::string& filePath, uint32_t index){
//	mipImages = LoadTexture(filePath);
//	metadata = mipImages.GetMetadata();
//	textureBuffers_[index] = CreateTextureResource(device_, metadata);
//	intermediateBuffers_[index] = UploadTextureData(textureBuffers_[index], mipImages, device_, commandList_);
//
//	srvDesc.Format = metadata.format;
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
//
//	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//
//	textureSrvHandleCPU[index] = GetCPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV, 2 + index);
//	textureSrvHandleGPU[index] = GetGPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV, 2 + index);
//
//	//SRVの生成
//	device_->CreateShaderResourceView(textureBuffers_[index].Get(), &srvDesc, textureSrvHandleCPU[index]);
//}

uint32_t TextureManager::Load(const std::string& filePath){
	int i = 0;
	bool isLoad = false;
	std::string result;

	slashPos_ = filePath.find_last_of('/');
	dotPos_ = filePath.find_last_of('.');
	if (slashPos_ != std::string::npos && dotPos_ != std::string::npos && dotPos_ > slashPos_) {
		result = filePath.substr(slashPos_ + 1, dotPos_ - slashPos_ - 1);
	}

	while (!textureArray_[i].first.empty()){
		if (textureArray_[i].first == result) {
			isLoad = true;
			return i;
		}
		i++;
	}

	if (isLoad) {
		return i;
	}

	textureArray_[i].first = result;
	textureArray_[i].second = i;

	mipImages = LoadTexture(filePath);
	metadata = mipImages.GetMetadata();
	textureBuffers_[i] = CreateTextureResource(metadata);
	intermediateBuffers_[i] = UploadTextureData(textureBuffers_[i], mipImages);

	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//インスタンシングやimguiで利用するための2
	//OffscreenRendering用で3つ使うため合わせて5
	textureSrvHandleCPU[i] = GetCPUDescriptorHandle(descriptorSizeSRV, 5 + i);
	textureSrvHandleGPU[i] = GetGPUDescriptorHandle(descriptorSizeSRV, 5 + i);

	//SRVの生成
	device_->CreateShaderResourceView(textureBuffers_[i].Get(), &srvDesc, textureSrvHandleCPU[i]);

	return i;
}

void TextureManager::MakeInstancingShaderResourceView(ID3D12Resource* resource){
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = 300;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	instancingSrvHandleCPU = GetCPUDescriptorHandle(descriptorSizeSRV, 1);
	instancingSrvHandleGPU = GetGPUDescriptorHandle(descriptorSizeSRV, 1);

	//SRVの生成
	device_->CreateShaderResourceView(resource, &instancingSrvDesc, instancingSrvHandleCPU);
}

void TextureManager::PreDraw2D(){
	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline2D_->GetRootSignature());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipeline2D_->GetPipeLineState());
	//commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PostDraw2D()
{
}

void TextureManager::PreDraw3D(){
	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline3D_->GetRootSignature());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipeline3D_->GetPipeLineState());
	//commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PostDraw3D(){

}

void TextureManager::PreDrawSkin3D(){
	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineSkinning3D_->GetRootSignature());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineSkinning3D_->GetPipeLineState());
	//commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PostDrawSkin3D()
{
}

void TextureManager::PreDrawParticle(){
	
}

void TextureManager::PostDrawParticle(){

}

void TextureManager::PreDrawCopy(){
	MakeShaderResourceView();
}

void TextureManager::DrawCopy(){
	
	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	if (selectPost_ == 0 || selectPost_ >= PostEffect::Over) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineCopy_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineCopy_->GetPipeLineState());

	}
	else if (selectPost_ == PostEffect::Gray) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineGray_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineGray_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::Sepia) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineSepia_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineSepia_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::NormalVignetting) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineNormalVignetting_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineNormalVignetting_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::GrayVignetting) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineGrayVignetting_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineGrayVignetting_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::SepiaVignetting) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineSepiaVignetting_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineSepiaVignetting_->GetPipeLineState());
	}
	else {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineCopy_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineCopy_->GetPipeLineState());
	}

	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(0, renderTextureSrvHandleGPU);
	//commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void TextureManager::MakeShaderResourceView() {

	recderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	recderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	recderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	recderTextureSrvDesc.Texture2D.MipLevels = 1;

	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	renderTextureSrvHandleCPU = GetCPUDescriptorHandle(descriptorSizeSRV, 3);
	renderTextureSrvHandleGPU = GetGPUDescriptorHandle(descriptorSizeSRV, 3);

	//SRVの生成
	device_->CreateShaderResourceView(DirectXCommon::GetInstance()->GetRenderTexture(), &recderTextureSrvDesc, renderTextureSrvHandleCPU);

}

DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath){
	//	テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//	ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));
	//	ミニマップ付きのデータを返す
	return mipImages;
}

[[nodiscard]]Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages){
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device_, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);
	UpdateSubresources(DirectXCommon::GetInstance()->GetCommandList(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	DirectXCommon::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateBufferResource(size_t sizeInBytes){
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

	ComPtr<ID3D12Resource> bufferResource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(hr));

	return bufferResource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata){
	//1, metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//Textureの幅
	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行きor配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元

	//2, 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//細かい設定

	//3, Respurceを生成する
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,//初回のResourceState。Textureは基本読むだけ
		nullptr,//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetCPUDescriptorHandle(uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = DirectXCommon::GetInstance()->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUDescriptorHandle(uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleCPU = DirectXCommon::GetInstance()->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}


