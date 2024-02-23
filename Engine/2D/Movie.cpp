#include "Movie.h"

#include"DirectXCommon.h"
#include <stdexcept>

#pragma comment(lib, "d3d11.lib")
// for MediaFoundation
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")

class MediaEngineNotify : public IMFMediaEngineNotify {
	long m_cRef;
	Movie* m_callback;
public:
	MediaEngineNotify() : m_cRef(0), m_callback(nullptr) {
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		if (__uuidof(IMFMediaEngineNotify) == riid) {
			*ppv = static_cast<IMFMediaEngineNotify*>(this);
		}
		else {
			*ppv = nullptr;
			return E_NOINTERFACE;
		}
		AddRef();
		return S_OK;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_cRef);
	}
	STDMETHODIMP_(ULONG) Release()
	{
		LONG cRef = InterlockedDecrement(&m_cRef);
		if (cRef == 0) {
			delete this;
		}
		return cRef;
	}
	void SetCallback(Movie* target)
	{
		m_callback = target;
	}

	STDMETHODIMP EventNotify(DWORD meEvent, DWORD_PTR param1, DWORD param2)
	{
		if (meEvent == MF_MEDIA_ENGINE_EVENT_NOTIFYSTABLESTATE)
		{
			SetEvent(reinterpret_cast<HANDLE>(param1));
		}
		else
		{
			m_callback->OnMediaEngineEvent(meEvent);
		}
		return S_OK;
	}


};

void Movie::Initailize(){
	MFStartup(MF_VERSION);
	device_ = DirectXCommon::GetInstance()->GetDevice();
	m_format = DXGI_FORMAT_B8G8R8A8_UNORM;

	ComPtr<ID3D11Device> deviceD3D11;
	D3D11CreateDevice(
		DirectXCommon::GetInstance()->GetAdapter(),
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		deviceD3D11.ReleaseAndGetAddressOf(),
		nullptr,
		nullptr);
	deviceD3D11.As(&m_d3d11Device);

	ComPtr<ID3D10Multithread> multithread;
	deviceD3D11.As(&multithread);
	multithread->SetMultithreadProtected(TRUE);

	// Media Engine のセットアップ.
	ComPtr<IMFDXGIDeviceManager> dxgiManager;
	UINT resetToken = 0;
	MFCreateDXGIDeviceManager(&resetToken, dxgiManager.ReleaseAndGetAddressOf());
	dxgiManager->ResetDevice(m_d3d11Device.Get(), resetToken);

	ComPtr<MediaEngineNotify> notify = new MediaEngineNotify();
	notify->SetCallback(this);

	ComPtr<IMFAttributes> attributes;
	MFCreateAttributes(attributes.GetAddressOf(), 1);
	attributes->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER, dxgiManager.Get());
	attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, notify.Get());
	//出力フォーマット指定
	attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, m_format);

	// Create media engine
	ComPtr<IMFMediaEngineClassFactory> mfFactory;
	CoCreateInstance(CLSID_MFMediaEngineClassFactory,
		nullptr,
		CLSCTX_ALL,
		IID_PPV_ARGS(mfFactory.GetAddressOf()));

	//IMFMediaEngineの取得
	mfFactory->CreateInstance(
		0, attributes.Get(), m_mediaEngine.ReleaseAndGetAddressOf());

	width = 0;
	height = 0;

	m_cRef = 1;
	m_hPrepare = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void Movie::Update(){
	if (!m_isPlaying) {
		return;
	}
	DecodeFrame();
	UpdateTexture();

}

void Movie::Finalize() {
	Stop();
	if (m_mediaEngine) {
		m_mediaEngine->Shutdown();
	}
	m_mediaEngine.Reset();
	m_d3d11Device.Reset();

	if (m_hSharedTexture) {
		CloseHandle(m_hSharedTexture);
		m_hSharedTexture = 0;
	}
	if (m_hPrepare) {
		CloseHandle(m_hPrepare);
		m_hPrepare = 0;
	}
}

