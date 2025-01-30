#pragma once
#include <format>
#include<wrl.h>
#include"WinApp.h"
#include"math/Vector4.h"
#include"DescriptorHeap.h"
#include<vector>
#include<chrono>
#include <d3d12.h> 
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
/*DirectX関係の処理*/

namespace MAEngine {
	class DirectXCommon {
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
		//終了処理
		void Finalize();

		//ログの表示
		void Log(const std::string& messaga);
		/*文字列変換処理*/
		std::wstring ConvertString(const std::string& str);

		std::string ConvertString(const std::wstring& str);
		//ヒープの作成
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
		//CPUハンドルの取得
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
		//DepthStencilTextureResourceの生成
		ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ComPtr<ID3D12Device> device, int32_t width, int32_t height);
		//RenderTextureResourceの生成
		ComPtr<ID3D12Resource> CreateRenderTextureResource(ComPtr<ID3D12Device> device, int32_t width, int32_t height, DXGI_FORMAT format, const Vector4& clearColor);


		//デバイス取得
		ID3D12Device* GetDevice() const { return device_.Get(); }

		//コマンドリスト取得
		ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

		//スワップチェーンの設定を取得
		DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc_; }

		//RTVの設定の取得
		D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return rtvDesc_; }

		//RenderTextureを取得する
		ID3D12Resource* GetRenderTexture()const { return renderTextureResouce_.Get(); }

		ID3D12Resource* GetDepthStencil()const { return depthStencilResource_.Get(); }


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
		ComPtr<ID3D12Device> device_ = nullptr;

		//DXGIファクトリーの生成
		ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

		//コマンドキューの生成
		ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

		//コマンドアロケーターの生成
		ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

		//コマンドリストの生成
		ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

		//スワップチェーンの生成
		ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

		//スワップチェーンを生成
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

		//RTVの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};



		//バックバッファ
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;

		ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;



		//深度バッファの生成
		ComPtr<ID3D12Resource> depthStencilResource_;

		ComPtr<ID3D12Resource> renderTextureResouce_;

		ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

		SRVDescriptorHeap* srvDescriptorHeap_ = nullptr;

		//フェンスの生成
		ComPtr<ID3D12Fence> fence_ = nullptr;

		uint64_t fenceValue_ = 0;

		int32_t chackHandleNum_;

		const Vector4 kRenderTargetClearValue_{ 0.0f,0.25f,0.5f,1.0f };//いったんわかりやすいように赤


		//TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER renderTexBarrier_{};

		//TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER swapBarrier_{};

		//バリア
		D3D12_RESOURCE_BARRIER depthBarrier_{};

		D3D12_VIEWPORT viewport_{};

		//シザー矩形
		D3D12_RECT scissorRect_{};

		//HRESULTはWindows系のエラーコード
		//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
		HRESULT hr_ = S_FALSE;

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
}