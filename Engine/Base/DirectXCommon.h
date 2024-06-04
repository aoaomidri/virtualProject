#pragma once
#include <format>
#include<wrl.h>
#include"WinApp.h"
#include"math/Vector4.h"
#include<vector>
#include<chrono>
#include <d3d12.h> 
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")


class DirectXCommon{
private:
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon& DirectXCommon) = delete;
	DirectXCommon& operator=(const DirectXCommon&) = delete;

public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static DirectXCommon* GetInstance();

	//初期化
	void Initialize();

	//RenderTextureに対して描画前処理
	void PreDrawRenderTexture();

	void PreDrawCopy();


	//SwapChainに対して描画前処理
	void PreDrawSwapChain();

	//描画後処理
	void PostDraw();

	void Finalize();

	/*void SpritePreDraw();

	void SpritePostDraw();*/
	//ログの表示
	void Log(const std::string& messaga);

	std::wstring ConvertString(const std::string& str);

	std::string ConvertString(const std::wstring& str);

	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ComPtr<ID3D12Device> device, int32_t width, int32_t height);

	ComPtr<ID3D12Resource> CreateRenderTextureResource(ComPtr<ID3D12Device> device, int32_t width, int32_t height, DXGI_FORMAT format, const Vector4& clearColor);

	void StartImgui();

	void EndImgui();

	//デバイス取得
	ID3D12Device* GetDevice() const { return device.Get(); }

	//コマンドリスト取得
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }

	//スワップチェーンの設定を取得
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc; }

	//RTVの設定の取得
	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return rtvDesc; }

	//RenderTextureを取得する
	ID3D12Resource* GetRenderTexture()const { return renderTextureResouce.Get(); }

	ID3D12Resource* GetDepthStencil()const { return depthStencilResource.Get(); }


	ID3D12DescriptorHeap* GetSRVHeap() const { return srvDescriptorHeap.Get(); }


private://メンバ関数
	

	//デバイスの初期化
	void DeviceInitialize();

	//コマンド関連の初期化
	void CommandInitialize();

	//スワップチェーン関連の初期化
	void SwapchainInitialize();

	//RTV関連の初期化
	void RTVInitialize();

	void RenderTextureRTVInitialize();

	//深度バッファ関連の初期化
	void DepthBufferInitialize();

	//フェンスの初期化
	void FenceInitialize();

	//Imguiの初期化
	void ImguiInitialize();
	

private://メンバ変数
	//DirectX12デバイス
	ComPtr<ID3D12Device> device = nullptr;

	//DXGIファクトリーの生成
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;

	//コマンドキューの生成
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;

	//コマンドアロケーターの生成
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;

	//コマンドリストの生成
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

	//スワップチェーンの生成
	ComPtr<IDXGISwapChain4> swapChain = nullptr;

	//スワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};



	//バックバッファ
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;

	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;



	//深度バッファの生成
	ComPtr<ID3D12Resource> depthStencilResource;

	ComPtr<ID3D12Resource> renderTextureResouce;

	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	//SRV
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;




	//フェンスの生成
	ComPtr<ID3D12Fence> fence = nullptr;

	uint64_t fenceValue = 0;

	int32_t chackHandleNum_;

	const Vector4 kRenderTargetClearValue{ 0.0f,0.f,0.f,1.0f };//いったんわかりやすいように赤


	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER renderTexBarrier{};

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER swapBarrier{};

	D3D12_VIEWPORT viewport{};

	//シザー矩形
	D3D12_RECT scissorRect{};
	
	//HRESULTはWindows系のエラーコード
	//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = S_FALSE;

	//WindowsAPI
	WinApp* winapp_ = nullptr;
private:
	/*FPS固定用*/

	//記録時間
	std::chrono::steady_clock::time_point reference_;
	 
	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();
	
};

