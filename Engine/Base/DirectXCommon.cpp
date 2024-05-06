#include "DirectXCommon.h"
#include <cassert>
//#include"Log.h"
#include <thread>


DirectXCommon* DirectXCommon::GetInstance(){
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Initialize(){
	winapp_ = WinApp::GetInstance();

	assert(winapp_);

	

	InitializeFixFPS();

	DeviceInitialize();
	CommandInitialize();
	SwapchainInitialize();
	RTVInitialize();
	RenderTextureRTVInitialize();
	srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	DepthBufferInitialize();
	FenceInitialize();



#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//http://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定してメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		//解放
		infoQueue->Release();
	}
#endif // _DEBUG

	ImguiInitialize();
}

void DirectXCommon::DeviceInitialize(){
	//HRESULTはWindows系のエラーコード
	//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	 hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));

	//初期化の根本的な部分でエラーがでた場合はプログラムが間違っているか、
	//どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	ComPtr<IDXGIAdapter4> useAdapter = nullptr;

	//いい順番にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプター情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));//取得できないのは一番危ない
		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力。wstringの方なので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;//ソフトウェアアダプタの場合は見なかったことにする

	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

	
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}

	}
	device->SetName(L"ID3D12Device");
	//デバイスの生成が上手くいかなかったので起動できない
	assert(device != nullptr);
	Log("Conplete create D3D12Device!!!\n");//初期化完了のログ
}

void DirectXCommon::CommandInitialize(){
	//コマンドキューを作成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドアロケータを生成する
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//コマンドアロケータの生成が上手くいかなかったので起動しない
	assert(SUCCEEDED(hr));

	//コマンドリストを生成
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	//コマンドリストの生成が上手くいかなかったので起動しない
	assert(SUCCEEDED(hr));

	
}

void DirectXCommon::SwapchainInitialize(){	

	swapChainDesc.Width = winapp_->kWindowWidth;
	swapChainDesc.Height = winapp_->kWindowHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), winapp_->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

void DirectXCommon::RTVInitialize(){
	rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, swapChainDesc.BufferCount + 1, false);

	const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	backBuffers_.resize(swapChainDesc.BufferCount);

	for (int i = 0; i < backBuffers_.size(); i++){
		//スワップチェーンからバッファを取得
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
		assert(SUCCEEDED(hr));

		
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, i);

		device->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
		chackHandleNum_++;
	}
	
}

void DirectXCommon::RenderTextureRTVInitialize(){
	renderTextureResouce = CreateRenderTextureResource(device, winapp_->kWindowWidth, winapp_->kWindowHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue);

	const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, chackHandleNum_);
	
	device->CreateRenderTargetView(renderTextureResouce.Get(), &rtvDesc, rtvHandle);
}

void DirectXCommon::DepthBufferInitialize(){
	depthStencilResource = CreateDepthStencilTextureResource(device, winapp_->kWindowWidth, winapp_->kWindowHeight);

	dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture

	//DSVHeapの先頭にDSVを作る
	device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

}

void DirectXCommon::FenceInitialize(){
	//初期値0でFenceを作る
	fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));
}

void DirectXCommon::ImguiInitialize(){

}

void DirectXCommon::InitializeFixFPS(){
	//現在時間を記録する
	reference_ = std::chrono::steady_clock::now();

}

void DirectXCommon::UpdateFixFPS(){
	//1/60秒ぴったりの時間
	std::chrono::microseconds kMinTime(uint32_t(1000000.0f / 60.0f));

	//1/60秒よりわずかに短い時間
	std::chrono::microseconds kMinCheckTime(uint32_t(1000000.0f / 65.0f));

	//現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	//前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
	std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60秒(よりわずかに短い時間)経っていない場合
	if (elapsed < kMinCheckTime) {
		//1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}

	}
	//現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void DirectXCommon::PreDrawRenderTexture(){
	////今回のバリアはTransition
	//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	////Noneにしておく
	//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	////バリアを張る対象のリソース。現在のバックバッファに対して行う
	//barrier.Transition.pResource = renderTextureResouce.Get();
	////遷移前(現在)のResouceState
	//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	////遷移後のResouceState
	//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	////TransitionBarrierを張る
	//commandList->ResourceBarrier(1, &barrier);

	//描画用のDescriptorHeapの設定

	ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = { srvDescriptorHeap };
	commandList->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());

	//描画先のRTVとDSVを設定する

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_ = GetCPUDescriptorHandle(
		rtvDescriptorHeap, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), chackHandleNum_);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &rtvHandle_, false, &dsvHandle);

	//指定した色で画面全体をクリアする
	float clearColor[] = { 1.0f,0.f,0.f,1.0f };//青っぽい色、RGBAの順番

	//レンダーターゲットクリア
	commandList->ClearRenderTargetView(rtvHandle_, clearColor, 0, nullptr);

	//深度バッファクリア
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//ビューポート
	
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(winapp_->kWindowWidth);
	viewport.Height = static_cast<float>(winapp_->kWindowHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = winapp_->kWindowWidth;
	scissorRect.top = 0;
	scissorRect.bottom = winapp_->kWindowHeight;

	//ビューポートの設定
	commandList->RSSetViewports(1, &viewport);
	//シザリング矩形の設定
	commandList->RSSetScissorRects(1, &scissorRect);

}

