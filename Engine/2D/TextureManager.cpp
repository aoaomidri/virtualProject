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
	GraphicsPipeline2D_->Initialize2D(L"resources/shaders/object2D/Object2d.VS.hlsl", L"resources/shaders/object2D/Object2d.PS.hlsl");
	GraphicsPipeline3D_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipeline3D_->Initialize(L"resources/shaders/object3D/Object3d.VS.hlsl", L"resources/shaders/object3D/Object3d.PS.hlsl", true);

	GraphicsPipelineSkyBox_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSkyBox_->InitializeSkyBox(L"resources/shaders/SkyBox/Skybox.VS.hlsl", L"resources/shaders/SkyBox/Skybox.PS.hlsl");


	GraphicsPipelineSkinning3D_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSkinning3D_->InitializeSkinning(L"resources/shaders/object3D/SkinningObject3d.VS.hlsl", L"resources/shaders/object3D/Object3d.PS.hlsl", true);
	
	//ヴィネッティングなし
	GraphicsPipelineCopy_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineCopy_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/CopyImage.PS.hlsl");
	GraphicsPipelineGray_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineGray_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/Grayscale.PS.hlsl");
	GraphicsPipelineSepia_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSepia_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/Sepiascale.PS.hlsl");
	GraphicsPipelineInverse_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineInverse_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/InvertedColor.PS.hlsl");
	//ヴィネッティングあり
	GraphicsPipelineNormalVignetting_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineNormalVignetting_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/NormalVignetting.PS.hlsl");
	GraphicsPipelineGrayVignetting_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineGrayVignetting_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/GrayVignetting.PS.hlsl");
	GraphicsPipelineSepiaVignetting_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSepiaVignetting_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/SepiaVignetting.PS.hlsl");
	
	//順序反転
	GraphicsPipelineVignettingGrayScale_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineVignettingGrayScale_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/VignettingGrayScale.PS.hlsl");
	GraphicsPipelineVignettingSepiaScale_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineVignettingSepiaScale_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/VignettingSepiaScale.PS.hlsl");

	//smoothing
	GraphicsPipelineSmoothing3x3 = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSmoothing3x3->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/GaussianFilter3x3.PS.hlsl");
	GraphicsPipelineSmoothing5x5 = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSmoothing5x5->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/GaussianFilter5x5.PS.hlsl");
	GraphicsPipelineSmoothing9x9 = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineSmoothing9x9->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/GaussianFilter9x9.PS.hlsl");

	//outline
	GraphicsPipelineOutLine_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineOutLine_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/DepthBasedOutline.PS.hlsl");

	//ラジアンブラー
	GraphicsPipelineRadialBlur_ = std::make_unique<GraphicsPipeline>();
	GraphicsPipelineRadialBlur_->InitializeCopy(L"resources/shaders/PostEffect/FullScreen.VS.hlsl", L"resources/shaders/PostEffect/RadialBlur.PS.hlsl");


	device_ = DirectXCommon::GetInstance()->GetDevice();
	Model::SetDevice(device_);

	CreateVignettingResource();
	ProjectInverseResource();
	
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

	mipImages = LoadTexture(filePath);
	metadata = mipImages.GetMetadata();
	textureBuffers_[i] = CreateTextureResource(metadata);
	intermediateBuffers_[i] = UploadTextureData(textureBuffers_[i], mipImages);

	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (metadata.IsCubemap()){
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	}
	

	

	//インスタンシングやimguiで利用するための2
	// depthで使う3
	//OffscreenRendering用で3つ使うため合わせて6個使われている
	textureSrvHandleCPU[i] = SRVDescriptorHeap::GetInstance()->GetCPUDescriptorHandle();
	textureSrvHandleGPU[i] = SRVDescriptorHeap::GetInstance()->GetGPUDescriptorHandle();

	//SRVの生成
	device_->CreateShaderResourceView(textureBuffers_[i].Get(), &srvDesc, textureSrvHandleCPU[i]);

	return i;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::MakeInstancingShaderResourceView(ID3D12Resource* resource){
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = 600;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	instancingSrvHandleCPU = SRVDescriptorHeap::GetInstance()->GetCPUDescriptorHandle();
	instancingSrvHandleGPU = SRVDescriptorHeap::GetInstance()->GetGPUDescriptorHandle();

	//SRVの生成
	device_->CreateShaderResourceView(resource, &instancingSrvDesc, instancingSrvHandleCPU);

	return instancingSrvHandleGPU;
}

void TextureManager::PreDraw2D(){
	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline2D_->GetRootSignature());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipeline2D_->GetPipeLineState());

	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PostDraw2D()
{
}