Movie* Movie::GetInstance(){
	static Movie instance;
	return &instance;
}

uint32_t Movie::Load(const std::string& filePath){
	int i = 0;
	bool isLoad = false;
	std::string result;

	slashPos_ = (movieFilePath_ + filePath).find_last_of('/');
	dotPos_ = (movieFilePath_ + filePath).find_last_of('.');
	if (slashPos_ != std::string::npos && dotPos_ != std::string::npos && dotPos_ > slashPos_) {
		result = (movieFilePath_ + filePath).substr(slashPos_ + 1, dotPos_ - slashPos_ - 1);
	}

	while (!textureArray_[i].first.empty()) {
		if (textureArray_[i].first == result) {
			isLoad = true;
			return i;
		}
		i++;
	}

	if (isLoad) {
		return i;
	}

	BSTR bstrURL = nullptr;
	if (bstrURL != nullptr) {
		::CoTaskMemFree(bstrURL);
		bstrURL = nullptr;
	}

	/*IMFSourceReaderインターフェースの取得*/
	auto fullPath = std::filesystem::absolute((movieFilePath_ + filePath)).wstring();

	size_t cchAllocationSize = 1 + fullPath.size();
	bstrURL = reinterpret_cast<BSTR>(::CoTaskMemAlloc(sizeof(wchar_t) * (cchAllocationSize)));


	HRESULT hr;
	wcscpy_s(bstrURL, cchAllocationSize, fullPath.c_str());
	m_isPlaying = false;
	m_isFinished = false;
	hr = m_mediaEngine->SetSource(bstrURL);

	WaitForSingleObject(m_hPrepare, INFINITE);

	m_mediaEngine->GetNativeVideoSize(&width, &height);

	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
	D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	/*(//フォーマット変換のために必要

	//MFCreateAttributes(&attribs, 1);
	//attribs->SetUINT32(
	//	MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

	//hr_ = MFCreateSourceReaderFromURL(
	//	fullPath.c_str(), attribs, &sourceReader);

	//if (FAILED(hr_)) {
	//	throw std::runtime_error("MFCreateSourceReaderFromURL() Failed");
	//}
	//
	///*フォーマットの設定*/
	//desiredFormat = MFVideoFormat_RGB32;
	//if (m_format == DXGI_FORMAT_B8G8R8A8_UNORM) {
	//	desiredFormat = MFVideoFormat_RGB32;
	//}
	//if (m_format == DXGI_FORMAT_NV12) {
	//	desiredFormat = MFVideoFormat_NV12;
	//}

	//MFCreateMediaType(&mediaType);
	//mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	//hr_ = mediaType->SetGUID(MF_MT_SUBTYPE, desiredFormat);
	//hr_ = sourceReader->SetCurrentMediaType(
	//	MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, mediaType.Get());

	///*ビデオ情報の取得*/
	//sourceReader->GetCurrentMediaType(
	//	MF_SOURCE_READER_FIRST_VIDEO_STREAM, &mediaType);
	//MFGetAttributeSize(mediaType.Get(), MF_MT_FRAME_SIZE, &width, &height);
	//UINT32 nume, denom;
	//MFGetAttributeRatio(mediaType.Get(), MF_MT_FRAME_RATE, &nume, &denom);

	//fps_ = (double)nume / denom;

	//hr_ = mediaType->GetUINT32(MF_MT_DEFAULT_STRIDE, &m_srcStride);)*/

	/*テクスチャリソースの準備*/
	auto resDescMovieTex = CD3DX12_RESOURCE_DESC::Tex2D(
		m_format, width, height, 1, 1);
	resDescMovieTex.Flags = flags;

	auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	device_->CreateCommittedResource(
		&defaultHeap, D3D12_HEAP_FLAG_SHARED,
		&resDescMovieTex, initialState,
		nullptr, IID_PPV_ARGS(&textureBuffers_[0]));

	//ShaderResourceViewの準備
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = 
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (m_format == DXGI_FORMAT_R8G8B8A8_UNORM){
		srvDesc.Format = resDescMovieTex.Format;

		const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		textureSrvHandleCPU[0] = GetCPUDescriptorHandle(descriptorSizeSRV, movieSRVmin);
		textureSrvHandleGPU[0] = GetGPUDescriptorHandle(descriptorSizeSRV, movieSRVmin);

		device_->CreateShaderResourceView(
			textureBuffers_[0].Get(), &srvDesc, textureSrvHandleCPU[0]);
	}
	if (m_format == DXGI_FORMAT_NV12) {
		const uint32_t descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		textureSrvHandleCPU[0] = GetCPUDescriptorHandle(descriptorSizeSRV, movieSRVmin);
		textureSrvHandleGPU[0] = GetGPUDescriptorHandle(descriptorSizeSRV, movieSRVmin);
		textureSrvHandleCPU[1] = GetCPUDescriptorHandle(descriptorSizeSRV, movieSRVmin + 1);
		textureSrvHandleGPU[1] = GetGPUDescriptorHandle(descriptorSizeSRV, movieSRVmin + 1);

		srvDesc.Format = DXGI_FORMAT_R8_UNORM;
		srvDesc.Texture2D.PlaneSlice = 0;
		device_->CreateShaderResourceView(textureBuffers_[0].Get(), &srvDesc, textureSrvHandleCPU[0]);

		srvDesc.Format = DXGI_FORMAT_R8G8_UNORM;
		srvDesc.Texture2D.PlaneSlice = 1;
		device_->CreateShaderResourceView(textureBuffers_[0].Get(), &srvDesc, textureSrvHandleCPU[1]);
	}

	// 共有ハンドルの処理.
	hr = device_->CreateSharedHandle(
		textureBuffers_[0].Get(),
		nullptr,
		GENERIC_ALL,
		nullptr,
		&m_hSharedTexture);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed CreateSharedHandle");
	}


	////テクスチャ転送用バッファ(ステージングバッファ)	

	//UINT64 totalBytes = 0;
	//UINT64 m_rowPitchBytes = 0;
	//UINT numRows = 0;
	//D3D12_PLACED_SUBRESOURCE_FOOTPRINT m_layouts;

	//device_->GetCopyableFootprints(
	//	&resDescMovieTex,
	//	0, 1, 0, &m_layouts,
	//	&numRows, &m_rowPitchBytes, &totalBytes);
	//totalBytes = (std::max)(totalBytes, numRows * m_rowPitchBytes);
	//auto stagingDesc = CD3DX12_RESOURCE_DESC::Buffer(totalBytes);
	//for (auto& buffer : m_frameDecoded) {
	//	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//	hr_ = device_->CreateCommittedResource(
	//		&heapProps, D3D12_HEAP_FLAG_NONE, &stagingDesc,
	//		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer));
	//	if (FAILED(hr_)) {
	//		throw std::runtime_error("CreateCommittedResource failed.");
	//	}
	//}

	return i;
}


