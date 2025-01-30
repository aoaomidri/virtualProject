#pragma once
#include"GraphicsPipeline.h"
/*heapの管理*/

namespace MAEngine {
	class SRVDescriptorHeap {
	public:
		SRVDescriptorHeap() = default;
		~SRVDescriptorHeap() = default;
		SRVDescriptorHeap(const SRVDescriptorHeap& input) = delete;
		SRVDescriptorHeap& operator=(const SRVDescriptorHeap&) = delete;

	public:
		//namespace省略
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

		static SRVDescriptorHeap* GetInstance();
		/// <summary>
		/// 初期化処理
		/// </summary>
		/// <param name="device">デバイス</param>
		void Initialize(ID3D12Device* device);

		/// DescriptorHeapの生成
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	public:
		/*getter*/
		ID3D12DescriptorHeap* GetSRVHeap() const { return srvDescriptorHeap_.Get(); }


		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle() {
			D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
			handleCPU.ptr += (size_t)(descriptorSizeSRV_ * setHeapPositionCPU_);
			setHeapPositionCPU_++;
			return handleCPU;
		}

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle() {
			D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();
			handleGPU.ptr += (size_t)(descriptorSizeSRV_ * setHeapPositionGPU_);
			setHeapPositionGPU_++;
			return handleGPU;
		}


	private:
		//SRV
		ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

		const uint32_t kMaxSRVHeapSize_ = 1024;

		int32_t setHeapPositionCPU_ = 0;

		int32_t setHeapPositionGPU_ = 0;

		ID3D12Device* device_ = nullptr;

		const D3D12_DESCRIPTOR_HEAP_TYPE srvHeapType_ = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		uint32_t descriptorSizeSRV_;

		HRESULT hr_;
	};

}