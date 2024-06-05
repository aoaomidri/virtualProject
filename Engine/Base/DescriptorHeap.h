#pragma once
#include"GraphicsPipeline.h"

class DescriptorHeap{
public:
	DescriptorHeap() = default;
	~DescriptorHeap() = default;
	DescriptorHeap(const DescriptorHeap& input) = delete;
	DescriptorHeap& operator=(const DescriptorHeap&) = delete;

public:
	static DescriptorHeap* GetInstance();
};

