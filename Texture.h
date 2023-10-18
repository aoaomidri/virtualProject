#pragma once
#include"externals/DirectXTex/d3dx12.h"
#include"externals/DirectXTex/DirectXTex.h"
#include "math/Matrix.h"

#include"Log.h"
#include<wrl.h>



class Texture{
public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public:
	void Load(const std::string& filePath, ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList);

	void CreateSRV(ComPtr<ID3D12Device> device, D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU);


public:
	

	//	DirectX12のTextureResourceを作る
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//	TextureResourceにデータを転送する
	[[nodiscard]] ComPtr<ID3D12Resource> UploadTextureData(
		ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages,
		ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList);

	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

	static ComPtr<ID3D12Resource> CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);

private:

	ComPtr<ID3D12Resource> textureResource;
	ComPtr<ID3D12Resource> intermediateResource;
	
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	bool isLoad;
};

