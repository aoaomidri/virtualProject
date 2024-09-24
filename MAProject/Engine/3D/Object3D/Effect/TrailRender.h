#pragma once
#include"TrailEffect.h"
class TrailRender{
public:
	//初期化処理
	void Initialize();

	void Draw(const TrailEffect* trail, const Matrix4x4& viewPro);

	void MakeResource();
private:
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineTrail_;


	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;

	//データを書き込む
	TransformationMatrix* wvpData_ = nullptr;

	HRESULT hr_;


	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	TrailEffect* trail_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルにデータを書き込む
	Model::Material* materialDate_ = nullptr;
};

