#pragma once
// for MediaFoundation
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mfmediaengine.h>
#include <filesystem>

#include <wrl.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <cstdint>
#include"TextureManager.h"


class Movie{
public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//初期化
	void Initailize();
	//更新処理
	void Update();

	//動画読み込み
	uint32_t Load(const std::string& filePath);

	//解放処理
	void Finalize();

	//再生
	void Play();
	//停止
	void Stop();

	void OnMediaEngineEvent(uint32_t meEvent);

	bool TransferFrame();

	bool IsPlaying() const { return m_isPlaying; }
	bool IsFinished() const { return m_isFinished; }

	void SetLoop(bool enable);

private:
	//メンバ関数
	//void GetMovieData();


public:
	//	シングルトンインスタンスの取得
	static Movie* GetInstance();

	D3D12_GPU_DESCRIPTOR_HANDLE SendGPUDescriptorHandle(uint32_t index)const { return textureSrvHandleGPU[index]; }

	ID3D12Resource* GetTextureBuffer(uint32_t index)const { return textureBuffers_[index].Get(); }

private:
	// DirectX12のTextureResourceを作る
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//	TextureResourceにデータを転送する
	[[nodiscard]] ComPtr<ID3D12Resource> UploadTextureData(
		ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages
	);

	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		uint32_t descriptorSize, uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		uint32_t descriptorSize, uint32_t index);

private:
	const uint32_t movieSRVmin = 300;
	//SRVの最大個数
	static const size_t kMaxSRVConst = 7200;

	//テクスチャバッファ
	std::array<ComPtr<ID3D12Resource>, kMaxSRVConst> textureBuffers_;

	std::array<ComPtr<ID3D12Resource>, kMaxSRVConst> intermediateBuffers_;

	ComPtr<ID3D12Resource> textureResource;
	ComPtr<ID3D12Resource> intermediateResource;

	////SRVを作成するDescripterHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU[kMaxSRVConst]{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU[kMaxSRVConst]{};

	DirectX::ScratchImage mipImages;
	DirectX::TexMetadata metadata;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc;

	//SRV
	ID3D12Device* device_ = nullptr;

private:
	/// <summary>
	/// Textureのコンテナ(キー値: ファイルネーム,番号);
	/// </summary>
	std::array<std::pair<std::string, uint32_t>, kMaxSRVConst> textureArray_;

	size_t slashPos_;
	size_t dotPos_;
	/*movie特有の変数又は、関数群*/
private:
	//メンバ関数
	
	void Terminate();

	//void Update(ComPtr<ID3D12GraphicsCommandList> commandList);

	//void SetMediaSource(const Path& fileName);


	

private:
	void DecodeFrame();
	void UpdateTexture();
	void ResetPosition();

private:
	Movie() = default;
	~Movie() = default;
	Movie(const Movie& DirectXCommon) = delete;
	Movie& operator=(const Movie&) = delete;

	struct MovieFrameInfo {
		double timestamp;
		int bufferIndex;
	};

	HRESULT hr_;

	ComPtr<ID3D11Device1> m_d3d11Device;
	
	ComPtr<IMFMediaEngine> m_mediaEngine;

	std::string movieFilePath_ = "resources/movie/";

	std::list<MovieFrameInfo> m_decoded;

	IMFAttributes* attribs = nullptr;

	IMFSourceReader* sourceReader = nullptr;

	GUID desiredFormat;
	ComPtr<IMFMediaType> mediaType;

	DWORD width, height;

	long m_cRef;

	double fps_;

	static const int DecodeBufferCount = 2;

	DXGI_FORMAT m_format;
	bool m_isPlaying = false;
	bool m_isFinished = false;
	bool m_isDecodeFinished = false;
	bool m_isLoop = false;

	int m_writeBufferIndex = 0;

	ComPtr<ID3D12Resource> m_frameDecoded[DecodeBufferCount];

	LARGE_INTEGER m_startedTime;
	ComPtr<ID3D12Resource> m_movieTextureRes;
	
	
	UINT m_srcStride = 0;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT m_layouts;
	UINT64 m_rowPitchBytes = 0;

	HANDLE m_hSharedTexture;
	HANDLE m_hPrepare;
};

