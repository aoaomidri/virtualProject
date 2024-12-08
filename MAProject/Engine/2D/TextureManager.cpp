#include "TextureManager.h"
#include"DirectXCommon.h"
#include"PostEffect.h"
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
	GraphicsPipeline2D_->Initialize2D(L"resources/shaders/object2D/Object2d.VS.hlsl", L"resources/shaders/object2D/Object2d.PS.hlsl");

	GraphicsPipelineWorld2D_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineWorld2D_->InitializeWorld2D(L"resources/shaders/object2D/Object2d.VS.hlsl", L"resources/shaders/object2D/Object2d.PS.hlsl");

	GraphicsPipeline3D_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipeline3D_->Initialize(L"resources/shaders/object3D/Object3d.VS.hlsl", L"resources/shaders/object3D/Object3d.PS.hlsl", true);

	GraphicsPipelineMapping3D_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineMapping3D_->Initialize(L"resources/shaders/object3D/Object3d.VS.hlsl", L"resources/shaders/object3D/MappingObject3d.PS.hlsl", true);


	GraphicsPipelineSkyBox_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSkyBox_->InitializeSkyBox(L"resources/shaders/SkyBox/Skybox.VS.hlsl", L"resources/shaders/SkyBox/Skybox.PS.hlsl");


	GraphicsPipelineSkinning3D_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSkinning3D_->InitializeSkinning(L"resources/shaders/object3D/SkinningObject3d.VS.hlsl", L"resources/shaders/object3D/MappingObject3d.PS.hlsl", true);
	

	device_ = DirectXCommon::GetInstance()->GetDevice();
	Model::SetDevice(device_);

	heap_ = SRVDescriptorHeap::GetInstance();

	MakeRenderTexShaderResourceView();
	MakeDepthShaderResouceView();

}

void TextureManager::Finalize() {
	//device_->Release();
}


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

	mipImages_ = LoadTexture(filePath);
	metadata_ = mipImages_.GetMetadata();
	textureBuffers_[i] = CreateTextureResource(metadata_);
	intermediateBuffers_[i] = UploadTextureData(textureBuffers_[i], mipImages_);

	srvDesc_.Format = metadata_.format;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (metadata_.IsCubemap()){
		srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc_.TextureCube.MostDetailedMip = 0;
		srvDesc_.TextureCube.MipLevels = UINT_MAX;
		srvDesc_.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {
		srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc_.Texture2D.MipLevels = UINT(metadata_.mipLevels);
	}
	

	textureSrvHandleCPU_[i] = heap_->GetCPUDescriptorHandle();
	textureSrvHandleGPU_[i] = heap_->GetGPUDescriptorHandle();

	//SRVの生成
	device_->CreateShaderResourceView(textureBuffers_[i].Get(), &srvDesc_, textureSrvHandleCPU_[i]);

	return i;
}


void TextureManager::PreDraw2D(){
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipeline2D_->GetRootSignature());
	commandList->SetPipelineState(GraphicsPipeline2D_->GetPipeLineState());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PostDraw2D()
{
}

void TextureManager::PreDrawWorld2D(){
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipelineWorld2D_->GetRootSignature());
	commandList->SetPipelineState(GraphicsPipelineWorld2D_->GetPipeLineState());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PreDraw3D(){
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipeline3D_->GetRootSignature());
	commandList->SetPipelineState(GraphicsPipeline3D_->GetPipeLineState());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PreDrawMapping3D(){
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipelineMapping3D_->GetRootSignature());
	commandList->SetPipelineState(GraphicsPipelineMapping3D_->GetPipeLineState());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PostDraw3D(){

}

void TextureManager::PreDrawSkyBox(){
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipelineSkyBox_->GetRootSignature());
	commandList->SetPipelineState(GraphicsPipelineSkyBox_->GetPipeLineState());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PostDrawSkyBox(){

}

void TextureManager::PreDrawSkin3D(){
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();

	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipelineSkinning3D_->GetRootSignature());
	commandList->SetPipelineState(GraphicsPipelineSkinning3D_->GetPipeLineState());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}	

void TextureManager::PostDrawSkin3D()
{
}

void TextureManager::PreDrawParticle(){
	
}

void TextureManager::PostDrawParticle(){

}

void TextureManager::PreDrawCopy(){
	MakeRenderTexShaderResourceView();
	MakeDepthShaderResouceView();
}

void TextureManager::DrawCopy(){

	auto commandList = DirectXCommon::GetInstance()->GetCommandList();
	auto postEffect = PostEffect::GetInstance();

	postEffect->SetPipeLine();	
	
	commandList->SetGraphicsRootDescriptorTable(0, renderTextureSrvHandleGPU_);
	if (postEffect->IsSelectOutLine()){

		commandList->SetGraphicsRootDescriptorTable(1, depthStencilSrvHandleGPU_);
		commandList->SetGraphicsRootConstantBufferView(3, postEffect->GetCameraMat());		
	}
	if (postEffect->IsSelectVignetting()) {
		commandList->SetGraphicsRootConstantBufferView(2, postEffect->GetVignetting());
	}

	if (postEffect->IsSelectDissolve()) {
		commandList->SetGraphicsRootDescriptorTable(1, textureSrvHandleGPU_[dissolveTexNumber_]);
		commandList->SetGraphicsRootConstantBufferView(3, postEffect->GetThreshold());
	}
	commandList->SetGraphicsRootConstantBufferView(4, postEffect->GetHSVMaterial());
	if (postEffect->IsSelectGray()){
		commandList->SetGraphicsRootConstantBufferView(5, postEffect->GetBlendPost());
	}


	commandList->DrawInstanced(3, 1, 0, 0);
}

void TextureManager::MakeRenderTexShaderResourceView() {

	recderTextureSrvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	recderTextureSrvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	recderTextureSrvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	recderTextureSrvDesc_.Texture2D.MipLevels = 1;

	renderTextureSrvHandleCPU_ = heap_->GetCPUDescriptorHandle();
	renderTextureSrvHandleGPU_ = heap_->GetGPUDescriptorHandle();

	//SRVの生成
	device_->CreateShaderResourceView(DirectXCommon::GetInstance()->GetRenderTexture(), &recderTextureSrvDesc_, renderTextureSrvHandleCPU_);

}

void TextureManager::MakeDepthShaderResouceView(){
	//DXGI_FORMAT_D24_UNORM_S8_UINTのDepthを読むときはDXGI_FORMAT_R24_UNORM_X8_TYPELESS
	depthTextureSrvDesc_.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthTextureSrvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	depthTextureSrvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	depthTextureSrvDesc_.Texture2D.MipLevels = 1;

	depthStencilSrvHandleCPU_ = heap_->GetCPUDescriptorHandle();
	depthStencilSrvHandleGPU_ = heap_->GetGPUDescriptorHandle();

	device_->CreateShaderResourceView(DirectXCommon::GetInstance()->GetDepthStencil(), &depthTextureSrvDesc_, depthStencilSrvHandleCPU_);
}

DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath){
	//	テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	
	if (filePathW.ends_with(L".dds")) {
		hr_ = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr_ = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr_));

	//	ミニマップの作成
	DirectX::ScratchImage mipImages{};
	if (DirectX::IsCompressed(image.GetMetadata().format)) {//圧縮フォーマットかどうか調べる
		mipImages = std::move(image);
	}
	else {
		hr_ = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		mipImages = std::move(image);

		assert(SUCCEEDED(hr_));
	}
	
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
	hr_ = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(hr_));

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
	hr_ = device_->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,//初回のResourceState。Textureは基本読むだけ
		nullptr,//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr_));

	return resource;
}