void DirectXCommon::PreDrawSwapChain(){
	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = backBuffers_[backBufferIndex].Get();
	//遷移前(現在)のResouceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResouceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);


	//描画用のDescriptorHeapの設定

	ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = { srvDescriptorHeap };
	commandList->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());

	//描画先のRTVとDSVを設定する

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_ = GetCPUDescriptorHandle(
		rtvDescriptorHeap, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), backBufferIndex);

	commandList->OMSetRenderTargets(1, &rtvHandle_, false, nullptr);

	float clearColor[] = {
	0.0f,
	0.25f,
	0.5f,
	kRenderTargetClearValue.w,
	};

	//レンダーターゲットクリア
	commandList->ClearRenderTargetView(rtvHandle_, clearColor, 0, nullptr);

	//ビューポートの設定
	commandList->RSSetViewports(1, &viewport);
	//シザリング矩形の設定
	commandList->RSSetScissorRects(1, &scissorRect);


	
	

}

void DirectXCommon::PostDraw(){
	//遷移前(現在)のResouceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//遷移後のResouceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);
	//遷移前(現在)のResouceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//遷移後のResouceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	//今回はRenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
	//TransitionのBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	//コマンドリストの内容を確定させる
	hr = commandList->Close();
	assert(SUCCEEDED(hr));

	//GPUにコマンドリストの実行を行わせる
	ComPtr<ID3D12CommandList> commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists->GetAddressOf());

	//GPUとOSに画面の交換を行うよう通知する
	swapChain->Present(1, 0);

#ifdef _DEBUG
	if (FAILED(hr)) {
		ComPtr<ID3D12DeviceRemovedExtendedData> dred;

		hr = device->QueryInterface(IID_PPV_ARGS(&dred));
		assert(SUCCEEDED(hr));

		// 自動パンくず取得
		D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT autoBreadcrumbsOutput{};
		hr = dred->GetAutoBreadcrumbsOutput(&autoBreadcrumbsOutput);
		assert(SUCCEEDED(hr));
	}
#endif

	

	//Fenceの値を更新
	fenceValue++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue->Signal(fence.Get(), fenceValue);
	//Fenceの値が指定したSignal値のたどり着いているか確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence->GetCompletedValue() < fenceValue) {
		//FenceのSignalを待つためのイベントを作成する
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent != nullptr);

		//指定したSignalにたどりついてないので、たどり着くまで待つようにイベントを設定する
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		//イベント待つ
		WaitForSingleObject(fenceEvent, INFINITE);

		CloseHandle(fenceEvent);
	}

	UpdateFixFPS();

	//次のフレーム用のコマンドリストを準備
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator.Get(), nullptr);
	assert(SUCCEEDED(hr));

}

void DirectXCommon::Finalize(){
	
}

void DirectXCommon::Log(const std::string& messaga) {
	OutputDebugStringA(messaga.c_str());
}

std::wstring DirectXCommon::ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string DirectXCommon::ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {

	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(ComPtr<ID3D12Device> device, int32_t width, int32_t height) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Resourceの生成
	ComPtr<ID3D12Resource> resource = nullptr;
	hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateRenderTextureResource(ComPtr<ID3D12Device> device, int32_t width, int32_t height, DXGI_FORMAT format, const Vector4& clearColor){
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//RenderTarGet年て利用可能にする
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//深度値のクリア設定
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;

	//Resourceの生成
	ComPtr<ID3D12Resource> resource = nullptr;
	hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

void DirectXCommon::StartImgui(){
	
}

void DirectXCommon::EndImgui(){
	
}



