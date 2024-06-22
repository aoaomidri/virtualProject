#include "Texture.h"
#include <cassert>

//void Texture::Load(const std::string& filePath, ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList){
//	if (!isLoad) {
//		//テクスチャを読み込んでリソースを作る
//		DirectX::ScratchImage mipImages = LoadTexture(filePath);
//		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
//		textureResource = CreateTextureResource(device,metadata);
//
//		if (textureResource) {
//			intermediateResource = UploadTextureData(textureResource.Get(), mipImages, device, commandList);
//		}
//		else {
//			return;
//		}
//
//		srvDesc.Format = metadata.format;
//		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
//
//		// load済み
//		isLoad = true;
//	}
//
//}

//void Texture::CreateSRV(ComPtr<ID3D12Device> device, D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU){
//	device->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
//}


//DirectX::ScratchImage Texture::LoadTexture(const std::string& filePath){
//	//	テクスチャファイルを読んでプログラムで扱えるようにする
//	DirectX::ScratchImage image{};
//	std::wstring filePathW = ConvertString(filePath);
//	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
//	assert(SUCCEEDED(hr));
//
//	//	ミニマップの作成
//	DirectX::ScratchImage mipImages{};
//	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
//	assert(SUCCEEDED(hr));
//	//	ミニマップ付きのデータを返す
//	return mipImages;
//}
//
////Microsoft::WRL::ComPtr<ID3D12Resource> Texture::UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList){
////	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
////	DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
////	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
////	ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
////	UpdateSubresources(commandList.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
////	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
////	D3D12_RESOURCE_BARRIER barrier{};
////	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
////	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
////	barrier.Transition.pResource = texture.Get();
////	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
////	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
////	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
////	commandList->ResourceBarrier(1, &barrier);
////	return intermediateResource;
////}
//
//Microsoft::WRL::ComPtr<ID3D12Resource> Texture::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes){
//	//頂点リソース用のヒープの設定
//	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
//	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
//	//頂点リソースの設定
//	D3D12_RESOURCE_DESC vertexResourceDesc{};
//	//バッファリソース。テクスチャの場合はまた別の設定をする
//	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	vertexResourceDesc.Width = sizeInBytes;
//
//	vertexResourceDesc.Height = 1;
//	vertexResourceDesc.DepthOrArraySize = 1;
//	vertexResourceDesc.MipLevels = 1;
//	vertexResourceDesc.SampleDesc.Count = 1;
//
//	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//	ComPtr<ID3D12Resource> bufferResource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
//		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
//	assert(SUCCEEDED(hr));
//
//	return bufferResource;
//}
//
//Microsoft::WRL::ComPtr<ID3D12Resource> Texture::CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata){
//	//1, metadataを基にResourceの設定
//	D3D12_RESOURCE_DESC resourceDesc{};
//	resourceDesc.Width = UINT(metadata.width);//Textureの幅
//	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
//	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
//	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行きor配列Textureの配列数
//	resourceDesc.Format = metadata.format;//TextureのFormat
//	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
//	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元
//
//	//2, 利用するHeapの設定
//	D3D12_HEAP_PROPERTIES heapProperties{};
//	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//細かい設定
//
//	//3, Respurceを生成する
//	ComPtr<ID3D12Resource> resource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(
//		&heapProperties,//Heapの設定
//		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。
//		&resourceDesc,//Resourceの設定
//		D3D12_RESOURCE_STATE_COPY_DEST,//初回のResourceState。Textureは基本読むだけ
//		nullptr,//Clear最適値。使わないのでnullptr
//		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
//	assert(SUCCEEDED(hr));
//
//	return resource;
//}
