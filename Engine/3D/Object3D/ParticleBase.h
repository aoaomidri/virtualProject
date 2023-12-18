#pragma once
#include "../../../math/Matrix.h"
#include <Windows.h>
#include <wrl.h>
#include"../Transform.h"
#include"../../Base/Log.h"
#include<vector>
#include<fstream>
#include<sstream>
#include"Model.h"
#include"../ViewProjection.h"
#include"../../../Game/random/RandomMaker.h"
#include"../../Base/GraphicsPipeline.h"
#include<numbers>
#include<list>
#include"../Shape/OBB.h"


class ParticleBase{
public:

	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	
	void Update(const Transform& transform,const ViewProjection& viewProjection);

	void Draw(D3D12_GPU_DESCRIPTOR_HANDLE TextureHandle, D3D12_GPU_DESCRIPTOR_HANDLE InstancingHandle);

	void DrawImgui();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
		ID3D12Device* device, size_t sizeInBytes);

	void makeResource();
public:

	void SetPosition(const Vector3& position) { position_ = position; }

	void SetScale(const Vector3& scale) { scale_ = scale; }

	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	
	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	const Vector3& GetPosition()const { return position_; }

	const bool& GetIsDraw()const { return isDraw_; }

	ID3D12Resource* GetInstancingResource()const { return wvpInstancingResource.Get(); }
	
public:
	Matrix4x4* parent_{};

private:
	struct Particle {
		Transform transform;	//座標
		Vector3 velocity;		//移動ベクトル
		Vector4 color;			//色
		float lifeTime;			//生存可能な時間
		float currentTime;		//発生してからの経過時間
	};

	struct ParticleRange {
		float min;
		float max;
	};

	struct Emitter {
		Transform transform;//エミッターのTransform
		int count;		//発生数
		float frequency;	//発生頻度
		float frequencyTime;//頻度用時刻
	};

	struct AccelerationField {
		Vector3 acceleration;	//加速度
		AABB area;				//範囲
	};

	ModelData MakePrimitive();

	//座標のリセット
	void PositionReset();

	//乱数による動きの変化
	void MoveChange();

	//パーティクル生成関数
	Particle MakeNewParticle(const Vector3& transform);

	std::list<Particle> Emission(const Emitter& emitter);

	bool IsCollision(const AABB& aabb, const Vector3& point);

private:
	const std::string ResourcesPath = "resources/";

	HRESULT hr;

	ID3D12Device* device_ = nullptr;

	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//頂点リソースにデータを書き込む
	VertexData* vertexDate = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;

	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleNone_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleNormal_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleAdd_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleSubtract_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleMultily_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleScreen_;

	//マテリアルにデータを書き込む
	Material* materialDate = nullptr;

	//モデル読み込み
	ModelData modelData_;

	//粒の数
	int numInstance = 0;

	static const int particleMaxNum_ = 300;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpInstancingResource;

	//データを書き込む
	ParticleForGPU* wvpData = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;

	//マテリアルにデータを書き込む
	DirectionalLight* directionalLightDate = nullptr;

	const float kDeltaTime_ = 1.0f / 60.0f;

	//データを書き込む

	Vector3 position_ = { 0.0f,0.0f ,0.0f };

	Vector3 scale_ = { 1.0f,1.0f ,1.0f };

	Vector3 rotate_ = { 0.0f,0.0f ,0.0f };

	Vector3 color_ = { 1.0f,1.0f,1.0f };


	Vector3 baseVelocity_ = { 0.0f,1.0f,0.0f };

	Vector4 chackMatrix_ = {};

	Matrix4x4 worldMatrix_{};

	Matrix4x4 backToFrontMatrix_{};

	Matrix4x4 billboardMatrix_{};

	/*Particle particles_[particleMaxNum_]{};*/
	std::list<Particle> particles_;

	Emitter emitter_{};

	AccelerationField accelerationField_{};

	Transform cameraTransform{};

	bool isDraw_ = true;

	bool isMove_ = false;

	bool isBillborad_ = false;

	bool isWind_ = false;

	ParticleRange positionRange_ = {
		.min = -3.0f,
		.max = 3.0f
	};

	ParticleRange velocityRange_ = {
		.min = -3.0f,
		.max = 3.0f
	};

	int blend_;

	//ランダム生成
	RandomMaker* random_ = nullptr;
};

