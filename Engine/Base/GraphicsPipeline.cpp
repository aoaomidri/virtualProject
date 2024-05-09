#include "GraphicsPipeline.h"
#include"DirectXCommon.h"
#include <cassert>
#pragma warning(disable : 6387)

GraphicsPipeline::~GraphicsPipeline(){
	
}

GraphicsPipeline* GraphicsPipeline::GetInstance() {
	static GraphicsPipeline Instance;
	return &Instance;
}

void GraphicsPipeline::Initialize(const std::wstring& VSname, const std::wstring& PSname, bool isCulling){

	makeRootSignature(DirectXCommon::GetInstance()->GetDevice());
	makeInputLayout();
	makeRasterizerState(isCulling);
	makeBlendState(kBlendModeNormal);
	ShaderCompile(VSname, PSname);
	makeDepthStencil(D3D12_DEPTH_WRITE_MASK_ALL);	

	makeGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());
}

void GraphicsPipeline::ParticleExclusiveInitialize(const std::wstring& VSname, const std::wstring& PSname, bool isCulling, const BlendMode& blend){
	makeParticleRootSignature(DirectXCommon::GetInstance()->GetDevice());
	makeInputLayout();
	makeBlendState(blend);
	makeRasterizerState(isCulling);
	ShaderCompile(VSname, PSname);
	makeDepthStencil(D3D12_DEPTH_WRITE_MASK_ZERO);

	makeGraphicsPipelineParticle(DirectXCommon::GetInstance()->GetDevice());
}

void GraphicsPipeline::InitializeCopy(const std::wstring& VSname, const std::wstring& PSname){
	makeRootSignatureCopy(DirectXCommon::GetInstance()->GetDevice());
	makeInputLayoutCopy();
	makeRasterizerState(false);
	makeBlendState(kBlendModeNone);
	ShaderCompile(VSname, PSname);
	makeDepthStencilCopy();

	makeGraphicsPipelineCopy(DirectXCommon::GetInstance()->GetDevice());
}

void GraphicsPipeline::InitializeSkinning(const std::wstring& VSname, const std::wstring& PSname, bool isCulling) {
	makeRootSignatureSkinning(DirectXCommon::GetInstance()->GetDevice());
	makeInputLayoutSkinning();
	makeRasterizerState(isCulling);
	makeBlendState(kBlendModeNormal);
	ShaderCompile(VSname, PSname);
	makeDepthStencil(D3D12_DEPTH_WRITE_MASK_ALL);

	makeGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());
}


void GraphicsPipeline::makeRootSignature(ID3D12Device* device){
	//RootSignature作成
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameter[6] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].Descriptor.ShaderRegister = 1;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[1].Descriptor.ShaderRegister = 0;

	descriptionRootSignature.pParameters = rootParameter;
	descriptionRootSignature.NumParameters = _countof(rootParameter);

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[3].Descriptor.ShaderRegister = 2;

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[4].Descriptor.ShaderRegister = 3;

	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[5].Descriptor.ShaderRegister = 4;

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0～1の範囲外をリピート
	staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSampler[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う

	descriptionRootSignature.pStaticSamplers = staticSampler;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);

	//シリアライズしてバイナリする
	
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	

}

void GraphicsPipeline::makeParticleRootSignature(ID3D12Device* device){
	//RootSignature作成
	descriptionRootSignatureParticle.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].Descriptor.ShaderRegister = 1;

	descriptionRootSignatureParticle.pParameters = rootParameter;
	descriptionRootSignatureParticle.NumParameters = _countof(rootParameter);

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[1].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameter[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[3].Descriptor.ShaderRegister = 2;

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0～1の範囲外をリピート
	staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSampler[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う

	descriptionRootSignatureParticle.pStaticSamplers = staticSampler;
	descriptionRootSignatureParticle.NumStaticSamplers = _countof(staticSampler);

	//シリアライズしてバイナリする

	hr = D3D12SerializeRootSignature(&descriptionRootSignatureParticle, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成

	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureParticle));
	assert(SUCCEEDED(hr));
}

void GraphicsPipeline::makeRootSignatureCopy(ID3D12Device* device){
	//RootSignature作成
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameter[1] = {};

	descriptionRootSignature.pParameters = rootParameter;
	descriptionRootSignature.NumParameters = _countof(rootParameter);

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0～1の範囲外をリピート
	staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSampler[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う

	descriptionRootSignature.pStaticSamplers = staticSampler;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);

	//シリアライズしてバイナリする

	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成

	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
}

