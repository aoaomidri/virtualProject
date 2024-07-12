#pragma once
#include"TrailEffect.h"
class TrailRender{
public:
	//初期化処理
	void Initialize();

	void Draw();

	void MakeResource();
private:
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineTrail_;


	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;

	//データを書き込む
	TransformationMatrix* wvpData_ = nullptr;

	HRESULT hr_;

	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//頂点リソースにデータを書き込む
	VertexData* vertexDate_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルにデータを書き込む
	Model::Material* materialDate_ = nullptr;
};

