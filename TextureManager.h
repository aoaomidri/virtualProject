#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include<array>
#include"GraphicsPipeline.h"
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

	D3D12_GPU_DESCRIPTOR_HANDLE SendGPUDescriptorHandle(uint32_t index)const { return textureSrvHandleGPU[index]; }

	

	void Load(const std::string& filePath, uint32_t index);

	void PreDraw2D();
	
	void PostDraw2D();

	void PreDraw3D();

	void PostDraw3D();

	void MakeShaderResourceView();

private:
	// DirectX12のTextureResourceを作る
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

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
	//SRVの最大個数
	static const size_t kMaxSRVConst = 257;

	//テクスチャバッファ
	std::array<ComPtr<ID3D12Resource>, kMaxSRVConst> textureBuffers_;

	std::array<ComPtr<ID3D12Resource>, kMaxSRVConst> intermediateBuffers_;


	ID3D12Device* device_ = nullptr;
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	std::unique_ptr<GraphicsPipeline> GraphicsPipeline2D_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipeline3D_;


	ComPtr<ID3D12Resource> textureResource;
	ComPtr<ID3D12Resource> intermediateResource;

	////SRVを作成するDescripterHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU[kMaxSRVConst]{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU[kMaxSRVConst]{};

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