#pragma once
#include"GraphicsPipeline.h"

class DescriptorHeap{
public:
	DescriptorHeap() = default;
	~DescriptorHeap() = default;
	DescriptorHeap(const DescriptorHeap& input) = delete;
	DescriptorHeap& operator=(const DescriptorHeap&) = delete;

public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static DescriptorHeap* GetInstance();

	ID3D12DescriptorHeap* GetSRVHeap() const { return srvDescriptorHeap.Get(); }

private:
	//SRV
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

	const uint32_t kMaxSRVHeapSize_ = 512;

	uint32_t setHeapPosition_ = 0;
};