void Movie::Terminate(){

}

void Movie::Play(){
	if (m_isPlaying) {
		return;
	}

	if (m_mediaEngine) {
		m_mediaEngine->Play();
		m_isPlaying = true;
	}
}

void Movie::Stop(){
	if (!m_isPlaying) {
		return;
	}

	if (m_mediaEngine) {
		m_mediaEngine->Pause();
		m_mediaEngine->SetCurrentTime(0.0);
	}

	m_isPlaying = false;
	m_isFinished = false;
}

void Movie::OnMediaEngineEvent(uint32_t meEvent){
	switch (meEvent)
	{
	case MF_MEDIA_ENGINE_EVENT_LOADSTART:
		OutputDebugStringA("MF_MEDIA_ENGINE_EVENT_LOADSTART\n");
		break;
	case MF_MEDIA_ENGINE_EVENT_PROGRESS:
		OutputDebugStringA("MF_MEDIA_ENGINE_EVENT_PROGRESS\n");
		break;
	case MF_MEDIA_ENGINE_EVENT_LOADEDDATA:
		SetEvent(m_hPrepare);
		break;
	case MF_MEDIA_ENGINE_EVENT_PLAY:
		OutputDebugStringA("MF_MEDIA_ENGINE_EVENT_PLAY\n");
		break;

	case MF_MEDIA_ENGINE_EVENT_CANPLAY:
		break;

	case MF_MEDIA_ENGINE_EVENT_WAITING:
		OutputDebugStringA("MF_MEDIA_ENGINE_EVENT_WAITING\n");
		break;

	case MF_MEDIA_ENGINE_EVENT_TIMEUPDATE:
		break;
	case MF_MEDIA_ENGINE_EVENT_ENDED:
		OutputDebugStringA("MF_MEDIA_ENGINE_EVENT_ENDED\n");
		if (m_mediaEngine) {
			m_mediaEngine->Pause();
			m_isPlaying = false;
			m_isFinished = true;
		}
		break;

	case MF_MEDIA_ENGINE_EVENT_ERROR:
		if (m_mediaEngine) {
			ComPtr<IMFMediaError> err;
			if (SUCCEEDED(m_mediaEngine->GetError(&err))) {
				USHORT errCode = err->GetErrorCode();
				HRESULT hr = err->GetExtendedErrorCode();
				char buf[256] = { 0 };
				sprintf_s(buf, "ERROR: Media Foundation Event Error %u (%08X)\n", errCode, static_cast<unsigned int>(hr));
				OutputDebugStringA(buf);
			}
			else
			{
				OutputDebugStringA("ERROR: Media Foundation Event Error *FAILED GetError*\n");
			}
		}
		break;
	}
}