void TextureManager::PreDraw3D(){
	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline3D_->GetRootSignature());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipeline3D_->GetPipeLineState());

	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PostDraw3D(){

}

void TextureManager::PreDrawSkyBox(){
	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineSkyBox_->GetRootSignature());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineSkyBox_->GetPipeLineState());

	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TextureManager::PostDrawSkyBox(){

}

void TextureManager::PreDrawSkin3D(){
	//RootSignatureを設定。PSOに設定しているが別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineSkinning3D_->GetRootSignature());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineSkinning3D_->GetPipeLineState());

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
	MakeRenderTexShaderResourceView();
	MakeDepthShaderResouceView();
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
	else if (selectPost_ == PostEffect::Inverse) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineInverse_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineInverse_->GetPipeLineState());
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
	else if (selectPost_ == PostEffect::VignettingGrayScale) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineVignettingGrayScale_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineVignettingGrayScale_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::VignettingSepiaScale) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineVignettingSepiaScale_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineVignettingSepiaScale_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::Smoothing3x3) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineSmoothing3x3->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineSmoothing3x3->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::Smoothing5x5) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineSmoothing5x5->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineSmoothing5x5->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::Smoothing9x9) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineSmoothing9x9->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineSmoothing9x9->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::OutLine) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineOutLine_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineOutLine_->GetPipeLineState());
	}
	else if (selectPost_ == PostEffect::RadialBlur) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineRadialBlur_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineRadialBlur_->GetPipeLineState());
	}
	else {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(GraphicsPipelineCopy_->GetRootSignature());
		DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(GraphicsPipelineCopy_->GetPipeLineState());
	}

	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(0, renderTextureSrvHandleGPU);
	if (selectPost_ == PostEffect::OutLine){

		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(1, depthStencilSrvHandleGPU);
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraResource_->GetGPUVirtualAddress());

		
	}
	if (selectPost_ == PostEffect::NormalVignetting || selectPost_ == PostEffect::GrayVignetting || selectPost_ == PostEffect::SepiaVignetting) {
		DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(2, vignettingResource_->GetGPUVirtualAddress());
	}

	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void TextureManager::MakeRenderTexShaderResourceView() {

	recderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	recderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	recderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	recderTextureSrvDesc.Texture2D.MipLevels = 1;

	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	renderTextureSrvHandleCPU = SRVDescriptorHeap::GetInstance()->GetCPUDescriptorHandle();
	renderTextureSrvHandleGPU = SRVDescriptorHeap::GetInstance()->GetGPUDescriptorHandle();

	//SRVの生成
	device_->CreateShaderResourceView(DirectXCommon::GetInstance()->GetRenderTexture(), &recderTextureSrvDesc, renderTextureSrvHandleCPU);

}

void TextureManager::MakeDepthShaderResouceView(){
	//DXGI_FORMAT_D24_UNORM_S8_UINTのDepthを読むときはDXGI_FORMAT_R24_UNORM_X8_TYPELESS
	depthTextureSrvDesc_.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthTextureSrvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	depthTextureSrvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	depthTextureSrvDesc_.Texture2D.MipLevels = 1;

	const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	depthStencilSrvHandleCPU = SRVDescriptorHeap::GetInstance()->GetCPUDescriptorHandle();
	depthStencilSrvHandleGPU = SRVDescriptorHeap::GetInstance()->GetGPUDescriptorHandle();

	device_->CreateShaderResourceView(DirectXCommon::GetInstance()->GetDepthStencil(), &depthTextureSrvDesc_, depthStencilSrvHandleCPU);
}

DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath){
	//	テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr;
	if (filePathW.ends_with(L".dds")) {
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	//	ミニマップの作成
	DirectX::ScratchImage mipImages{};
	if (DirectX::IsCompressed(image.GetMetadata().format)) {//圧縮フォーマットかどうか調べる
		mipImages = std::move(image);
	}
	else {
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(hr));
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

void TextureManager::CreateVignettingResource(){
	/*//マテリアル用のリソース
	 materialResource_ = CreateBufferResource(sizeof(Model::Material));

	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialDate_));
	//今回は赤を書き込んでみる
	materialDate_->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialDate_->enableLighting = false;

	materialDate_->uvTransform.Identity();

	materialDate_->shininess = 1.0f;*/

	vignettingResource_ = CreateBufferResource(sizeof(Vignetting));

	vignettingResource_->Map(0, nullptr, reinterpret_cast<void**>(&vignettingData_));

	vignettingData_->scale = 16.0f;

	vignettingData_->pow = 0.8f;

}

void TextureManager::ProjectInverseResource(){
	cameraResource_ = CreateBufferResource(sizeof(CameraMat));

	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	cameraData_->matProjectionInverse_.Identity();
}




