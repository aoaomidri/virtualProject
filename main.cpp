#include"Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"Sprite.h"
#include <cassert>
#include<fstream>
#include<sstream>


//struct VertexData {
//	Vector4 position;
//	Vector2 texcoord;
//	Vector3 normal;
//};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};


struct Material{
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

//struct TransformationMatrix{
//	Matrix4x4 WVP;
//	Matrix4x4 World;
//};

struct DirectionalLight {
	Vector4 color;		//ライトの色
	Vector3 direction;	//ライトの向き
	float intensity;	//輝度
};

Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result = { 0.0f };
	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1.0f;
	}
	return result;
}

IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
{
	////////1, hlslファイルを読む
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知
	////////2, Compilerする
	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",//デバッグ等の情報を埋め込む
		L"-Od",//最適化を外しておく
		L"-Zpr",//メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler,
		IID_PPV_ARGS(&shaderResult)
	);
	//コンパイルエラーではなくDxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));
	////////3, 警告・エラーが出ていないか確認する
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//警告・エラーダメ絶対
		assert(false);
	}
	////////4, Compiler結果を受けとって返す
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

Microsoft::WRL::ComPtr<ID3D12Resource> BufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes) {

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

	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(hr));

	return bufferResource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height) {
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
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors,bool shaderVisible) {

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker()
	{
		//リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//1,中で必要となる変数の宣言
	MaterialData materialData;//構築するMaterialData
	std::string line;//ファイルから読んだ1行を格納するもの	

	//2,ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());//とりあえず開けなかったら止める

	//3,実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}

	}
	//4,MaterialDataを返す
	return materialData;
}


ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	//1,中で必要になる変数の宣言
	ModelData modelData;//構築するModelData
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャ座標
	std::string line;//ファイルから読んだ1行を格納するもの

	//2,ファイルを開く

	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める

	//3,実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む
		//identifierに応じた処理
		if (identifier == "v") {
			Vector4 position{};
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord{};
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal{};
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3]{};
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのindexは「位置/UV/法線」で格納されているので、分解してindexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3]{};
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');// /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);

				}
				//要素へのindexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				/*VertexData vertex = { position,texcoord,normal };
				modelData.vertices.push_back(vertex);*/
				triangle[faceVertex] = { position,texcoord,normal };

			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルに同一改装にmtlは存在させるので、ディレクトリとファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}

	}

	//4,ModelDataを返す
	return modelData;
}



