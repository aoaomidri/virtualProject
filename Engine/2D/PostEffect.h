#pragma once
#include"GraphicsPipeline.h"
#include"math/Matrix.h"

//namespace省略
template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class PostEffect{
public:
	PostEffect() = default;
	~PostEffect() = default;
	PostEffect(const PostEffect& postEffect) = delete;
	PostEffect& operator=(const PostEffect&) = delete;

	//初期化
	void Initialize();
	//終了処理
	void Finalize();

	//現在の設定のパイプラインを送る
	void SetPipeLine();

	static PostEffect* GetInstance();

public:
	enum  EffectType {
		None,//なし
		Gray,//グレースケール
		Sepia,//セピア調
		NormalVignetting,//ヴィネッティング
		GrayVignetting,//グレーヴィネッティング
		SepiaVignetting,//セピアヴィネッティング
		VignettingGrayScale,//グレーヴィネッティング
		VignettingSepiaScale,//セピアヴィネッティング
		Smoothing3x3,//スモーシング弱
		Smoothing5x5,//スモーシング中
		Smoothing9x9,//スモーシング強
		Inverse,//色反転
		OutLine,//アウトライン
		RadialBlur,//ラジアルブラー

		Over,//これ以上ないことを表す
	};

	struct Vignetting {
		float scale;//大きさ
		float pow;//掛ける累乗
	};

	struct CameraMat {
		Matrix4x4 matProjectionInverse_;
	};
public:
	const uint32_t GetEffectType()const {
		return selectPost_;
	}
	//ヴィネッティングのResourceを送る
	const D3D12_GPU_VIRTUAL_ADDRESS GetVignetting()const {
		return vignettingResource_->GetGPUVirtualAddress();
	}

	//カメラのResourceを送る
	const D3D12_GPU_VIRTUAL_ADDRESS GetCameraMat()const {
		return cameraResource_->GetGPUVirtualAddress();
	}
public:
	void SetVignettingData(const Vignetting& data) {
		vignettingData_->scale = data.scale;
		vignettingData_->pow = data.pow;
	}

	void SetPostEffect(const EffectType& name) {
		selectPost_ = name;
	}

	void SetMatProjectionInverse(const Matrix4x4& mat) {
		cameraData_->matProjectionInverse_ = mat;
	}

public:
	bool IsSelectOutLine() const {
		if (selectPost_ == EffectType::OutLine) {
			return true;
		}
		return false;
	}

	bool IsSelectVignetting() const {
		if (selectPost_ == EffectType::NormalVignetting || selectPost_ == EffectType::GrayVignetting || selectPost_ == EffectType::SepiaVignetting) {
			return true;
		}
		return false;
	}

private:
	void CreateVignettingResource();

	void ProjectInverseResource();

private:
	//ポストエフェクト
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineCopy_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineGray_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSepia_;

	//色反転
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineInverse_;

	/*ヴィネッティング*/
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineNormalVignetting_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineGrayVignetting_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSepiaVignetting_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineVignettingGrayScale_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineVignettingSepiaScale_;

	//Smoothing
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSmoothing3x3;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSmoothing5x5;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineSmoothing9x9;

	//LuminanceOutLine
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineOutLine_;

	//ラジアンブラー
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineRadialBlur_;


	ComPtr<ID3D12Resource> textureResource;
	ComPtr<ID3D12Resource> intermediateResource;

	ComPtr<ID3D12Resource> vignettingResource_;

	Vignetting* vignettingData_ = nullptr;

	ComPtr<ID3D12Resource> cameraResource_;

	CameraMat* cameraData_ = nullptr;

	//どのポストエフェクトを選択しているか
	uint32_t selectPost_ = 0;



};