void GraphicsPipeline::makeRootSignatureSkinning(ID3D12Device* device){
	//RootSignature作成
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRangeの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameter[7] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].Descriptor.ShaderRegister = 1;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[1].Descriptor.ShaderRegister = 0;

	descriptionRootSignature.pParameters = rootParameter;
	descriptionRootSignature.NumParameters = _countof(rootParameter);

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[3].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameter[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[4].Descriptor.ShaderRegister = 2;

	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[5].Descriptor.ShaderRegister = 3;

	rootParameter[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[6].Descriptor.ShaderRegister = 4;

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0～1の範囲外をリピート
	staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSampler[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う

	descriptionRootSignature.pStaticSamplers = staticSampler;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);

	//シリアライズしてバイナリする

	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成

	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
}

void GraphicsPipeline::makeInputLayout(){
	//InputLayout
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	
	inputLayoutDesc.pInputElementDescs = inputElementDescs_.data();
	inputLayoutDesc.NumElements = static_cast<UINT>(inputElementDescs_.size());
}

void GraphicsPipeline::makeInputLayoutCopy(){
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;
}

void GraphicsPipeline::makeInputLayoutSkinning(){
	//InputLayout
	inputElementSkinDescs_[0].SemanticName = "POSITION";
	inputElementSkinDescs_[0].SemanticIndex = 0;
	inputElementSkinDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementSkinDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementSkinDescs_[1].SemanticName = "TEXCOORD";
	inputElementSkinDescs_[1].SemanticIndex = 0;
	inputElementSkinDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementSkinDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementSkinDescs_[2].SemanticName = "NORMAL";
	inputElementSkinDescs_[2].SemanticIndex = 0;
	inputElementSkinDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementSkinDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementSkinDescs_[3].SemanticName = "WEIGHT";
	inputElementSkinDescs_[3].SemanticIndex = 0;
	inputElementSkinDescs_[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementSkinDescs_[3].InputSlot = 1;
	inputElementSkinDescs_[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementSkinDescs_[4].SemanticName = "INDEX";
	inputElementSkinDescs_[4].SemanticIndex = 0;
	inputElementSkinDescs_[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	inputElementSkinDescs_[4].InputSlot = 1;
	inputElementSkinDescs_[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc.pInputElementDescs = inputElementSkinDescs_.data();
	inputLayoutDesc.NumElements = static_cast<UINT>(inputElementSkinDescs_.size());
}

void GraphicsPipeline::makeBlendState(const BlendMode& blend){

	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	switch (blend)
	{
	case kBlendModeNone:
		blendDesc.RenderTarget[0].BlendEnable = false;
		break;
	case kBlendModeNormal:
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		
		break;
	case kBlendModeAdd:
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeSubtract:
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	case kBlendModeMultily:
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		break;
	case kBlendModeScreen:
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;
	default:
		break;
	}
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

void GraphicsPipeline::makeRasterizerState(bool isCulling){
	
	//裏面(時計回り)を表示しない
	if (isCulling){
		rasterrizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	}
	else{
		rasterrizerDesc.CullMode =D3D12_CULL_MODE_NONE;
	}
	
	//三角形の中を塗りつぶす
	rasterrizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void GraphicsPipeline::ShaderCompile(const std::wstring& VSname, const std::wstring& PSname){
	//dxcCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	//Shaderをコンパイルする
	vertexShaderBlob = CompileShader(VSname, L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	pixelShaderBlob = CompileShader(PSname, L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);
}

void GraphicsPipeline::makeDepthStencil(D3D12_DEPTH_WRITE_MASK depthWriteMask){
	
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = depthWriteMask/*D3D12_DEPTH_WRITE_MASK_ZERO*/;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

}

void GraphicsPipeline::makeDepthStencilCopy(){
	depthStencilDesc.DepthEnable = false;
}

void GraphicsPipeline::makeGraphicsPipeline(ID3D12Device* device) {
	//PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };

	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}

void GraphicsPipeline::makeGraphicsPipelineParticle(ID3D12Device* device){
	//PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureParticle.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };

	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}

void GraphicsPipeline::makeGraphicsPipelineCopy(ID3D12Device* device){
	//PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };

	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterrizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}




IDxcBlob* GraphicsPipeline::CompileShader(
	const std::wstring& filePath, const wchar_t* profile, 
	IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, 
	IDxcIncludeHandler* includeHandler)
{
	////////1, hlslファイルを読む
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
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