//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//CoInitializeEx(0, COINIT_MULTITHREADED);

	D3DResourceLeakChecker leakCheck;

	Matrix* matrix = new Matrix;

	auto window_ = std::make_unique<WinApp>();
	window_->Initialize();

	//HRESULT hr;

	/*キー入力の初期化処理*/
	auto input_ = std::make_unique<Input>();

	input_->Initialize(window_.get());

	auto dxCommon_ = std::make_unique<DirectXCommon>();
	dxCommon_->Initialize(window_.get());

	auto sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(dxCommon_->GetDevice(), dxCommon_->GetCommandList());



	/*ここから先整理前なのでごちゃごちゃしてるゾーン*/

	////モデル読み込み
	//ModelData modelData = LoadObjFile("resources/fence", "fence.obj");

	////Textureを読んで転送する
	//DirectX::ScratchImage mipImages[2]{};
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource[2]{};
	//Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource[2]{};
	//mipImages[0] = texture_->LoadTexture(modelData.material.textureFilePath);
	//mipImages[1] = texture_->LoadTexture("resources/monsterBall.png");
	//const DirectX::TexMetadata& metadata = mipImages[0].GetMetadata();
	//const DirectX::TexMetadata& metadata2 = mipImages[1].GetMetadata();

	//textureResource[0] = texture_->CreateTextureResource(dxCommon_->GetDevice(), metadata);
	//textureResource[1] = texture_->CreateTextureResource(dxCommon_->GetDevice(), metadata2);
	//intermediateResource[0] = texture_->UploadTextureData(textureResource[0], mipImages[0], dxCommon_->GetDevice(), dxCommon_->GetCommandList());
	//intermediateResource[1] = texture_->UploadTextureData(textureResource[1], mipImages[1], dxCommon_->GetDevice(), dxCommon_->GetCommandList());
	//

	////dxcCompilerを初期化
	//IDxcUtils* dxcUtils = nullptr;
	//IDxcCompiler3* dxcCompiler = nullptr;
	//hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	//assert(SUCCEEDED(hr));
	//hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	//assert(SUCCEEDED(hr));

	////現時点でincludeはしないが、includeに対応するための設定を行っておく
	//IDxcIncludeHandler* includeHandler = nullptr;
	//hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	//assert(SUCCEEDED(hr));

	////RootSignature作成
	//D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	//descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	////DescriptorRangeの設定
	//D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	//descriptorRange[0].NumDescriptors = 1;//数は1つ
	//descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算

	////RootParameter作成。複数設定できるので配列。
	//D3D12_ROOT_PARAMETER rootParameter[4] = {};
	//rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//rootParameter[0].Descriptor.ShaderRegister = 1;

	//rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//rootParameter[1].Descriptor.ShaderRegister = 0;

	//descriptionRootSignature.pParameters = rootParameter;
	//descriptionRootSignature.NumParameters = _countof(rootParameter);

	//rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	//rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	//rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	//rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//rootParameter[3].Descriptor.ShaderRegister = 2;

	////Samplerの設定
	//D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	//staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	//staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0～1の範囲外をリピート
	//staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	//staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	//staticSampler[0].ShaderRegister = 0;//レジスタ番号0を使う
	//staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う

	//descriptionRootSignature.pStaticSamplers = staticSampler;
	//descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);

	////シリアライズしてバイナリする
	//ID3DBlob* signatureBlob = nullptr;
	//ID3DBlob* errorBlob = nullptr;
	//hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	//if (FAILED(hr)) {
	//	Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	//	assert(false);
	//}
	////バイナリを元に生成
	//Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
	//	signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	//assert(SUCCEEDED(hr));
	////InputLayout
	//D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	//inputElementDescs[0].SemanticName = "POSITION";
	//inputElementDescs[0].SemanticIndex = 0;
	//inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//inputElementDescs[1].SemanticName = "TEXCOORD";
	//inputElementDescs[1].SemanticIndex = 0;
	//inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	//inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//inputElementDescs[2].SemanticName = "NORMAL";
	//inputElementDescs[2].SemanticIndex = 0;
	//inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	//inputLayoutDesc.pInputElementDescs = inputElementDescs;
	//inputLayoutDesc.NumElements = _countof(inputElementDescs);
	////BlendStateの設定
	//D3D12_BLEND_DESC blendDesc{};

	//enum BlendMode {
	//	//ブレンド無し
	//	kBlendModeNone,
	//	//通常のブレンド
	//	kBlendModeNormal,
	//	//加算合成
	//	kBlendModeAdd,
	//	//減算合成
	//	kBlendModeSubtract,
	//	//乗算合成
	//	kBlendModeMultily,
	//	//スクリーン合成
	//	kBlendModeScreen
	//};

	//BlendMode blendMode_ = kBlendModeNone;

	////全ての色要素を書き込む
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//switch (blendMode_)
	//{
	//case kBlendModeNone:
	//	blendDesc.RenderTarget[0].BlendEnable = false;
	//	break;
	//case kBlendModeNormal:
	//	blendDesc.RenderTarget[0].BlendEnable = true;
	//	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//	
	//	break;
	//case kBlendModeAdd:
	//	blendDesc.RenderTarget[0].BlendEnable = true;
	//	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	//	break;
	//case kBlendModeSubtract:
	//	blendDesc.RenderTarget[0].BlendEnable = true;
	//	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	//	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	//	break;
	//case kBlendModeMultily:
	//	blendDesc.RenderTarget[0].BlendEnable = true;
	//	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	//	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	//	break;
	//case kBlendModeScreen:
	//	blendDesc.RenderTarget[0].BlendEnable = true;
	//	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	//	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	//	break;
	//default:
	//	break;
	//}
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	////RasiterzerStateの設定
	//D3D12_RASTERIZER_DESC rasterrizerDesc{};
	////裏面(時計回り)を表示しない
	//rasterrizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	////三角形の中を塗りつぶす
	//rasterrizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	////Shaderをコンパイルする
	//IDxcBlob* vertexShaderBlob = CompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	//assert(vertexShaderBlob != nullptr);

	//IDxcBlob* pixelShaderBlob = CompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	//assert(pixelShaderBlob != nullptr);

	////DepthStencilState設定
	//D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	////Depthの機能を有効化する
	//depthStencilDesc.DepthEnable = true;
	////書き込みします
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	////比較関数はLessEqual。つまり、近ければ描画される
	//depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//
	////PSOを生成する
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	//graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	//graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	//	vertexShaderBlob->GetBufferSize() };

	//graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	//	pixelShaderBlob->GetBufferSize() };
	//graphicsPipelineStateDesc.BlendState = blendDesc;
	//graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc;

	//graphicsPipelineStateDesc.NumRenderTargets = 1;
	//graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//graphicsPipelineStateDesc.PrimitiveTopologyType =
	//	D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//graphicsPipelineStateDesc.SampleDesc.Count = 1;
	//graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	////DepthStencilの設定
	//graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	//graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	//hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	//assert(SUCCEEDED(hr));

	////const uint32_t kSubdivision = 16;
	////const float kLatEvery = float(M_PI / kSubdivision);//緯度一つ分の角度
	////const float kLonEvery = float((M_PI * 2.0f) / kSubdivision);//経度一つ分の角度
	//
	////頂点リソースの作成
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * modelData.vertices.size());

	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	////リソースの先頭のアドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点三つ分のサイズ
	//vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	////1頂点当たりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	////頂点リソースにデータを書き込む
	//VertexData* vertexDate = nullptr;
	////書き込むためのアドレスを取得
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate));
	//std::memcpy(vertexDate, modelData.vertices.data(), sizeof(VertexData)* modelData.vertices.size());
	//

	////マテリアル用のリソース
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
	////マテリアルにデータを書き込む
	//Material* materialDate = nullptr;

	////書き込むためのアドレスを取得
	//materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	////今回は赤を書き込んでみる
	//materialDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	//materialDate->enableLighting = false;

	//materialDate->uvTransform = MakeIdentity4x4();

	////wvp用のリソースを作る。TransformationMatrix一つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(TransformationMatrix));
	////データを書き込む
	//TransformationMatrix* wvpData = nullptr;
	////書き込むためのアドレスを取得
	//wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	////単位行列を書き込んでおく
	//wvpData->WVP = MakeIdentity4x4();
	//wvpData->World = MakeIdentity4x4();

	///*ここから2D描画関連のあれこれを設定している*/
	////マテリアル用のリソース
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
	////マテリアルにデータを書き込む
	//Material* materialDateSprite = nullptr;

	////書き込むためのアドレスを取得
	//materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDateSprite));
	////今回は白を書き込んでみる
	//materialDateSprite->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	////SpriteはLightingしないのでfalseを設定しておく
	//materialDateSprite->enableLighting = false;

	//materialDateSprite->uvTransform = MakeIdentity4x4();

	///*平行光源用リソース関連*/
	////マテリアル用のリソース
	//Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionalLight));
	////マテリアルにデータを書き込む
	//DirectionalLight* directionalLightDate = nullptr;

	////書き込むためのアドレスを取得
	//directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDate));
	////今回は白を書き込んでみる
	//directionalLightDate->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	//directionalLightDate->direction = { 0.0f,1.0f,0.0f };

	//directionalLightDate->intensity = 1.0f;

	////Sprite用の頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 4);
	////頂点バッファービューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	////リソースの先頭アドレスから使う
	//vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点6つ分のサイズ
	//vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	////1頂点当たりのサイズ
	//vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	//VertexData* vertexDataSprite = nullptr;
	//vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	////1枚目の三角形
	//vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	//vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	//vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	//vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	//vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	//vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	//vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	//vertexDataSprite[3].texcoord = { 1.0f,0.0f };

	//Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * 6);

	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	////リソースの先頭のアドレスから使用する
	//indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	////使用するリソースのサイズはインデックス六つ分のサイズ
	//indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	////インデックスはuint32_tとする
	//indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
	////インデックスリソースにデータを書き込む
	//uint32_t* indexDataSprite = nullptr;
	//indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	//indexDataSprite[0] = 0;	indexDataSprite[1] = 1;	indexDataSprite[2] = 2;
	//indexDataSprite[3] = 1;	indexDataSprite[4] = 3;	indexDataSprite[5] = 2;

	////Sprite用のTransformationMatrix用リソースを作る。TransformationMatrix 一つ分のサイズを用意する
	//Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite = CreateBufferResource(dxCommon_->GetDevice(), sizeof(TransformationMatrix));
	////データを書き込む
	//TransformationMatrix* transformationMatrixDataSprite = nullptr;
	////書き込むためのアドレスを取得
	//transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	////単位行列を書き込んでいく
	//transformationMatrixDataSprite->WVP = MakeIdentity4x4();

	//Transform transform{ {1.0f,1.0f,1.0f},{0.0f,3.14f,0.0f}, {0.0f,0.0f,0.0f} };

	//Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{640.0f,320.0f,0.0f} };

	//Transform CameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	//Transform uvTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	//Transform uvTransformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	////ビューポート
	//D3D12_VIEWPORT viewport{};
	////クライアント領域のサイズと一緒にして画面全体に表示
	//viewport.Width = static_cast<float>(kWindowWidth);
	//viewport.Height = static_cast<float>(kWindowHeight);
	//viewport.TopLeftX = 0;
	//viewport.TopLeftY = 0;
	//viewport.MinDepth = 0.0f;
	//viewport.MaxDepth = 1.0f;

	////シザー矩形
	//D3D12_RECT scissorRect{};
	////基本的にビューポートと同じ矩形が構成されるようにする
	//scissorRect.left = 0;
	//scissorRect.right = kWindowWidth;
	//scissorRect.top = 0;
	//scissorRect.bottom = kWindowHeight;

	////スワップチェーンを生成
	//Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	//DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//swapChainDesc.Width = kWindowWidth;
	//swapChainDesc.Height = kWindowHeight;
	//swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//swapChainDesc.SampleDesc.Count = 1;
	//swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//swapChainDesc.BufferCount = 2;
	//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), window_->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	//assert(SUCCEEDED(hr));

	//ディスクリプタヒープの生成
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = CreateDescriptorHeap(dxCommon_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	//DescriptorSizeを取得しておく
	const uint32_t descriptorSizeSRV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//SwapChainからResourceを引っ張ってくる
	//Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
	//hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	//assert(SUCCEEDED(hr));

	//hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	//assert(SUCCEEDED(hr));

	////RTVの設定
	//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	////ディスクリプタの先頭を取得する
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	////RTVを2つ作るのでディスクリプタを二つ用意
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	////一つ目を作る
	//rtvHandles[0] = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, 0);
	//device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	////二つ目のディスクリプタハンドルを得る
	//rtvHandles[1] = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, 1);
	////
	//device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);


	//ImGuiの初期化
	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window_->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice(),
		dxCommon_->GetSwapChainDesc().BufferCount,
		dxCommon_->GetRTVDesc().Format,
		dxCommon_->GetSRVHeap(),
		dxCommon_->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxCommon_->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart());*/

	MSG msg{};
	/*ShaderResourceViewの生成*/
	////mateDataを基にSRVの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc[2]{};
	//srvDesc[0].Format = metadata.format;
	//srvDesc[0].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc[0].ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc[0].Texture2D.MipLevels = UINT(metadata.mipLevels);
	//srvDesc[1].Format = metadata2.format;
	//srvDesc[1].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc[1].ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc[1].Texture2D.MipLevels = UINT(metadata2.mipLevels);
	//

	////SRVを作成するDescripterHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU[2]{};
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU[2]{};

	//textureSrvHandleCPU[0] = GetCPUDescriptorHandle(dxCommon_->GetSRVHeap(), descriptorSizeSRV, 1);
	//textureSrvHandleGPU[0] = GetGPUDescriptorHandle(dxCommon_->GetSRVHeap(), descriptorSizeSRV, 1);

	//textureSrvHandleCPU[1] = GetCPUDescriptorHandle(dxCommon_->GetSRVHeap(), descriptorSizeSRV, 2);
	//textureSrvHandleGPU[1] = GetGPUDescriptorHandle(dxCommon_->GetSRVHeap(), descriptorSizeSRV, 2);

	////SRVの生成
	//dxCommon_->GetDevice()->CreateShaderResourceView(textureResource[0].Get(), &srvDesc[0], textureSrvHandleCPU[0]);
	//dxCommon_->GetDevice()->CreateShaderResourceView(textureResource[1].Get(), &srvDesc[1], textureSrvHandleCPU[1]);

	////DSVの設定
	//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResourceに合わせる
	//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
	////DSVHeapの先頭にDSVを作る
	//device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	bool useMonsterBall = true;

	bool DrawTexture = false;

	/*bool DrawSphere = false;*/

	bool isMoveRotate = false;

	bool useDirectionLight = false;

	float rotateSpeed = 0.06f;

	int selectNumber = 0;
	
	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (window_->ProcessMessage()) {
			break;
		}
		
		dxCommon_->StartImgui();
		//ゲームの処理

		input_->Update();

		sprite_->Update();
		//if (input_->TrigerRight()) {
		//	transform.translate.x += 1.0f;
		//}
		//if (input_->TrigerLeft()) {
		//	transform.translate.x -= 1.0f;
		//}

		//ImGui::ShowDemoWindow();
		//ImGui::Begin("FPS");
		//ImGui::Text("Frame rate: %6.2f fps", ImGui::GetIO().Framerate);
		//ImGui::End();
		//ImGui::ColorEdit4("Color", &materialDate->color.x);
		//ImGui::DragFloat3("Transform", &transform.translate.x, 0.01f);
		//ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.01f);
		//ImGui::SliderFloat("RotateSpeed", &rotateSpeed, -0.1f, 0.1f);
		//ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f);
		//ImGui::DragFloat2("UVTransform", &uvTransform.translate.x, 0.01f, -10.0f, 10.0f);
		//ImGui::DragFloat2("UVScale", &uvTransform.scale.x, 0.01f, -10.0f, 10.0f);
		//ImGui::SliderAngle("UVRotate", &uvTransform.rotate.z);
		//	
		//ImGui::DragFloat3("CameraTransform", & CameraTransform.translate.x, 0.01f);
		//ImGui::DragFloat3("CameraRotate", &CameraTransform.rotate.x, 0.01f);
		//if (useDirectionLight) {
		//	ImGui::ColorEdit4("DirectionalLight.Color", &directionalLightDate->color.x);
		//	ImGui::DragFloat3("DirectionalLight.Direction", &directionalLightDate->direction.x, 0.01f);
		//	ImGui::DragFloat("DirectionalLight.intensity", &directionalLightDate->intensity, 0.01f);
		//}
		//ImGui::Checkbox("useDirectionLight", &useDirectionLight);
		//ImGui::Checkbox("useMonsterBall", &useMonsterBall);
		//ImGui::Checkbox("DrawTexture", &DrawTexture);
		//ImGui::Checkbox("isRotate", &isMoveRotate);
		//if (DrawTexture) {
		//	ImGui::DragFloat2("TransformSprite", &transformSprite.translate.x, 1.0f);
		//	ImGui::DragFloat3("RotateSprite", &transformSprite.rotate.x, 0.01f);
		//	ImGui::DragFloat2("ScaleSprite", &transformSprite.scale.x, 0.01f);

		//	ImGui::DragFloat2("UVTransform", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		//	ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		//	ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
		//}
		//	
		//if (materialDate->color.x < 0.0f) {
		//	materialDate->color.x = 0.0f;
		//}
		//else if (materialDate->color.y < 0.0f) {
		//	materialDate->color.y = 0.0f;
		//}
		//else if (materialDate->color.z < 0.0f) {
		//	materialDate->color.z = 0.0f;
		//}

		//if (materialDate->color.x > 1.0f) {
		//	materialDate->color.x = 1.0f;
		//}
		//else if (materialDate->color.y > 1.0f) {
		//	materialDate->color.y = 1.0f;
		//}
		//else if (materialDate->color.z > 1.0f) {
		//	materialDate->color.z = 1.0f;
		//}

		//if (directionalLightDate->color.x < 0.0f) {
		//	directionalLightDate->color.x = 0.0f;
		//}
		//else if (directionalLightDate->color.y < 0.0f) {
		//	directionalLightDate->color.y = 0.0f;
		//}
		//else if (directionalLightDate->color.z < 0.0f) {
		//	directionalLightDate->color.z = 0.0f;
		//}

		//if (directionalLightDate->color.x > 1.0f) {
		//	directionalLightDate->color.x = 1.0f;
		//}
		//else if (directionalLightDate->color.y > 1.0f) {
		//	directionalLightDate->color.y = 1.0f;
		//}
		//else if (directionalLightDate->color.z > 1.0f) {
		//	directionalLightDate->color.z = 1.0f;
		//}
		//if (useDirectionLight){
		//	materialDate->enableLighting = true;
		//}
		//else {
		//	materialDate->enableLighting = false;
		//}

		////三角形のWorldMatrix
		//if (isMoveRotate){
		//	transform.rotate.y += rotateSpeed;
		//}

		//directionalLightDate->direction = matrix->Normalize(directionalLightDate->direction);

		//Matrix4x4 worldMatrix = matrix->MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		//Matrix4x4 cameraMatrix = matrix->MakeAffineMatrix(CameraTransform.scale,CameraTransform.rotate,CameraTransform.translate);
		//Matrix4x4 viewMatrix = matrix->Inverce(cameraMatrix);
		//Matrix4x4 projectionMatrix = matrix->MakePerspectiveFovMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
		//Matrix4x4 worldViewProjectionMatrix = matrix->Multiply(worldMatrix, matrix->Multiply(viewMatrix, projectionMatrix));
		//wvpData->WVP = worldViewProjectionMatrix;
		//wvpData->World = worldMatrix;

		//Matrix4x4 uvTransformMatrix = matrix->MakeScaleMatrix(uvTransform.scale);
		//uvTransformMatrix = matrix->Multiply(uvTransformMatrix, matrix->MakeRotateMatrixZ(uvTransform.rotate));
		//uvTransformMatrix = matrix->Multiply(uvTransformMatrix, matrix->MakeTranslateMatrix(uvTransform.translate));
		//materialDate->uvTransform = uvTransformMatrix;

		////Sprite用のWorldViewProjectMatrixを作る
		//Matrix4x4 worldMatrixSprite = matrix->MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		//Matrix4x4 cameraMatrixSprite = matrix->MakeAffineMatrix(CameraTransform.scale, CameraTransform.rotate, CameraTransform.translate);
		//Matrix4x4 viewMatrixSprite = matrix->Inverce(cameraMatrixSprite);
		//Matrix4x4 projectionMatrixSprite = matrix->MakeOrthographicMatrix(0.0f, 0.0f, float(window_->kWindowWidth), float(window_->kWindowHeight), 0.0f, 100.0f);
		//Matrix4x4 worldViewProjectionMatrixSprite = matrix->Multiply(worldMatrixSprite, matrix->Multiply(viewMatrixSprite, projectionMatrixSprite));
		//transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
		//transformationMatrixDataSprite->World = worldMatrixSprite;

		//Matrix4x4 uvTransformMatrixSprite = matrix->MakeScaleMatrix(uvTransformSprite.scale);
		//uvTransformMatrixSprite = matrix->Multiply(uvTransformMatrixSprite, matrix->MakeRotateMatrixZ(uvTransformSprite.rotate));
		//uvTransformMatrixSprite = matrix->Multiply(uvTransformMatrixSprite, matrix->MakeTranslateMatrix(uvTransformSprite.translate));
		//materialDateSprite->uvTransform = uvTransformMatrixSprite;
		dxCommon_->EndImgui();

		dxCommon_->PreDraw();


		sprite_->Draw(dxCommon_->GetCommandList());
		////RootSignatureを設定。PSOに設定しているが別途設定が必要
		//dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
		//dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
		//dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
		////形状を設定。
		//dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
		//dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU[1] : textureSrvHandleGPU[0]);
		//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
		//////描画
		////dxCommon_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		////3D三角の描画
		////dxCommon_->GetCommandList()->DrawInstanced(DrawSphere ? 1542 : 0, 1, 0, 0);
		//dxCommon_->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
		////2DのSRVの設定
		//dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU[0]);
		////2Dの描画
		////マテリアルにCBufferの場所を設定
		//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());			 
		////Spriteの描画。変更が必要なものだけ変更する
		//dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
		//dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
		////TransformationMatrixCBufferの場所を設定
		//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
		////描画
		//if (DrawTexture)
		//{
		//	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
		//}
			
		//実際ののCommandListのImGuiの描画コマンドを積む
		//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());

		dxCommon_->PostDraw();

		if (input_->Trigerkey(DIK_ESCAPE)){
			break;
		}	

	}
	CoUninitialize();

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//CloseHandle(fenceEvent);
	//fence->Release();
	/*rtvDescriptorHeap->Release();
	srvDescriptorHeap->Release();
	dsvDescriptorHeap->Release();*/
