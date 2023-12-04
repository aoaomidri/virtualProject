#pragma once
#include <d3d12.h> 
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include<dxgidebug.h>
#pragma comment(lib,"dxguid.lib")
#include<dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
class ResourceObject
{
public:
	ResourceObject(ID3D12Resource* resource);
	~ResourceObject();

	ID3D12Resource* Get() { return resource_; }
private:
	ID3D12Resource* resource_;

};

