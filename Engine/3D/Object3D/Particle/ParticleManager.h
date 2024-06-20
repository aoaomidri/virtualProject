#pragma once
#include"RandomMaker.h"
#include"GraphicsPipeline.h"
#include<numbers>
#include<list>
#include"OBB.h"
#include"TextureManager.h"
#include"Model.h"

class ParticleManager{
public:
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager& particleManager) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	static ParticleManager* GetInstance() {
		static ParticleManager instance;
		return &instance;
	}

	struct Particle {
		EulerTransform transform;	//座標
		Vector3 velocity;		//移動ベクトル
		Vector4 color;			//色
		float lifeTime;			//生存可能な時間
		float currentTime;		//発生してからの経過時間
	};

	struct ParticleGroup {
		Model::MaterialData materialData;
		std::list<Particle> particleList;
		//インスタンシング用のgpuハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;

		ComPtr<ID3D12Resource> wvpInstancingResource;
		//データを書き込む
		ParticleForGPU* wvpData = nullptr;
		//生成する最大の個数
		int particleMaxNum_;

	};

	//初期化処理
	void Initialize();

	void CreateParticleGroup(const std::string name, const std::string textureName);

private:
	void MakeResource();

	void MakePipeLines();

	Model::ModelData MakePrimitive();

private:
	//パイプライン
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleNone_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleNormal_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleAdd_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleSubtract_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleMultily_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleScreen_;

private:
	/////Resource関係/////

	//頂点バッファービューを作成する
	ComPtr<ID3D12Resource> vertexResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//頂点リソースにデータを書き込む
	VertexData* vertexDate_ = nullptr;

	ComPtr<ID3D12Resource> materialResource_;

	//マテリアルにデータを書き込む
	Model::Material* materialDate_ = nullptr;

	//モデル読み込み
	Model::ModelData modelData_;

	

	

private:
	const std::string ResourcesPath_ = "resources/";

	HRESULT hr_;

	//粒の数
	int numInstance_ = 0;
	
	std::unordered_map<std::string, ParticleGroup>particleGroups_;

	//ランダム生成
	RandomMaker* random_ = nullptr;
	//デバイス
	ID3D12Device* device_ = nullptr;
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;
};

