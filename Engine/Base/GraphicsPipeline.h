#pragma once
#include"Log.h"
#include<wrl.h>

enum BlendMode {
	//ブレンド無し
	kBlendModeNone,
	//通常のブレンド
	kBlendModeNormal,
	//加算合成
	kBlendModeAdd,
	//減算合成
	kBlendModeSubtract,
	//乗算合成
	kBlendModeMultily,
	//スクリーン合成
	kBlendModeScreen
};

class GraphicsPipeline{
public:
	~GraphicsPipeline();

	static GraphicsPipeline* GetInstance();

	void Initialize(const std::wstring& VSname, const std::wstring& PSname, bool isCulling);
	//パーティクル専用の初期化処理
	void ParticleExclusiveInitialize(const std::wstring& VSname, const std::wstring& PSname, bool isCulling, const BlendMode& blend);

	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

	ID3D12PipelineState* GetPipeLineState() {
		return graphicsPipelineState.Get();
	}

	ID3D12RootSignature* GetRootSignature() {
		return rootSignature.Get();
	}

	ID3D12RootSignature* GetParticleRootSignature() {
		return rootSignatureParticle.Get();
	}

private:
	//メンバ関数
	void makeGraphicsPipeline(ID3D12Device* device);

	void makeGraphicsPipelineParticle(ID3D12Device* device);

	void makeRootSignature(ID3D12Device* device);

	void makeParticleRootSignature(ID3D12Device* device);

	void makeInputLayout();

	void makeBlendState(const BlendMode& blend);

	void makeRasterizerState(bool isCulling);

	void ShaderCompile(const std::wstring& VSname, const std::wstring& PSname);

	void makeDepthStencil(D3D12_DEPTH_WRITE_MASK depthWriteMask);


private:
	HRESULT hr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignatureParticle{};

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureParticle = nullptr;

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};

	

	//RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterrizerDesc{};

	IDxcBlob* vertexShaderBlob = nullptr;

	IDxcBlob* pixelShaderBlob = nullptr;

	//DepthStencilState設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

};

