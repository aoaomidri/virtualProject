#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include"Texture.h"
//namespace省略
template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
class TextureManager {
public:
	//TextureManager();
	~TextureManager();

	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, 
		ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap);

	void Finalize();
	

	//	シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	D3D12_GPU_DESCRIPTOR_HANDLE SendGPUDescriptorHandle()const { return textureSrvHandleGPU; }

	void Load(const std::string& filePath);

	void MakeShaderResourceView();

	// DirectX12のTextureResourceを作る
		DirectX::ScratchImage LoadTexture(const std::string & filePath);

	//	TextureResourceにデータを転送する
	[[nodiscard]] ComPtr<ID3D12Resource> UploadTextureData(
		ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages,
		ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList);

	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes);

	ComPtr<ID3D12Resource> CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, 
		uint32_t descriptorSize, uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
		uint32_t descriptorSize, uint32_t index);
private:

	ID3D12Device* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	ComPtr<ID3D12Resource> textureResource;
	ComPtr<ID3D12Resource> intermediateResource;

	////SRVを作成するDescripterHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU{};

	DirectX::ScratchImage mipImages;
	DirectX::TexMetadata metadata;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	//SRV
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;


private:
	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム  コンテナデータ型: Texture*)
	/// </summary>

	
};