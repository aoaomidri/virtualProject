#pragma once
#include "Matrix.h"
#include <Windows.h>
#include <wrl.h>
#include"Transform.h"
#include"Log.h"
#include<vector>
#include<fstream>
#include<sstream>
#include"Model.h"
#include"ViewProjection.h"
#include"RandomMaker.h"
#include"GraphicsPipeline.h"
#include<numbers>
#include<list>
#include"OBB.h"
#include"TextureManager.h"
#include"DirectionalLight.h"
/*パーティクルを生成する*/

class ParticleBase{
public:
	struct Emitter {
		EulerTransform transform;//エミッターのTransform
		int count;		//発生数
		float frequency;	//発生頻度
		float frequencyTime;//頻度用時刻
	};
public:

	void Initialize(const ParticleBase::Emitter& emitter, const bool isLoop);
	
	void Update(const EulerTransform& transform,const ViewProjection& viewProjection);

	void Draw();

	void DrawImgui(const std::string& imguiTag);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	void makeResource();
public:
	void SetAcceleration(const Vector3& acceleration) { accelerationField_.acceleration = acceleration; }

	void SetPositionRange(const Vector2& positionRamge) { positionRange_ = { positionRamge.x,positionRamge.y }; }

	void SetVelocityRange(const Vector2& velocityRange) { velocityRange_ = { velocityRange.x,velocityRange.y }; }

	void SetPosition(const Vector3& position) { position_ = position; }

	void SetScale(const Vector3& scale) { emitter_.transform.scale = scale; }

	void SetNotMove() { isMoveParticle_ = false; }

	void SetLifeTime(const float time) { lifeTime_ = time; }

	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }

	void SetAddParticle(const int32_t number) { emitter_.count = number; }
	
	void SetIsDraw(const bool& isDraw) { isDraw_ = isDraw; }

	void SetIsUpper(const bool& isUpper) { isUpper_ = isUpper; }

	void SetIsMove(const bool& isMove) { isMove_ = isMove; }

	void SetIsBillborad(const bool& isBillborad) { isBillborad_ = isBillborad; }

	void SetIsAlignedToMovement(const bool& isAlignedToMovement) { isAlignedToMovement_ = isAlignedToMovement; }

	void SetTexture(const std::string& fileName) {
		textureHandle_ = TextureManager::GetInstance()->Load(fileName);

	}

	void SetOneColor(const Vector3& color) { isOneColor_ = true; color_ = color; }

	void SetBlend(const BlendMode blend) { blend_ = blend; }

	const Vector3& GetPosition()const { return position_; }

	const bool& GetIsDraw()const { return isDraw_; }

	ID3D12Resource* GetInstancingResource()const { return wvpInstancingResource.Get(); }
	/// <summary>
	/// ループしないものに限り追加でパーティクルを発生させる
	/// </summary>
	/// <param name="emitter">設定するエミッター</param>
	void AddParticle(const ParticleBase::Emitter& emitter);
	
public:
	Matrix4x4* parent_{};

private:
	struct Particle {
		EulerTransform transform;	//座標
		Vector3 velocity;		//移動ベクトル
		Vector4 color;			//色
		float lifeTime;			//生存可能な時間
		float currentTime;		//発生してからの経過時間
	};

	struct ParticleRange {
		float min;
		float max;
	};

	struct AccelerationField {
		Vector3 acceleration;	//加速度
		AABB area;				//範囲
	};

	Model::ModelData MakePrimitive();

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

	//頂点バッファービューを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//頂点リソースにデータを書き込む
	VertexData* vertexDate = nullptr;


	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleNone_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleNormal_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleAdd_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleSubtract_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleMultily_;
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineParticleScreen_;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//マテリアルにデータを書き込む
	Model::Material* materialDate = nullptr;

	//モデル読み込み
	Model::ModelData modelData_;

	//粒の数
	int numInstance = 0;

	static const int particleMaxNum_ = 600;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpInstancingResource;

	//データを書き込む
	ParticleForGPU* wvpData = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;

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

	EulerTransform cameraTransform{};

	bool isDraw_ = true;

	bool isMove_ = false;

	bool isBillborad_ = false;

	bool isAlignedToMovement_ = false;

	bool isLoop_ = true;

	bool isOneColor_ = false;

	bool isWind_ = false;

	bool isUpper_ = false;

	bool isMoveParticle_ = false;

	ParticleRange positionRange_ = {
		.min = -3.0f,
		.max = 3.0f
	};

	ParticleRange velocityRange_ = {
		.min = -3.0f,
		.max = 3.0f
	};

	float lifeTime_ = 0;

	int blend_;

	uint32_t textureHandle_ = 0;

	//ランダム生成
	RandomMaker* random_ = nullptr;
};

