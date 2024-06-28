#pragma once
#include"Log.h"
#include<array>
#include<wrl.h>


class ComputeGraphicsPipeline{
public:
	~ComputeGraphicsPipeline();
	//通常の初期化
	void Initialize(const std::wstring& VSname, const std::wstring& PSname, bool isCulling);

	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

	ID3D12PipelineState* GetPipeLineState() {
		return graphicsPipelineState_.Get();
	}

	ID3D12RootSignature* GetRootSignature() {
		return computeRootSignature_.Get();
	}


private:
	//メンバ関数
	void makeGraphicsPipeline(ID3D12Device* device);

	void makeRootSignature(ID3D12Device* device);

	void makeInputLayout();

	void makeRasterizerState(bool isCulling);

	void ShaderCompile(const std::wstring& VSname, const std::wstring& PSname);

	void makeDepthStencil(D3D12_DEPTH_WRITE_MASK depthWriteMask);

private:
	HRESULT hr{};

	ID3D12Device* device_ = nullptr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};

	Microsoft::WRL::ComPtr<ID3D12RootSignature> computeRootSignature_ = nullptr;

	ID3DBlob* signatureBlob_ = nullptr;
	ID3DBlob* errorBlob_ = nullptr;

	std::array<D3D12_INPUT_ELEMENT_DESC, 1> inputElementSkyDescs_{};

	std::array<D3D12_INPUT_ELEMENT_DESC, 3> inputElementDescs_{};

	std::array<D3D12_INPUT_ELEMENT_DESC, 5> inputElementSkinDescs_{};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};

	//RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterrizerDesc_{};

	IDxcBlob* computeShaderBlob_ = nullptr;

	//DepthStencilState設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

};

