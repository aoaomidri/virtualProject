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
	static GraphicsPipeline* GetInstance();

	void Initialize(ID3D12Device* device, const std::wstring& VSname, const std::wstring& PSname);

	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

	ID3D12PipelineState* GetPipeLineState() {
		return graphicsPipelineState.Get();
	}

	ID3D12RootSignature* GetRootSignature() {
		return rootSignature.Get();
	}

private:
	//メンバ関数
	void makeGraphicsPipeline(ID3D12Device* device);

	void makeRootSignature(ID3D12Device* device);

	void makeInputLayout();

	void makeBlendState();

	void makeRasterizerState();

	void ShaderCompile(const std::wstring& VSname, const std::wstring& PSname);

	void makeDepthStencil();


private:
	HRESULT hr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};

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

