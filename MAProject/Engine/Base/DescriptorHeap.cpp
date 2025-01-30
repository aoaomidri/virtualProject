#include "DescriptorHeap.h"
#include <cassert>

using namespace MAEngine;

SRVDescriptorHeap* SRVDescriptorHeap::GetInstance(){
	static SRVDescriptorHeap instance;
	return &instance;
}

void SRVDescriptorHeap::Initialize(ID3D12Device* device){
	device_ = device;
	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(srvHeapType_);

	srvDescriptorHeap_ = CreateDescriptorHeap(srvHeapType_, kMaxSRVHeapSize_, true);

}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVDescriptorHeap::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible){
	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr_ = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr_));
	return descriptorHeap;
}
