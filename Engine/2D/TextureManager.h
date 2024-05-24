#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include<array>
#include"GraphicsPipeline.h"
#include"Texture.h"
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
	TextureManager(const TextureManager& TextureManager) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
	//TextureManager();

	void Initialize();

	void Finalize();
	

	//	シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	D3D12_GPU_DESCRIPTOR_HANDLE SendGPUDescriptorHandle(uint32_t index)const { return textureSrvHandleGPU[index]; }

	D3D12_GPU_DESCRIPTOR_HANDLE SendInstancingGPUDescriptorHandle()const { return instancingSrvHandleGPU; }

	D3D12_GPU_DESCRIPTOR_HANDLE SendRenderGPUDescriptorHandle()const { return renderTextureSrvHandleGPU; }

	ID3D12Resource* GetTextureBuffer(uint32_t index)const { return textureBuffers_[index].Get(); }

	

	//void Load(const std::string& filePath, uint32_t index);

	uint32_t Load(const std::string& filePath);

	void MakeInstancingShaderResourceView(ID3D12Resource* resource);

	void PreDraw2D();
	
	void PostDraw2D();

	void PreDraw3D();

	void PostDraw3D();

	void PreDrawSkyBox();

	void PostDrawSkyBox();

	void PreDrawSkin3D();

	void PostDrawSkin3D();

	void PreDrawParticle();

	void PostDrawParticle();

	void PreDrawCopy();

	void DrawCopy();

	void MakeShaderResourceView();

public:
	enum  PostEffect{
		None,//なし
		Gray,//グレースケール
		Sepia,//セピア調
		NormalVignetting,//ヴィネッティング
		GrayVignetting,//グレーヴィネッティング
		SepiaVignetting,//セピアヴィネッティング
		VignettingGrayScale,//グレーヴィネッティング
		VignettingSepiaScale,//セピアヴィネッティング
		Smoothing3x3,//スモーシング弱
		Smoothing5x5,//スモーシング中
		Smoothing9x9,//スモーシング強
		Inverse,//色反転
		OutLine,//アウトライン
		Over,//これ以上ないことを表す
	};

	struct Vignetting {
		float scale;//大きさ
		float pow;//掛ける累乗
	};

	void SetVignettingData(const Vignetting& data) {
		vignettingData_->scale = data.scale;
		vignettingData_->pow = data.pow;
	}

	void SetPostEffect(const PostEffect& name) {
		selectPost_ = name;
	}

	void SetMatProjectionInverse(const Matrix4x4& mat) {
		matProjectionInverse_ = mat;
	}

public:

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		uint32_t descriptorSize, uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		uint32_t descriptorSize, uint32_t index);

private:
	// DirectX12のTextureResourceを作る
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//	TextureResourceにデータを転送する
	[[nodiscard]] ComPtr<ID3D12Resource> UploadTextureData(
		ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages
		);

	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	void CreateVignettingResource();

private:
	//SRVの最大個数
	static const size_t kMaxSRVConst = 257;

	//テクスチャバッファ
	std::array<ComPtr<ID3D12Resource>, kMaxSRVConst> textureBuffers_;

	std::array<ComPtr<ID3D12Resource>, kMaxSRVConst> intermediateBuffers_;


	std::unique_ptr<GraphicsPipeline> GraphicsPipeline2D_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipeline3D_;

	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSkyBox_;

	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSkinning3D_;

	//ポストエフェクト
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineCopy_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineGray_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSepia_;

	//色反転
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineInverse_;

	/*ヴィネッティング*/
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineNormalVignetting_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineGrayVignetting_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSepiaVignetting_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineVignettingGrayScale_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineVignettingSepiaScale_;

	//Smoothing
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSmoothing3x3;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSmoothing5x5;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSmoothing9x9;

	//LuminanceOutLine
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineOutLine_;


	ComPtr<ID3D12Resource> textureResource;
	ComPtr<ID3D12Resource> intermediateResource;

	ComPtr<ID3D12Resource> vignettingResource_;

	Vignetting* vignettingData_ = nullptr;

	////SRVを作成するDescripterHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU[kMaxSRVConst]{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU[kMaxSRVConst]{};
	//インスタンシング用のSRV作成用
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU{};

	//RenderTexture用のSRV作成用
	D3D12_CPU_DESCRIPTOR_HANDLE renderTextureSrvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE renderTextureSrvHandleGPU{};

	DirectX::ScratchImage mipImages;
	DirectX::TexMetadata metadata;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc;

	D3D12_SHADER_RESOURCE_VIEW_DESC recderTextureSrvDesc;

	//SRV
	ID3D12Device* device_ = nullptr;

	//どのポストエフェクトを選択しているか
	uint32_t selectPost_ = 0;

	Matrix4x4 matProjectionInverse_;

private:
	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム,番号);
	/// </summary>
	std::array<std::pair<std::string, uint32_t>, kMaxSRVConst> textureArray_;

	
	
	size_t slashPos_;
	size_t dotPos_;

};