#pragma once
#include"Log.h"
#include<array>
#include<wrl.h>
/*パイプラインの設定、生成を行う*/

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
	//通常の初期化
	void Initialize(const std::wstring& VSname, const std::wstring& PSname, bool isCulling);

	//通常の初期化
	void Initialize2D(const std::wstring & VSname, const std::wstring & PSname);

	//3d空間に配置するスプライトのパイプライン
	void InitializeWorld2D(const std::wstring& VSname, const std::wstring& PSname);

	//パーティクル専用の初期化処理
	void ParticleExclusiveInitialize(const std::wstring& VSname, const std::wstring& PSname, bool isCulling, const BlendMode& blend);
	//コピー用の初期化
	void InitializeCopy(const std::wstring& VSname, const std::wstring& PSname);

	void InitializeSkyBox(const std::wstring& VSname, const std::wstring& PSname);

	//Skinning用の初期化処理
	void InitializeSkinning(const std::wstring& VSname, const std::wstring& PSname, bool isCulling);

	//トレイル用の初期化処理
	void InitializeTrail(const std::wstring& VSname, const std::wstring& PSname);

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
	//種類や用途が違うものを関数で分けています

	void makeGraphicsPipeline(ID3D12Device* device);

	void makeGraphicsPipelineParticle(ID3D12Device* device);

	void makeGraphicsPipelineCopy(ID3D12Device* device);

	void makeRootSignature(ID3D12Device* device);

	void makeRootSignatureSkyBox(ID3D12Device* device);

	void makeParticleRootSignature(ID3D12Device* device);

	void makeRootSignatureCopy(ID3D12Device* device);

	void makeRootSignatureSkinning(ID3D12Device* device);

	void makeRootSignatureTrail(ID3D12Device* device);

	void makeInputLayout();

	void makeInputLayoutSkyBox();

	void makeInputLayoutCopy();

	void makeInputLayoutSkinning();

	void makeBlendState(const BlendMode& blend);

	void makeRasterizerState(bool isCulling);

	void ShaderCompile(const std::wstring& VSname, const std::wstring& PSname);

	void makeDepthStencil(D3D12_DEPTH_WRITE_MASK depthWriteMask);

	void makeDepthStencilCopy();

	void makeDepthStencilSkyBox();


private:
	HRESULT hr{};

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignatureParticle{};

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureParticle = nullptr;

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	std::array<D3D12_INPUT_ELEMENT_DESC, 1> inputElementSkyDescs_{};

	std::array<D3D12_INPUT_ELEMENT_DESC, 3> inputElementDescs_{};

	std::array<D3D12_INPUT_ELEMENT_DESC, 5> inputElementSkinDescs_{};

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

