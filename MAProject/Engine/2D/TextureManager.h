#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include<array>
#include"GraphicsPipeline.h"
#include"../../externals/DirectXTex/DirectXTex.h"
#include"../../externals/DirectXTex/d3dx12.h"
#include"Matrix.h"
#include"Log.h"
#include<wrl.h>
//namespace省略
template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

class TextureManager {
public:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager& textureManager) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
	//TextureManager();

	void Initialize();

	void Finalize();
	

	//	シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	D3D12_GPU_DESCRIPTOR_HANDLE SendGPUDescriptorHandle(uint32_t index)const { return textureSrvHandleGPU[index]; }

	D3D12_GPU_DESCRIPTOR_HANDLE SendRenderGPUDescriptorHandle()const { return renderTextureSrvHandleGPU; }

	ID3D12Resource* GetTextureBuffer(uint32_t index)const { return textureBuffers_[index].Get(); }

	

	//void Load(const std::string& filePath, uint32_t index);

	uint32_t Load(const std::string& filePath);

	D3D12_GPU_DESCRIPTOR_HANDLE MakeInstancingShaderResourceView(ID3D12Resource* resource);

	void PreDraw2D();
	
	void PostDraw2D();

	void PreDrawWorld2D();

	void PreDraw3D();

	void PreDrawMapping3D();

	void PostDraw3D();

	void PreDrawSkyBox();

	void PostDrawSkyBox();

	void PreDrawSkin3D();

	void PostDrawSkin3D();

	void PreDrawParticle();

	void PostDrawParticle();

	void PreDrawCopy();

	void DrawCopy();

	

public:

	void MakeRenderTexShaderResourceView();

	void MakeDepthShaderResouceView();

	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

public:

	const uint32_t GetSkyBoxTex()const {
		return skyBoxTexNumber_;
	}

	const uint32_t GetDissolveTex()const {
		return dissolveTexNumber_;
	}

	void  SetSkyBoxTex(const uint32_t texNum) {
		skyBoxTexNumber_ = texNum;
	}

	void  SetDissolveTex(const uint32_t texNum) {
		dissolveTexNumber_ = texNum;
	}

private:
	// DirectX12のTextureResourceを作る
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//	TextureResourceにデータを転送する
	[[nodiscard]] ComPtr<ID3D12Resource> UploadTextureData(
		ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages
		);

	

	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);




private:
	//SRVの最大個数
	static const size_t kMaxSRVConst = 257;

	//テクスチャバッファ
	std::array<ComPtr<ID3D12Resource>, kMaxSRVConst> textureBuffers_;

	std::array<ComPtr<ID3D12Resource>, kMaxSRVConst> intermediateBuffers_;


	std::unique_ptr<GraphicsPipeline> GraphicsPipeline2D_;
	
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineWorld2D_;

	std::unique_ptr<GraphicsPipeline> GraphicsPipeline3D_;

	std::unique_ptr<GraphicsPipeline> GraphicsPipelineMapping3D_;

	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSkyBox_;

	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSkinning3D_;



	////SRVを作成するDescripterHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU[kMaxSRVConst]{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU[kMaxSRVConst]{};
	//インスタンシング用のSRV作成用
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU{};

	//RenderTexture用のSRV作成用
	D3D12_CPU_DESCRIPTOR_HANDLE renderTextureSrvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE renderTextureSrvHandleGPU{};

	//Outline用のSRV作成用
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilSrvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE depthStencilSrvHandleGPU{};

	

	DirectX::ScratchImage mipImages;
	DirectX::TexMetadata metadata;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc;

	D3D12_SHADER_RESOURCE_VIEW_DESC recderTextureSrvDesc;

	D3D12_SHADER_RESOURCE_VIEW_DESC depthTextureSrvDesc_;

	//SRV
	ID3D12Device* device_ = nullptr;

	//スカイボックスのテクスチャ番号
	uint32_t skyBoxTexNumber_;

	//Dissolve用のハンドル
	uint32_t dissolveTexNumber_{};


private:
	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム,番号);
	/// </summary>
	std::array<std::pair<std::string, uint32_t>, kMaxSRVConst> textureArray_;

	
	
	size_t slashPos_;
	size_t dotPos_;

};