//	swapChainResources[0]->Release();
	//swapChainResources[1]->Release();
	//swapChain->Release();
	//commandList->Release();
	//commandAllocator->Release();
	//commandQueue->Release();
	//device->Release();
	//useAdapter->Release();
	//dxgiFactory->Release();
	/*vertexResource->Release();
	vertexResource.Reset();
	vertexResourceSprite->Release();
	vertexResourceSprite.Reset();
	indexResourceSprite->Release();
	indexResourceSprite.Reset();*/

	//graphicsPipelineState->Release();
	//signatureBlob->Release();
	//if (errorBlob) {
	//	errorBlob->Release();
	//}
	////rootSignature->Release();
	//pixelShaderBlob->Release();
	//vertexShaderBlob->Release();

	/*materialResource->Release();
	materialResource.Reset();
	materialResourceSprite->Release();
	materialResourceSprite.Reset();
	directionalLightResource->Release();
	directionalLightResource.Reset();
	wvpResource->Release();
	wvpResource.Reset();
	transformationMatrixResourceSprite->Release();
	transformationMatrixResourceSprite.Reset();
	intermediateResource[0]->Release();
	intermediateResource[0].Reset();
	intermediateResource[1]->Release();
	intermediateResource[1].Reset();

	depthStencilResource->Release();
	depthStencilResource.Reset();
	textureResource[0]->Release();
	textureResource[0].Reset();
	textureResource[1]->Release();
	textureResource[1].Reset();*/

	delete matrix;