bool Movie::TransferFrame(){
	if (m_mediaEngine && m_isPlaying) {
		LONGLONG pts;
		HRESULT hr = m_mediaEngine->OnVideoStreamTick(&pts);
		if (SUCCEEDED(hr)) {
			ComPtr<ID3D11Texture2D> mediaTexture;
			hr = m_d3d11Device->OpenSharedResource1(m_hSharedTexture, IID_PPV_ARGS(mediaTexture.GetAddressOf()));
			if (SUCCEEDED(hr)) {
				MFVideoNormalizedRect rect{ 0.0f, 0.0f, 1.0f, 1.0f };
				RECT rcTarget{ 0, 0, LONG(width), LONG(height) };

				MFARGB  m_bkgColor{ 0xFF, 0xFF, 0xFF, 0xFF };
				hr = m_mediaEngine->TransferVideoFrame(
					mediaTexture.Get(), &rect, &rcTarget, &m_bkgColor);
				if (SUCCEEDED(hr)) {
					return true;
				}
			}
		}
	}
	return false;
}

void Movie::SetLoop(bool enable){
	if (m_mediaEngine) {
		m_mediaEngine->SetLoop(enable);
	}
}

void Movie::DecodeFrame(){
	if (m_decoded.size() == DecodeBufferCount) {
		return;
	}

	if (sourceReader) {
		DWORD flags;
		ComPtr<IMFSample> sample;
		HRESULT hr = sourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, &flags, NULL, &sample);
		if (SUCCEEDED(hr)) {
			if (flags == MF_SOURCE_READERF_ENDOFSTREAM) {
				//ストリームの終了
				OutputDebugStringA("Stream End.\n");

				ResetPosition();
				m_isDecodeFinished = true;
			}
			else {
				// サンプルデータからバッファを取得
				ComPtr<IMFMediaBuffer> buffer;
				sample->GetBufferByIndex(0, &buffer);

				// テクスチャへの書き込み
				BYTE* src;
				DWORD size;
				UINT srcPitch = width * sizeof(UINT);
				buffer->Lock(&src, NULL, &size);

				BYTE* dst = nullptr;
				auto writeBuffer = m_frameDecoded[m_writeBufferIndex];
				writeBuffer->Map(0, nullptr, (void**)&dst);
				memcpy(dst, src, srcPitch * height);
				writeBuffer->Unmap(0, nullptr);
				buffer->Unlock();

				// 時間情報の取得.
				int64_t ts = 0;
				int64_t duration = 0;

				sample->GetSampleTime(&ts);
				sample->GetSampleDuration(&duration);

				// 100ns単位 => ns 単位へ.
				ts *= 100;
				duration *= 100;
				double durationSec = double(duration) / 1000000000;
				double timestamp = double(ts) / 1000000000;

				// キューイング
				MovieFrameInfo frameInfo{};
				frameInfo.timestamp = timestamp;
				frameInfo.bufferIndex = m_writeBufferIndex;

				m_decoded.push_back(frameInfo);

				m_writeBufferIndex = (m_writeBufferIndex + 1) % DecodeBufferCount;
			}
		}
	}
}

