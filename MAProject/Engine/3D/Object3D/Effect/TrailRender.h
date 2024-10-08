#pragma once
#include"TrailEffect.h"
class TrailRender{
public:
	enum  EffectType {
		None,//なし
		Glound,//地の型
		Wind,//風の型
		Fire,//火の型
		Water,//水の型
		Empty,//空の型

		Over,//これ以上ないことを表す
	};
public:
	//初期化処理
	void Initialize();

	void Draw(const TrailEffect* trail, const Matrix4x4& viewPro);

	void MakeResource();

	void SetTrailColor(const EffectType& name) {
		selectTrail_ = name;
	}


private:
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineTrail_;


	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;

	//データを書き込む
	Matrix4x4* wvpData_ = nullptr;

	HRESULT hr_;


	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	TrailEffect* trail_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルにデータを書き込む
	Model::Material* materialDate_ = nullptr;

	//どの型を選択しているか
	uint32_t selectTrail_ = 0;

	/*型ごとの色を指定*/
	const Vector4 groundColor_ = { 0.8039f, 0.3608f, 0.0f,0.7f };

	const Vector4 waterColor_ = { 0.678f, 0.847f, 0.902f,0.7f };
	
};