#ifdef _DEBUG
	//debugController->Release();
#endif // _DEBUG
	//CloseWindow(window_->GetHwnd());

	window_->Finalize();


	return 0;
}
//球の
////緯度の方向に分割-π/2～π/2
//for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
//	float lat = float(-M_PI) / 2.0f + (kLatEvery * latIndex);//現在の緯度(θ)
//	//経度の方向に分割 0～2π
//	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
//		uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
//		float lon = lonIndex * kLonEvery;//現在の経度(φ)

//		//頂点にデータを入力していく。基準点a
//		vertexDate[start].position.x = cos(lat) * cos(lon);
//		vertexDate[start].position.y = sin(lat);
//		vertexDate[start].position.z = cos(lat) * sin(lon);
//		vertexDate[start].position.w = 1.0f;
//		vertexDate[start].texcoord = 
//		{ float(lonIndex) / float(kSubdivision) ,
//			1.0f - float(latIndex) / float(kSubdivision) };
//		vertexDate[start].normal.x = vertexDate[start + 6].position.x;
//		vertexDate[start].normal.y = vertexDate[start + 6].position.y;
//		vertexDate[start].normal.z = vertexDate[start + 6].position.z;

//		//基準点b
//		vertexDate[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
//		vertexDate[start + 1].position.y = sin(lat + kLatEvery);
//		vertexDate[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
//		vertexDate[start + 1].position.w = 1.0f;
//		vertexDate[start + 1].texcoord =
//		{ float(lonIndex) / float(kSubdivision) ,
//			1.0f - float(latIndex+1) / float(kSubdivision) };
//		//基準点c
//		vertexDate[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
//		vertexDate[start + 2].position.y = sin(lat);
//		vertexDate[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
//		vertexDate[start + 2].position.w = 1.0f;
//		vertexDate[start + 2].texcoord =
//		{ float(lonIndex+1) / float(kSubdivision) ,
//			1.0f - float(latIndex) / float(kSubdivision) };
//		//基準点c
//		vertexDate[start + 3].position.x = cos(lat) * cos(lon + kLonEvery);
//		vertexDate[start + 3].position.y = sin(lat);
//		vertexDate[start + 3].position.z = cos(lat) * sin(lon + kLonEvery);
//		vertexDate[start + 3].position.w = 1.0f;
//		vertexDate[start + 3].texcoord =
//		{ float(lonIndex+1) / float(kSubdivision) ,
//			1.0f - float(latIndex) / float(kSubdivision) };
//		//基準点b
//		vertexDate[start + 4].position.x = cos(lat + kLatEvery) * cos(lon);
//		vertexDate[start + 4].position.y = sin(lat + kLatEvery);
//		vertexDate[start + 4].position.z = cos(lat + kLatEvery) * sin(lon);
//		vertexDate[start + 4].position.w = 1.0f;
//		vertexDate[start + 4].texcoord =
//		{ float(lonIndex) / float(kSubdivision) ,
//			1.0f - float(latIndex+1) / float(kSubdivision) };
//		//基準点d
//		vertexDate[start + 5].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
//		vertexDate[start + 5].position.y = sin(lat + kLatEvery);
//		vertexDate[start + 5].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
//		vertexDate[start + 5].position.w = 1.0f;
//		vertexDate[start + 5].texcoord =
//		{ float(lonIndex+1) / float(kSubdivision) ,
//			1.0f - float(latIndex+1) / float(kSubdivision) };
//	}
//}
