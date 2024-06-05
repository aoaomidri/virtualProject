#include "DescriptorHeap.h"

DescriptorHeap* DescriptorHeap::GetInstance(){
	static DescriptorHeap instance;
	return &instance;
}