void Movie::UpdateTexture(){
	if (m_decoded.empty()) {
		return;
	}
	auto frameInfo = m_decoded.front();

	LARGE_INTEGER now, freq;
	QueryPerformanceCounter(&now);
	QueryPerformanceFrequency(&freq);
	double playTime = double(now.QuadPart - m_startedTime.QuadPart) / freq.QuadPart;

	// 表示してよい時間かチェック.
	if (playTime < frameInfo.timestamp) {
		//OutputDebugStringA("Stay...\n");
		return;
	}
	m_decoded.pop_front();

	if (m_isDecodeFinished == true && m_decoded.empty()) {
		m_isFinished = true;
		if (m_isLoop) {
			m_isFinished = false;
			m_isDecodeFinished = false;
			QueryPerformanceCounter(&m_startedTime);
		}
	}

	// テクスチャの転送
	D3D12_TEXTURE_COPY_LOCATION dst{}, src{};
	dst.pResource = m_movieTextureRes.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	src.pResource = m_frameDecoded[frameInfo.bufferIndex].Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = m_layouts;
	DirectXCommon::GetInstance()->GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
}

void Movie::ResetPosition(){
	PROPVARIANT varPosition;
	PropVariantInit(&varPosition);
	varPosition.vt = VT_I8;
	varPosition.hVal.QuadPart = 0;
	sourceReader->SetCurrentPosition(GUID_NULL, varPosition);
	PropVariantClear(&varPosition);
}

DirectX::ScratchImage Movie::LoadTexture(const std::string& filePath){
	//	テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//	ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));
	//	ミニマップ付きのデータを返す
	return mipImages;
}

ComPtr<ID3D12Resource> Movie::UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages){
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device_, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);
	UpdateSubresources(DirectXCommon::GetInstance()->GetCommandList(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	DirectXCommon::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

ComPtr<ID3D12Resource> Movie::CreateBufferResource(size_t sizeInBytes){
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;

	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ComPtr<ID3D12Resource> bufferResource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(hr));

	return bufferResource;
}

ComPtr<ID3D12Resource> Movie::CreateTextureResource(const DirectX::TexMetadata& metadata){
	//1, metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//Textureの幅
	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行きor配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元

	//2, 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//細かい設定

	//3, Respurceを生成する
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,//初回のResourceState。Textureは基本読むだけ
		nullptr,//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE Movie::GetCPUDescriptorHandle(uint32_t descriptorSize, uint32_t index){
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = DirectXCommon::GetInstance()->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE Movie::GetGPUDescriptorHandle(uint32_t descriptorSize, uint32_t index){
	D3D12_GPU_DESCRIPTOR_HANDLE handleCPU = DirectXCommon::GetInstance()->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}